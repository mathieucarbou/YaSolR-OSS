// SPDX-License-Identifier: GPL-3.0-or-later
/*
 * Copyright (C) 2023-2024 Mathieu Carbou and others
 */
#include <YaSolR.h>
#include <YaSolRWebsite.h>

Mycila::Task routerTask("Router", [](void* params) {
  grid.applyExpiration();

  output1.applyDimmerLimits();
  output2.applyDimmerLimits();

  output1.applyAutoBypass();
  output2.applyAutoBypass();
});

Mycila::Task relayTask("Relay", [](void* params) {
  Mycila::GridMetrics gridMetrics;
  grid.getMeasurements(gridMetrics);

  Mycila::RouterMetrics routerMetrics;
  router.getMeasurements(routerMetrics);

  float virtualGridPower = gridMetrics.power - routerMetrics.power;

  if (routerRelay1.tryRelayStateAuto(true, virtualGridPower))
    return;
  if (routerRelay2.tryRelayStateAuto(true, virtualGridPower))
    return;
  if (routerRelay2.tryRelayStateAuto(false, virtualGridPower))
    return;
  if (routerRelay1.tryRelayStateAuto(false, virtualGridPower))
    return;
});

Mycila::Task routingTask("Routing", Mycila::TaskType::ONCE, [](void* params) {
  Mycila::GridMetrics gridMetrics;
  grid.getMeasurements(gridMetrics);
  router.divert(gridMetrics.voltage, gridMetrics.power);
  if (config.getBool(KEY_ENABLE_PID_VIEW)) {
    // Dashboard
    YaSolR::Website.updatePID();
    dashboard.sendUpdates();

    // WebSocket
    AsyncWebSocketMessageBuffer* buffer = wsDebugPID.makeBuffer(256);
    snprintf((char*)buffer->get(), 256, "%d,%d,%d,%d,%d,%.3f,%.3f,%.3f,%d,%d,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f", pidController.getProportionalMode(), pidController.getDerivativeMode(), pidController.getIntegralCorrectionMode(), pidController.isReversed(), static_cast<int>(pidController.getSetPoint()), pidController.getKp(), pidController.getKi(), pidController.getKd(), static_cast<int>(pidController.getOutputMin()), static_cast<int>(pidController.getOutputMax()), pidController.getInput(), pidController.getOutput(), pidController.getError(), pidController.getSum(), pidController.getPTerm(), pidController.getITerm(), pidController.getDTerm()); // NOLINT
    wsDebugPID.textAll(buffer);
  }
});
