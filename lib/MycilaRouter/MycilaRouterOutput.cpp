// SPDX-License-Identifier: MIT
/*
 * Copyright (C) 2023-2024 Mathieu Carbou and others
 */
#include <MycilaRouterOutput.h>

#include <MycilaGrid.h>
#include <MycilaLogger.h>
#include <MycilaNTP.h>
#include <MycilaTime.h>

#define TAG "OUTPUT"

extern Mycila::Logger logger;

static const char* RouterOutputStateNames[] = {
  "Disabled",
  "Idle",
  "Routing",
  "Bypass",
  "Bypass Auto",
};

static const char* DaysOfWeek[] = {"sun", "mon", "tue", "wed", "thu", "fri", "sat"};

void Mycila::RouterOutput::applyDimmerLimit() {
  if (!_dimmer->isEnabled())
    return;
  if (_autoBypassEnabled)
    return;
  if (_dimmer->isOff())
    return;
  if (_bypassEnabled)
    return;
  uint8_t limit = RouterOutputConfig.getDimmerLevelLimit(_name);
  if (_dimmer->getLevel() > limit) {
    logger.warn(TAG, "Dimmer '%s' reached its limit at %" PRIu8, _name, limit);
    _dimmer->setLevel(limit);
  }
}

bool Mycila::RouterOutput::tryBypassRelayState(bool switchOn) {
  if (_autoBypassEnabled && !switchOn) {
    logger.warn(TAG, "Auto Bypass '%s' is activated: unable to turn of bypass relay", _name);
    return false;
  }
  _setBypassRelay(switchOn);
  return true;
}

bool Mycila::RouterOutput::tryDimmerLevel(uint8_t level) {
  if (!_dimmer->isEnabled())
    return false;

  if (_autoBypassEnabled) {
    logger.warn(TAG, "Auto Bypass '%s' is activated: unable to change dimmer level", _name);
    return false;
  }

  if (RouterOutputConfig.isAutoDimmerEnabled(_name)) {
    logger.warn(TAG, "Auto Dimmer '%s' is activated: unable to change dimmer level", _name);
    return false;
  }

  uint8_t limit = RouterOutputConfig.getDimmerLevelLimit(_name);

  if (level > limit) {
    level = limit;
  }

  logger.debug(TAG, "Setting Dimmer '%s' level to %" PRIu8 "...", _name, level);
  _setBypassRelay(false, level);
  return true;
}

Mycila::RouterOutputState Mycila::RouterOutput::getState() const {
  if (!_dimmer->isEnabled() && !_relay->isEnabled())
    return RouterOutputState::OUTPUT_DISABLED;
  if (_dimmer->getLevel() > 0)
    return RouterOutputState::OUTPUT_ROUTING;
  // dimmer level is 0
  if (_autoBypassEnabled)
    return RouterOutputState::OUTPUT_BYPASS_AUTO;
  if (_relay->isOn())
    return RouterOutputState::OUTPUT_BYPASS_MANUAL;
  return RouterOutputState::OUTPUT_IDLE;
}

const char* Mycila::RouterOutput::getStateString() const { return RouterOutputStateNames[static_cast<int>(getState())]; }

void Mycila::RouterOutput::toJson(const JsonObject& root) const {
  root["apparent_power"] = getApparentPower();
  root["current"] = _current;
  root["enabled"] = _dimmer->isEnabled();
  root["energy"] = _energy;
  root["power_factor"] = getPowerFactor();
  root["power"] = getActivePower();
  root["resistance"] = getResistance();
  root["state"] = RouterOutputStateNames[static_cast<int>(getState())];
  root["thdi"] = getTHDi();
  root["voltage_in"] = _inputVoltage;
  root["voltage_out"] = _outputVoltage;
}

void Mycila::RouterOutput::autoBypass() {
  if (!RouterOutputConfig.isAutoBypassEnabled(_name)) {
    if (_autoBypassEnabled) {
      logger.warn(TAG, "Auto Bypass disabled: stopping Auto Bypass '%s'...", _name);
      _autoBypassEnabled = false;
      _setBypassRelay(false);
    }
    return;
  }

  // dimmer & relay checks

  if (!_relay->isEnabled() && !_dimmer->isEnabled()) {
    if (_autoBypassEnabled) {
      logger.warn(TAG, "Relay and dimmer disabled: stopping Auto Bypass '%s'...", _name);
      _autoBypassEnabled = false;
      _setBypassRelay(false);
    }
    return;
  }

  // time checks

  if (!NTP.isSynced()) {
    if (_autoBypassEnabled) {
      logger.warn(TAG, "NTP not available: stopping Auto Bypass '%s'...", _name);
      _autoBypassEnabled = false;
      _setBypassRelay(false);
    }
    return;
  }

  struct tm timeInfo;
  if (!getLocalTime(&timeInfo, 5)) {
    if (_autoBypassEnabled) {
      logger.warn(TAG, "Unable to get time: stopping Auto Bypass '%s'...", _name);
      _autoBypassEnabled = false;
      _setBypassRelay(false);
    }
    return;
  }

  // temperature checks

  const float temp = _temperatureSensor->getLastTemperature();

  if (_temperatureSensor->isEnabled()) {
    if (_temperatureSensor->getLastTime() == 0) {
      if (_autoBypassEnabled) {
        logger.warn(TAG, "Temperature sensor is not answering: stopping Auto Bypass '%s'...", _name);
        _autoBypassEnabled = false;
        _setBypassRelay(false);
      }
      return;
    }

    if (temp >= RouterOutputConfig.getAutoStopTemperature(_name)) {
      if (_autoBypassEnabled) {
        logger.info(TAG, "Temperature reached %.02f °C: stopping Auto Bypass '%s'...", temp, _name);
        _autoBypassEnabled = false;
        _setBypassRelay(false);
      }
      return;
    }

    if (temp >= RouterOutputConfig.getAutoStartTemperature(_name)) {
      // temperature OK, no need to start
      return;
    }
  }

  const String autoStart = RouterOutputConfig.getAutoStartTime(_name);
  const String autoStop = RouterOutputConfig.getAutoStopTime(_name);
  const int inRange = Time::timeInRange(timeInfo, autoStart, autoStop);
  if (inRange == -1) {
    if (_autoBypassEnabled) {
      logger.warn(TAG, "Time range %s to %s is invalid: stopping Auto Bypass '%s'...", autoStart.c_str(), autoStop.c_str(), _name);
      _autoBypassEnabled = false;
      _setBypassRelay(false);
    }
    return;
  }

  if (!inRange) {
    if (_autoBypassEnabled) {
      logger.info(TAG, "Time reached %s: stopping Auto Bypass '%s'...", autoStop.c_str(), _name);
      _autoBypassEnabled = false;
      _setBypassRelay(false);
    }
    return;
  }

  // time and temp OK, let's start
  if (!_autoBypassEnabled) {
    // auto bypass is not enabled, let's start it
    const char* wday = DaysOfWeek[timeInfo.tm_wday];
    if (RouterOutputConfig.getWeekDays(_name).indexOf(wday) >= 0) {
      logger.info(TAG, "Time within %s-%s on %s: starting Auto Bypass '%s' at %.02f °C...", autoStart.c_str(), autoStop.c_str(), wday, _name, temp);
      _autoBypassEnabled = true;
      _setBypassRelay(true);
    }
    return;
  }

  // auto bypass is enabled

  // relay is on ?
  if (_relay->isOn())
    return;

  // or relay is disabled and dimmer at full power tu replace it ?
  if (!_relay->isEnabled() && _dimmer->isOnAtFullPower())
    return;

  // start bypass
  logger.info(TAG, "Auto Bypass '%s' is activated: restarting Relay...", _name);
  _setBypassRelay(true);
}

void Mycila::RouterOutput::_setBypassRelay(bool state, uint8_t dimmerLevelWhenRelayOff) {
  if (_relay->isEnabled()) {
    if (state)
      _dimmer->off();
    if (state ^ _relay->isOn()) {
      logger.debug(TAG, "Turning %s Bypass Relay '%s'...", state ? "on" : "off", _name);
      _relay->setState(state);
    }
    if (!state)
      _dimmer->setLevel(dimmerLevelWhenRelayOff);
  } else {
    logger.debug(TAG, "Turning %s Dimmer '%s'...", state ? "on" : "off", _name);
    _dimmer->setLevel(state ? 100 : dimmerLevelWhenRelayOff);
    _bypassEnabled = state;
  }
}

void Mycila::RouterOutput::updateElectricityStatistics() {
  _energy = _pzem->getEnergy();
  _current = _pzem->getCurrent();
  _inputVoltage = _pzem->getVoltage();
  if (_inputVoltage == 0)
    _inputVoltage = Grid.getVoltage();
  _outputVoltage = _dimmer->computeDimmedVoltage(_inputVoltage);
}

namespace Mycila {
  RouterOutputConfigClass RouterOutputConfig;
} // namespace Mycila
