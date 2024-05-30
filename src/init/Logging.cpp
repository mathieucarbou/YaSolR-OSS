// SPDX-License-Identifier: GPL-3.0-or-later
/*
 * Copyright (C) 2023-2024 Mathieu Carbou and others
 */
#include <YaSolR.h>

#define TAG "YASOLR"

static const Mycila::TaskDoneCallback LOG_EXEC_TIME = [](const Mycila::Task& me, const uint32_t elapsed) {
  logger.debug(TAG, "%s in %" PRIu32 " us", me.getName(), elapsed);
};

Mycila::Task initLoggingTask("Init Logging", [](void* params) {
  logger.info(TAG, "Initializing logging...");

  const bool debug = config.getBool(KEY_ENABLE_DEBUG);

  if (debug) {
    logger.setLevel(debug ? ARDUHAL_LOG_LEVEL_DEBUG : ARDUHAL_LOG_LEVEL_INFO);
    esp_log_level_set("*", static_cast<esp_log_level_t>(logger.getLevel()));
  }

  profilerTask.setEnabled(debug);

  if (debug) {
    // Enable profiling for some FOREVER tasks
    displayTask.enableProfiling(8, Mycila::TaskTimeUnit::MILLISECONDS);
    ds18Task.enableProfiling(8, Mycila::TaskTimeUnit::MILLISECONDS);
    jsyTask.enableProfiling(8, Mycila::TaskTimeUnit::MILLISECONDS);
    mqttPublishTask.enableProfiling(8, Mycila::TaskTimeUnit::MILLISECONDS);
    pzemO1Task.enableProfiling(8, Mycila::TaskTimeUnit::MILLISECONDS);
    pzemO2Task.enableProfiling(8, Mycila::TaskTimeUnit::MILLISECONDS);
    profilerTask.enableProfiling(8, Mycila::TaskTimeUnit::MILLISECONDS);
    routerTask.enableProfiling(8, Mycila::TaskTimeUnit::MILLISECONDS);
    dashboardTask.enableProfiling(8, Mycila::TaskTimeUnit::MILLISECONDS);
  } else {
    displayTask.disableProfiling();
    ds18Task.disableProfiling();
    jsyTask.disableProfiling();
    mqttPublishTask.disableProfiling();
    pzemO1Task.disableProfiling();
    pzemO2Task.disableProfiling();
    profilerTask.disableProfiling();
    routerTask.disableProfiling();
    dashboardTask.disableProfiling();
  }

  // Log execution time for some "ONCE" tasks
  haDiscoveryTask.setCallback(debug ? LOG_EXEC_TIME : nullptr);
  otaTask.setCallback(debug ? LOG_EXEC_TIME : nullptr);
  mqttPublishStaticTask.setCallback(debug ? LOG_EXEC_TIME : nullptr);
  mqttPublishConfigTask.setCallback(debug ? LOG_EXEC_TIME : nullptr);
  pzemO1PairingTask.setCallback(debug ? LOG_EXEC_TIME : nullptr);
  pzemO2PairingTask.setCallback(debug ? LOG_EXEC_TIME : nullptr);

  if (!debug) {
    logger.setLevel(debug ? ARDUHAL_LOG_LEVEL_DEBUG : ARDUHAL_LOG_LEVEL_INFO);
    esp_log_level_set("*", static_cast<esp_log_level_t>(logger.getLevel()));
  }
});