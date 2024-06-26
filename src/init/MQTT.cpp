// SPDX-License-Identifier: GPL-3.0-or-later
/*
 * Copyright (C) 2023-2024 Mathieu Carbou and others
 */
#include <YaSolR.h>

Mycila::Task initMqttSubscribersTask("Init MQTT Subscribers", [](void* params) {
  logger.info(TAG, "Initializing MQTT Subscribers...");

  const String baseTopic = config.get(KEY_MQTT_TOPIC);

  // config

  mqtt.subscribe(baseTopic + "/config/+/set", [](const String& topic, const String& payload) {
    const int end = topic.lastIndexOf("/");
    const int start = topic.lastIndexOf("/", end - 1);
    const String key = topic.substring(start + 1, end);
    const char* keyRef = config.keyRef(key.c_str());
    if (keyRef)
      config.set(keyRef, payload);
  });

  // relays

  mqtt.subscribe(baseTopic + "/router/relay1/state/set", [](const String& topic, const String& payload) {
    if (relay1.isEnabled()) {
      int start = payload.indexOf("=");
      String state = start >= 0 ? payload.substring(0, start) : payload;
      uint32_t duration = start >= 0 ? payload.substring(start + 1).toInt() : 0;
      if (state == YASOLR_ON)
        routerRelay1.tryRelayState(true, duration);
      else if (state == YASOLR_OFF)
        routerRelay1.tryRelayState(false, duration);
    }
  });
  mqtt.subscribe(baseTopic + "/router/relay2/state/set", [](const String& topic, const String& payload) {
    if (relay2.isEnabled()) {
      int start = payload.indexOf("=");
      String state = start >= 0 ? payload.substring(0, start) : payload;
      uint32_t duration = start >= 0 ? payload.substring(start + 1).toInt() : 0;
      if (state == YASOLR_ON)
        routerRelay2.tryRelayState(true, duration);
      else if (state == YASOLR_OFF)
        routerRelay2.tryRelayState(false, duration);
    }
  });

  // router

  mqtt.subscribe(baseTopic + "/router/output1/dimmer/duty/set", [](const String& topic, const String& payload) {
    output1.tryDimmerDuty(payload.toInt());
  });

  mqtt.subscribe(baseTopic + "/router/output2/dimmer/duty/set", [](const String& topic, const String& payload) {
    output2.tryDimmerDuty(payload.toInt());
  });

  mqtt.subscribe(baseTopic + "/router/output1/dimmer/duty_cycle/set", [](const String& topic, const String& payload) {
    output1.tryDimmerDutyCycle(payload.toFloat());
  });

  mqtt.subscribe(baseTopic + "/router/output2/dimmer/duty_cycle/set", [](const String& topic, const String& payload) {
    output2.tryDimmerDutyCycle(payload.toFloat());
  });

  mqtt.subscribe(baseTopic + "/router/output1/bypass/set", [](const String& topic, const String& payload) {
    if (output1.isBypassEnabled()) {
      if (payload == YASOLR_ON)
        output1.tryBypassState(true);
      else if (payload == YASOLR_OFF)
        output1.tryBypassState(false);
    }
  });

  mqtt.subscribe(baseTopic + "/router/output2/bypass/set", [](const String& topic, const String& payload) {
    if (output2.isBypassEnabled()) {
      if (payload == YASOLR_ON)
        output2.tryBypassState(true);
      else if (payload == YASOLR_OFF)
        output2.tryBypassState(false);
    }
  });

  // device

  mqtt.subscribe(baseTopic + "/system/device/restart", [](const String& topic, const String& payload) {
    restartTask.resume();
  });

  // grid power
  String gridPowerMQTTTopic = config.get(KEY_GRID_POWER_MQTT_TOPIC);
  if (!gridPowerMQTTTopic.isEmpty()) {
    logger.info(TAG, "Reading Grid Power from MQTT topic: %s", gridPowerMQTTTopic.c_str());
    // initialize to 0 in order to make Grid object aware that we will use MQTT and initialise the expiration
    grid.updateMQTTGridPower(0);
    mqtt.subscribe(gridPowerMQTTTopic.c_str(), [](const String& topic, const String& payload) {
      float p = payload.toFloat();
      logger.info(TAG, "Grid Power from MQTT: %f", p);
      if (grid.updateMQTTGridPower(p))
        routingTask.resume();
    });
  }

  // grid voltage
  String gridVoltageMQTTTopic = config.get(KEY_GRID_VOLTAGE_MQTT_TOPIC);
  if (!gridVoltageMQTTTopic.isEmpty()) {
    // initialize to 0 in order to make Grid object aware that we will use MQTT and initialise the expiration
    grid.updateMQTTGridVoltage(0);
    logger.info(TAG, "Reading Grid Voltage from MQTT topic: %s", gridVoltageMQTTTopic.c_str());
    mqtt.subscribe(gridVoltageMQTTTopic.c_str(), [](const String& topic, const String& payload) {
      float v = payload.toFloat();
      logger.info(TAG, "Grid Voltage from MQTT: %f", v);
      grid.updateMQTTGridVoltage(v);
    });
  }
});
