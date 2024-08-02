// SPDX-License-Identifier: MIT
/*
 * Copyright (C) 2023-2024 Mathieu Carbou
 */
#include <MycilaDimmer.h>

#include "driver/gptimer.h"

#ifdef MYCILA_LOGGER_SUPPORT
  #include <MycilaLogger.h>
extern Mycila::Logger logger;
  #define LOGD(tag, format, ...) logger.debug(tag, format, ##__VA_ARGS__)
  #define LOGI(tag, format, ...) logger.info(tag, format, ##__VA_ARGS__)
  #define LOGW(tag, format, ...) logger.warn(tag, format, ##__VA_ARGS__)
  #define LOGE(tag, format, ...) logger.error(tag, format, ##__VA_ARGS__)
#else
  #define LOGD(tag, format, ...) ESP_LOGD(tag, format, ##__VA_ARGS__)
  #define LOGI(tag, format, ...) ESP_LOGI(tag, format, ##__VA_ARGS__)
  #define LOGW(tag, format, ...) ESP_LOGW(tag, format, ##__VA_ARGS__)
  #define LOGE(tag, format, ...) ESP_LOGE(tag, format, ##__VA_ARGS__)
#endif

#ifndef GPIO_IS_VALID_OUTPUT_GPIO
  #define GPIO_IS_VALID_OUTPUT_GPIO(gpio_num) ((gpio_num >= 0) && \
                                               (((1ULL << (gpio_num)) & SOC_GPIO_VALID_OUTPUT_GPIO_MASK) != 0))
#endif

#ifndef GPIO_IS_VALID_GPIO
  #define GPIO_IS_VALID_GPIO(gpio_num) ((gpio_num >= 0) && \
                                        (((1ULL << (gpio_num)) & SOC_GPIO_VALID_GPIO_MASK) != 0))
#endif

#define TAG "DIMMER"

// Minimum delay to reach the voltage required for a gate current of 30mA.
// delay_us = asin((gate_resistor * gate_current) / grid_volt_max) / pi * period_us
// delay_us = asin((330 * 0.03) / 325) / pi * 10000 = 97us
#define PHASE_DELAY_MIN_US (100)

#define TABLE_PHASE_LEN (80U)

//////////////////////////////////////////////////////////////////////////
// Static members
//////////////////////////////////////////////////////////////////////////

static const uint32_t DIMMER_MAX = (1 << MYCILA_DIMMER_RESOLUTION) - 1;
static const uint32_t TABLE_PHASE_SCALE = (TABLE_PHASE_LEN - 1U) * (1UL << (16 - MYCILA_DIMMER_RESOLUTION));
static const uint16_t TABLE_PHASE_DELAY[TABLE_PHASE_LEN] PROGMEM = {0xefea, 0xdfd4, 0xd735, 0xd10d, 0xcc12, 0xc7cc, 0xc403, 0xc094, 0xbd6a, 0xba78, 0xb7b2, 0xb512, 0xb291, 0xb02b, 0xaddc, 0xaba2, 0xa97a, 0xa762, 0xa557, 0xa35a, 0xa167, 0x9f7f, 0x9da0, 0x9bc9, 0x99fa, 0x9831, 0x966e, 0x94b1, 0x92f9, 0x9145, 0x8f95, 0x8de8, 0x8c3e, 0x8a97, 0x88f2, 0x8750, 0x85ae, 0x840e, 0x826e, 0x80cf, 0x7f31, 0x7d92, 0x7bf2, 0x7a52, 0x78b0, 0x770e, 0x7569, 0x73c2, 0x7218, 0x706b, 0x6ebb, 0x6d07, 0x6b4f, 0x6992, 0x67cf, 0x6606, 0x6437, 0x6260, 0x6081, 0x5e99, 0x5ca6, 0x5aa9, 0x589e, 0x5686, 0x545e, 0x5224, 0x4fd5, 0x4d6f, 0x4aee, 0x484e, 0x4588, 0x4296, 0x3f6c, 0x3bfd, 0x3834, 0x33ee, 0x2ef3, 0x28cb, 0x202c, 0x1016};

static uint16_t _lookupPhaseDelay(uint32_t duty, uint16_t period) {
  uint32_t slot = duty * TABLE_PHASE_SCALE + (TABLE_PHASE_SCALE >> 1);
  uint16_t index = slot >> 16;
  uint32_t a = TABLE_PHASE_DELAY[index];
  uint32_t b = TABLE_PHASE_DELAY[index + 1];
  uint32_t delay = a - (((a - b) * (slot & 0xffff)) >> 16); // interpolate a b
  return (delay * period) >> 16;                            // scale to period
}

Mycila::Dimmer* Mycila::Dimmer::_root = nullptr;

//////////////////////////////////////////////////////////////////////////
// ZCD
//////////////////////////////////////////////////////////////////////////

bool Mycila::Dimmer::_zcdEnabled = false;
gpio_num_t Mycila::Dimmer::_zcPin = GPIO_NUM_NC;
uint32_t Mycila::Dimmer::_zcSemiPeriod = 0;
uint32_t Mycila::Dimmer::_zcDelay = 0;
uint32_t Mycila::Dimmer::_zcMinimumPeriod = 0;
uint32_t Mycila::Dimmer::_zcMaximumPeriod = 0;
hw_timer_t* Mycila::Dimmer::_timer = nullptr;

void Mycila::Dimmer::beginZCD(int8_t pin, uint32_t semiPeriod, uint32_t delay) {
  if (_zcdEnabled)
    return;

  if (!GPIO_IS_VALID_GPIO(pin)) {
    LOGE(TAG, "Invalid Zero-Cross Detection pin: %" PRId8, pin);
    return;
  }

  if (!semiPeriod) {
    LOGE(TAG, "Invalid Zero-Cross Detection semi-period: %" PRIu32 " us", pin, semiPeriod);
    return;
  }

  if (delay >= semiPeriod * 8 / 10) {
    LOGE(TAG, "Invalid Zero-Cross Detection delay: %" PRIu32 " us", delay);
    return;
  }

  LOGI(TAG, "Enable Zero-Cross Detection on pin %" PRId8 " with semi-period %" PRIu32 " us and delay %" PRIu32 " us", _zcPin, semiPeriod, delay);

  _zcPin = (gpio_num_t)pin;
  _zcSemiPeriod = semiPeriod;
  _zcMinimumPeriod = semiPeriod * 8 / 10;  // 80% of semi-period
  _zcMaximumPeriod = semiPeriod * 12 / 10; // 120% of semi-period
  _zcDelay = delay < PHASE_DELAY_MIN_US ? 0 : delay;
  _zcdEnabled = true;

  attachInterruptArg(_zcPin, _zeroCrossISR, nullptr, RISING);
  _timer = timerBegin(1000000);
  assert(_timer);
  timerAttachInterrupt(_timer, _timerISR);
}

void Mycila::Dimmer::endZCD() {
  if (!_zcdEnabled)
    return;

  LOGI(TAG, "Disable Zero-Cross Detection on pin %" PRId8, _zcPin);
  _zcdEnabled = false;
  detachInterrupt(_zcPin);
  timerDetachInterrupt(_timer);
  timerEnd(_timer);
  _timer = nullptr;

  LOGI(TAG, "Disable all dimmers");
  for (Dimmer* d = _root; d; d = d->_next) {
    d->_disable();
  }
}

//////////////////////////////////////////////////////////////////////////
// Dimmer
//////////////////////////////////////////////////////////////////////////

Mycila::Dimmer::Dimmer() {
  if (_root) {
    _prev = _root;
    _root->_next = this;
  } else {
    _root = this;
  }
}

Mycila::Dimmer::~Dimmer() {
  endDimmer();
  if (_root == this) {
    _root = _next;
  } else {
    _prev->_next = _next;
    if (_next)
      _next->_prev = _prev;
  }
}

void Mycila::Dimmer::beginDimmer(int8_t pin) {
  if (_dimmerEnabled)
    return;

  if (GPIO_IS_VALID_OUTPUT_GPIO(pin)) {
    _dimmerPin = (gpio_num_t)pin;
  } else {
    LOGE(TAG, "Disable Dimmer: Invalid pin: %" PRId8, pin);
    return;
  }

  LOGI(TAG, "Enable Dimmer on pin %" PRId8, _dimmerPin);

  pinMode(_dimmerPin, OUTPUT);
  digitalWrite(_dimmerPin, LOW);
  _dimmerEnabled = true;

  setDutyCycle(_dutyCycleMin);
}

void Mycila::Dimmer::endDimmer() {
  if (!_dimmerEnabled)
    return;

  LOGI(TAG, "Disable Dimmer on pin %" PRId8, _dimmerPin);
  _dimmerEnabled = false;
  _disable();
}

void Mycila::Dimmer::setDutyCycle(const float newDutyCycle) {
  if (!_dimmerEnabled)
    return;

  if (!_zcdEnabled) {
    _dutyCycle = 0;
    _delay = UINT32_MAX;
    return;
  }

  // apply limit
  _dutyCycle = constrain(newDutyCycle, 0, _dutyCycleLimit);

  if (!_zcdEnabled) {
    _disable();
    return;
  }

  // duty remapping (equivalent to Shelly Dimmer remapping feature)
  const float mappedDutyCycle = _dutyCycleMin + _dutyCycle * (_dutyCycleMax - _dutyCycleMin);
  // ESP_LOGD(TAG, "Dimmer %" PRIu8 " duty cycle %f mapped to %f", _dimmerPin, newDutyCycle, mappedDutyCycle);

  if (mappedDutyCycle == 0) {
    _delay = UINT32_MAX;

  } else if (mappedDutyCycle == 1) {
    _delay = _zcDelay + PHASE_DELAY_MIN_US;

  } else {
    _delay = _zcDelay + _lookupPhaseDelay(mappedDutyCycle * DIMMER_MAX, _zcSemiPeriod);
  }
}

void Mycila::Dimmer::setDutyCycleLimit(float limit) {
  _dutyCycleLimit = constrain(limit, 0, 1);
  LOGD(TAG, "Set dimmer %" PRId8 " duty cycle limit to %f", _dimmerPin, _dutyCycleLimit);
  if (_dutyCycle > _dutyCycleLimit)
    setDutyCycle(_dutyCycleLimit);
}

void Mycila::Dimmer::setDutyCycleMin(float min) {
  _dutyCycleMin = constrain(min, 0, _dutyCycleMax);
  LOGD(TAG, "Set dimmer %" PRId8 " duty cycle min to %f", _dimmerPin, _dutyCycleMin);
  setDutyCycle(_dutyCycle);
}

void Mycila::Dimmer::setDutyCycleMax(float max) {
  _dutyCycleMax = constrain(max, _dutyCycleMin, 1);
  LOGD(TAG, "Set dimmer %" PRId8 " duty cycle max to %f", _dimmerPin, _dutyCycleMax);
  setDutyCycle(_dutyCycle);
}

void Mycila::Dimmer::_disable() {
  if (_dimmerEnabled) {
    _dutyCycle = 0;
    _delay = UINT32_MAX;
    digitalWrite(_dimmerPin, LOW);
  }
}

//////////////////////////////////////////////////////////////////////////
// ISR
//////////////////////////////////////////////////////////////////////////

void ARDUINO_ISR_ATTR Mycila::Dimmer::_zeroCrossISR(void* arg) {
  uint32_t elapsed = timerRead(_timer);
  // filter spurious zero-crossing events
  if (elapsed < _zcMinimumPeriod) {
    return;
  }

  timerRestart(_timer);

  for (Dimmer* d = _root; d; d = d->_next) {
  }

  // sync the alarm counter with the zero-crossing event
  // the timer counter measures the time since the last zero-crossing event (which can happen before the real zero-crossing in case of a delay)
  timerWrite(_timer, elapsed);
  // _manageDimmers(elapsed);
}

void ARDUINO_ISR_ATTR Mycila::Dimmer::_timerISR() {
  uint32_t timerElapsed = timerRead(_timer);
}
