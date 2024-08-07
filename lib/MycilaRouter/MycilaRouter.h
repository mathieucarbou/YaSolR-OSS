// SPDX-License-Identifier: MIT
/*
 * Copyright (C) 2023-2024 Mathieu Carbou
 */
#pragma once

#include <MycilaGrid.h>
#include <MycilaJSY.h>
#include <MycilaPID.h>
#include <MycilaRouterOutput.h>

#include <algorithm>
#include <vector>

#ifdef MYCILA_JSON_SUPPORT
  #include <ArduinoJson.h>
#endif

namespace Mycila {
  class Router {
    public:
      typedef struct {
          float apparentPower = 0;
          float current = 0;
          float energy = 0;
          float power = 0;
          float powerFactor = 0;
          float resistance = 0;
          float thdi = 0;
          float voltage = 0;
      } Metrics;

      Router(PID& pidController, JSY& jsy) : _pidController(&pidController), _jsy(&jsy) {}

      void addOutput(RouterOutput& output) { _outputs.push_back(&output); }
      const std::vector<RouterOutput*>& getOutputs() const { return _outputs; }

      bool isRouting() const {
        for (const auto& output : _outputs) {
          if (output->getState() == RouterOutput::State::OUTPUT_ROUTING) {
            return true;
          }
        }
        return false;
      }

      void divert(float gridVoltage, float gridPower) {
        float powerToDivert = _pidController->compute(gridPower);
        for (const auto& output : _outputs) {
          if (output->isAutoDimmerEnabled()) {
            const float usedPower = output->autoDivert(gridVoltage, powerToDivert);
            powerToDivert = std::max(0.0f, powerToDivert - usedPower);
          }
        }
      }

      void noDivert() {
        for (const auto& output : _outputs) {
          if (output->isAutoDimmerEnabled()) {
            output->autoDivert(0, 0);
          }
        }
      }

      typedef std::function<void()> CalibrationCallback;
      void beginCalibration(CalibrationCallback cb = nullptr);
      void calibrate();
      bool isCalibrationRunning() const { return _calibrationRunning; }

#ifdef MYCILA_JSON_SUPPORT
      void toJson(const JsonObject& root, float voltage) const;
      static void toJson(const JsonObject& dest, const Metrics& metrics);
#endif

      // get router theoretical metrics based on the dimmer states and the grid voltage
      void getMetrics(Metrics& metrics, float voltage) const;
      void getMeasurements(Metrics& metrics) const;

    private:
      PID* _pidController;
      JSY* _jsy;
      std::vector<RouterOutput*> _outputs;

      // calibration
      // 0: idle
      // 1: prepare
      // 2: output 1 50%
      uint8_t _calibrationStep = 0;
      uint32_t _calibrationStartTime = 0;
      size_t _calibrationOutputIndex = 0;
      bool _calibrationRunning = false;
      CalibrationCallback _calibrationCallback = nullptr;
  };
} // namespace Mycila
