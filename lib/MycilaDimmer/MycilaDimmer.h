// SPDX-License-Identifier: MIT
/*
 * Copyright (C) 2023-2024 Mathieu Carbou
 */
#pragma once

#include <WString.h>
#include <esp32-hal-gpio.h>
#include <esp32-hal-timer.h>

#ifdef MYCILA_JSON_SUPPORT
  #include <ArduinoJson.h>
#endif

/**
 * Optional resolution, 15bits max
 */
#ifndef MYCILA_DIMMER_RESOLUTION
  #define MYCILA_DIMMER_RESOLUTION 12
#endif

namespace Mycila {
  class Dimmer {
    public:
      //////////////////////////////////////////////////////////////////////////
      // ZCD
      //////////////////////////////////////////////////////////////////////////

      /**
       * @brief Enable Zero-Cross Detection
       *
       * @param pin: the GPIO pin to use for ZCD
       * @param semiPeriod: the semi-period in us for the ZCD\: 10000 us for 50Hz, 8333 us for 60Hz, or an automatically detected value with MycilaPulseAnalyzer library.
       * @param delay: the delay in us between the rising edge of the ZC pulse and the real ZC point.
       * Most ZC modules are detecting the ZC point before the real 0 V and create a pulse which is centered around the 0 V value.
       * MycilaPulseAnalyzer library can analyze the pulse length.
       * For example, if the pulse length is 1200 us, the delay to set is 600 us (half the pulse length).
       * When using a ZCD module based on the BM1Z102FJ chip, the delay must be set to 0 us because the pulse length is the same as the semi-period.
       *
       * @warning ZCD won't start if semi-period is 0 or pin is invalid
       */
      static void beginZCD(int8_t pin, uint32_t semiPeriod, uint32_t delay = 0);

      /**
       * @brief Disable Zero-Cross Detection.
       *
       * @warning Dimmers won't be destroyed but won't turn on anymore even is a duty cycle is set to them.
       */
      static void endZCD();

      /**
       * @brief Check if ZCD is enabled and listening for zero-crossing events
       */
      static bool isZCDEnabled() { return _zcdEnabled; }

      /**
       * @brief Get the GPIO pin used for ZCD
       */
      static gpio_num_t getZCPin() { return _zcPin; }

      /**
       * @brief Get the semi-period in us used for ZCD
       */
      static uint32_t getZCSemiPeriod() { return _zcSemiPeriod; }

      /**
       * @brief Get the delay between the rising edge of the ZC pulse and the real ZC point
       */
      static uint32_t getZCDelay() { return _zcDelay; }

#ifdef MYCILA_JSON_SUPPORT
      /**
       * @brief Serialize ZCD information to a JSON object
       *
       * @param root: the JSON object to serialize to
       */
      static void zcdToJson(const JsonObject& root) {
        root["enabled"] = _zcdEnabled;
        root["pin"] = static_cast<int>(_zcPin);
        root["semi_period"] = _zcSemiPeriod;
        root["delay"] = _zcDelay;
      }
#endif

      //////////////////////////////////////////////////////////////////////////
      // Dimmer
      //////////////////////////////////////////////////////////////////////////

      Dimmer();
      ~Dimmer();

      /**
       * @brief Enable a dimmer on a specific GPIO pin
       *
       * @param pin: the GPIO pin to use for the dimmer
       *
       * @warning Dimmer won't be enabled if pin is invalid
       * @warning Dimmer won't be activated until the ZCD is enabled
       */
      void beginDimmer(const int8_t pin);

      /**
       * @brief Disable the dimmer
       *
       * @warning Dimmer won't be destroyed but won't turn on anymore even is a duty cycle is set.
       */
      void endDimmer();

      /**
       * @brief Check if the dimmer is enabled
       */
      bool isDimmerEnabled() const { return _dimmerEnabled; }

      /**
       * @brief Get the GPIO pin used for the dimmer
       */
      gpio_num_t getDimmerPin() const { return _dimmerPin; }

#ifdef MYCILA_JSON_SUPPORT
      /**
       * @brief Serialize Dimmer information to a JSON object
       *
       * @param root: the JSON object to serialize to
       */
      void dimmerToJson(const JsonObject& root) const {
        const float angle = getPhaseAngle();
        root["enabled"] = _dimmerEnabled;
        root["state"] = isOn() ? "on" : "off";
        root["angle_d"] = angle * RAD_TO_DEG;
        root["angle"] = angle;
        root["delay"] = _delay;
        root["duty_cycle"] = _dutyCycle;
        root["duty_cycle_limit"] = _dutyCycleLimit;
        root["duty_cycle_min"] = _dutyCycleMin;
        root["duty_cycle_max"] = _dutyCycleMax;
      }
#endif

      /**
       * @brief Turn on the dimmer at full power
       */
      void on() { setDutyCycle(1); }

      /**
       * @brief Turn off the dimmer
       */
      void off() { setDutyCycle(0); }

      /**
       * @brief Check if the dimmer is off
       */
      bool isOff() const { return _dutyCycle <= _dutyCycleMin; }

      /**
       * @brief Check if the dimmer is on
       */
      bool isOn() const { return _dutyCycle > _dutyCycleMin; }

      /**
       * @brief Check if the dimmer is on at full power
       */
      bool isOnAtFullPower() const { return _dutyCycle >= _dutyCycleMax; }

      /**
       * @brief Set the power duty
       *
       * @param dutyCycle: the power duty cycle in the range [0.0, 1.0]
       */
      void setDutyCycle(float dutyCycle);

      /**
       * @brief Set the power duty cycle limit of the dimmer. The duty cycle will be clamped to this limit.
       *
       * @param limit: the power duty cycle limit in the range [0.0, 1.0]
       */
      void setDutyCycleLimit(float limit);

      /**
       * @brief Duty remapping (equivalent to Shelly Dimmer remapping feature).
       * Useful to calibrate the dimmer when using for example a PWM signal to 0-10V analog convertor connected to a voltage regulator which is only working in a specific voltage range like 1-8V.
       *
       * @param min: Set the new "0" value for the power duty cycle. The duty cycle in the range [0.0, 1.0] will be remapped to [min, max].
       */
      void setDutyCycleMin(float min);

      /**
       * @brief Duty remapping (equivalent to Shelly Dimmer remapping feature).
       * Useful to calibrate the dimmer when using for example a PWM signal to 0-10V analog convertor connected to a voltage regulator which is only working in a specific voltage range like 1-8V.
       *
       * @param max: Set the new "1" value for the power duty cycle. The duty cycle in the range [0.0, 1.0] will be remapped to [min, max].
       */
      void setDutyCycleMax(float max);

      /**
       * @brief Get the power duty cycle of the dimmer
       */
      float getDutyCycle() const { return _dutyCycle; }

      /**
       * @brief Get the power duty cycle limit of the dimmer
       */
      float getDutyCycleLimit() const { return _dutyCycleLimit; }

      /**
       * @brief Get the remapped "0" of the dimmer duty cycle
       */
      float getDutyCycleMin() const { return _dutyCycleMin; }

      /**
       * @brief Get the remapped "1" of the dimmer duty cycle
       */
      float getDutyCycleMax() const { return _dutyCycleMax; }

      /**
       * @brief Get the firing delay in us of the dimmer in the range [0, semi-period]
       * At 0% power, delay is equal to the semi-period.
       * At 100% power, the delay is 0 us
       * If the firing delay is UINT32_MAX, the dimmer is off
       */
      uint32_t getFiringDelay() const { return _delay; }

      /**
       * @brief Get the phase angle in radians of the dimmer in the range [0, PI]
       * At 0% power, the phase angle is equal to PI
       * At 100% power, the phase angle is equal to 0
       */
      float getPhaseAngle() const { return _delay >= _zcSemiPeriod ? PI : PI * _delay / _zcSemiPeriod; }

    private:
      // --------------------------------------------------------------------
      // zcd
      // --------------------------------------------------------------------

      static bool _zcdEnabled;
      static gpio_num_t _zcPin;
      static uint32_t _zcSemiPeriod;
      static uint32_t _zcDelay;

      // --------------------------------------------------------------------
      // dimmers
      // --------------------------------------------------------------------

      bool _dimmerEnabled = false;
      gpio_num_t _dimmerPin = GPIO_NUM_NC;
      float _dutyCycle = 0;
      float _dutyCycleLimit = 1;
      float _dutyCycleMin = 0;
      float _dutyCycleMax = 1;
      void _disable();
      // alarm delay in us since the zero-crossing pulse's rising edge (not the real 0 V crossing, which can happen later)
      // dimmer will be activated for this duration if its delay is not UINT32_MAX
      volatile uint32_t _delay = UINT32_MAX;

      // --------------------------------------------------------------------
      // chain of dimmers
      // --------------------------------------------------------------------

      static Dimmer* _root;
      Dimmer* _prev = nullptr;
      Dimmer* _next = nullptr;

      // --------------------------------------------------------------------
      // zero-crossing detection and dimmer firing
      // --------------------------------------------------------------------

      // used to filter spurious zero-crossing events: it is expected that ZC events are within [80% - 120%] of the semi-period
      static uint32_t _zcMinimumPeriod;
      static uint32_t _zcMaximumPeriod;
      // ISR used to synchronize the timer with the zero-crossing event
      static void _zeroCrossISR(void* arg);
      // ISR + timer handle used to fire the dimmers
      static hw_timer_t* _timer;
      // alarm triggered each time an action must be taken on the dimmers
      static void _timerISR();
  };
} // namespace Mycila
