// SPDX-License-Identifier: GPL-3.0-or-later
/*
 * Copyright (C) 2023-2024 Mathieu Carbou
 */
#include <YaSolR.h>

Mycila::Task dimmer1Task("Dimmer 1", Mycila::TaskType::ONCE, [](void* params) {
  if (dimmerO1.isEnabled()) {
    dimmerO1.end();
    delay(10);
  }
  dimmerO1.setDutyCycleMin(config.get(KEY_OUTPUT1_DIMMER_MIN).toFloat() / 100);
  dimmerO1.setDutyCycleMax(config.get(KEY_OUTPUT1_DIMMER_MAX).toFloat() / 100);
  dimmerO1.setDutyCycleLimit(config.get(KEY_OUTPUT1_DIMMER_LIMIT).toFloat() / 100);
  if (config.getBool(KEY_ENABLE_OUTPUT1_DIMMER) && zcd.isEnabled()) {
    dimmerO1.begin(config.get(KEY_PIN_OUTPUT1_DIMMER).toInt(), zcd.getSemiPeriod());
  }
});

Mycila::Task dimmer2Task("Dimmer 2", Mycila::TaskType::ONCE, [](void* params) {
  if (dimmerO2.isEnabled()) {
    dimmerO2.end();
    delay(10);
  }
  dimmerO2.setDutyCycleMin(config.get(KEY_OUTPUT2_DIMMER_MIN).toFloat() / 100);
  dimmerO2.setDutyCycleMax(config.get(KEY_OUTPUT2_DIMMER_MAX).toFloat() / 100);
  dimmerO2.setDutyCycleLimit(config.get(KEY_OUTPUT2_DIMMER_LIMIT).toFloat() / 100);
  if (config.getBool(KEY_ENABLE_OUTPUT2_DIMMER) && zcd.isEnabled()) {
    dimmerO2.begin(config.get(KEY_PIN_OUTPUT2_DIMMER).toInt(), zcd.getSemiPeriod());
  }
});