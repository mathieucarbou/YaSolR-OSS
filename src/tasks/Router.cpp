// SPDX-License-Identifier: GPL-3.0-or-later
/*
 * Copyright (C) 2023-2024 Mathieu Carbou
 */
#include <YaSolR.h>

Mycila::Task calibrationTask("Calibration", [](void* params) { router.calibrate(); });

Mycila::Task routerTask("Router", [](void* params) {
  std::optional<float> voltage = grid.getVoltage();
  const Mycila::ExpiringValue<float>& power = grid.power();

  if (!voltage.has_value() || power.isAbsent())
    router.noDivert();

  output1.applyTemperatureLimit();
  output2.applyTemperatureLimit();

  output1.applyAutoBypass();
  output2.applyAutoBypass();
});
