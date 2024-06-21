// SPDX-License-Identifier: GPL-3.0-or-later
/*
 * Copyright (C) 2023-2024 Mathieu Carbou and others
 */
#include <YaSolR.h>

#ifndef ESPCONNECT_NO_MDNS
  #include <ESPmDNS.h>
#endif

Mycila::Task networkManagerTask("ESPConnect", [](void* params) { ESPConnect.loop(); });

Mycila::Task networkServiceTask("NetworkServices", Mycila::TaskType::ONCE, [](void* params) {
  logger.info("YASOLR", "Enable Network Services...");

  // Web server
  webServer.begin();
  webServer.onNotFound([](AsyncWebServerRequest* request) {
    request->send(404);
  });

  if (!config.getBool(KEY_ENABLE_AP_MODE)) {
    Mycila::NTP.sync(config.get(KEY_NTP_SERVER));
#ifndef ESPCONNECT_NO_MDNS
    MDNS.addService("http", "tcp", 80);
#endif
    mqttConfigTask.resume();
  }
});

Mycila::Task mqttConfigTask("MQTT Config", Mycila::TaskType::ONCE, [](void* params) {
  mqtt.end();
  if (!config.getBool(KEY_ENABLE_AP_MODE) && config.getBool(KEY_ENABLE_MQTT)) {
    logger.info("YASOLR", "Enable MQTT...");

    bool secured = config.getBool(KEY_MQTT_SECURED);
    String serverCert = "";

    if (secured && LittleFS.exists("/mqtt-server.crt")) {
      logger.debug("YASOLR", "Loading MQTT server certificate...");
      File serverCertFile = LittleFS.open("/mqtt-server.crt", "r");
      serverCert = serverCertFile.readString();
      serverCertFile.close();
      logger.debug("YASOLR", "Loaded MQTT server certificate:\n%s", serverCert.c_str());
    }

    mqtt.begin({
      .server = config.get(KEY_MQTT_SERVER),
      .port = static_cast<uint16_t>(config.get(KEY_MQTT_PORT).toInt()),
      .secured = secured,
      .serverCert = serverCert,
      .username = config.get(KEY_MQTT_USERNAME),
      .password = config.get(KEY_MQTT_PASSWORD),
      .clientId = Mycila::AppInfo.defaultMqttClientId,
      .willTopic = config.get(KEY_MQTT_TOPIC) + YASOLR_MQTT_WILL_TOPIC,
      .keepAlive = YASOLR_MQTT_KEEPALIVE,
    });
  }
});
