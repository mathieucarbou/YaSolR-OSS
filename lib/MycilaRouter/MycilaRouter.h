// SPDX-License-Identifier: MIT
/*
 * Copyright (C) 2023-2024 Mathieu Carbou and others
 */
#pragma once

#include <MycilaRouterOutput.h>

#include <MycilaJSY.h>
#include <vector>

#ifdef MYCILA_JSON_SUPPORT
  #include <ArduinoJson.h>
#endif

#ifndef MYCILA_ROUTER_OUTPUT_COUNT
  #define MYCILA_ROUTER_OUTPUT_COUNT 2
#endif

namespace Mycila {

  typedef struct {
      float apparentPower = 0;
      float current = 0;
      float energy = 0;
      float nominalPower = 0;
      float power = 0;
      float powerFactor = 0;
      float resistance = 0;
      float thdi = 0;
      RouterOutputMetrics outputs[MYCILA_ROUTER_OUTPUT_COUNT] = {};
  } RouterMetrics;

  class Router {
    public:
      explicit Router(JSY& jsy) : _jsy(&jsy) {}

      void addOutput(RouterOutput& output) { _outputs.push_back(&output); }
      const std::vector<RouterOutput*>& getOutputs() const { return _outputs; }

      bool isRouting() const {
        for (const auto& output : _outputs) {
          if (output->getState() == RouterOutputState::OUTPUT_ROUTING) {
            return true;
          }
        }
        return false;
      }

      void getMetrics(RouterMetrics& metrics) const {
        size_t index = 0;
        bool routing = false;

        for (const auto& output : _outputs) {
          assert(index < MYCILA_ROUTER_OUTPUT_COUNT);
          output->getMetrics(metrics.outputs[index]);
          metrics.energy += metrics.outputs[index].energy;
          if (output->getState() == RouterOutputState::OUTPUT_ROUTING) {
            routing = true;
            metrics.apparentPower += metrics.outputs[index].apparentPower;
            metrics.current += metrics.outputs[index].current;
            metrics.nominalPower += metrics.outputs[index].nominalPower;
            metrics.power += metrics.outputs[index].power;
          }
          index++;
        }

        if (routing && _jsy->isConnected()) {
          if (!metrics.apparentPower)
            metrics.apparentPower = _jsy->getApparentPower1();
          if (!metrics.current)
            metrics.current = _jsy->getCurrent1();
          if (!metrics.power)
            metrics.power = _jsy->getPower1();
        }

        if (!metrics.energy)
          metrics.energy = _jsy->getEnergy1() + _jsy->getEnergyReturned1();

        metrics.powerFactor = metrics.apparentPower == 0 ? 0 : metrics.power / metrics.apparentPower;
        metrics.resistance = metrics.current == 0 ? 0 : metrics.power / (metrics.current * metrics.current);
        metrics.thdi = metrics.powerFactor == 0 ? 0 : sqrt(1 / pow(metrics.powerFactor, 2) - 1);
      }

#ifdef MYCILA_JSON_SUPPORT
      void toJson(const JsonObject& root) const {
        RouterMetrics metrics;
        getMetrics(metrics);
        root["metrics"]["apparent_power"] = metrics.apparentPower;
        root["metrics"]["current"] = metrics.current;
        root["metrics"]["energy"] = metrics.energy;
        root["metrics"]["power"] = metrics.power;
        root["metrics"]["power_factor"] = metrics.powerFactor;
        root["metrics"]["thdi"] = metrics.thdi;
        for (const auto& output : _outputs)
          output->toJson(root[output->getName()].to<JsonObject>());
      }
#endif

    private:
      JSY* _jsy;
      std::vector<RouterOutput*> _outputs;
  };
} // namespace Mycila
