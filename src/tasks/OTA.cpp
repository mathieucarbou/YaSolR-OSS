// SPDX-License-Identifier: GPL-3.0-or-later
/*
 * Copyright (C) 2023-2024 Mathieu Carbou
 */
#include <YaSolR.h>

Mycila::Task otaTask("OTA", Mycila::TaskType::ONCE, [](void* params) {
  logger.info(TAG, "Preparing OTA update");
  // stop electricity
  dimmerO1.endDimmer();
  dimmerO2.endDimmer();
  bypassRelayO1.end();
  bypassRelayO2.end();
  relay1.end();
  relay2.end();
  Mycila::Dimmer::endZCD();
  // stop blocking I/O tasks
  mqtt.end();
  udp.close();
#ifdef APP_MODEL_TRIAL
  Mycila::Trial.end();
#endif
});
