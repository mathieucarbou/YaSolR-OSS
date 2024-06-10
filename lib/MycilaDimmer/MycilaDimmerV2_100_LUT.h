// SPDX-License-Identifier: MIT
/*
 * Copyright (C) 2023-2024 Mathieu Carbou and others
 */
#pragma once

#include "MycilaDimmer.h"
#include <Arduino.h>

#define MYCILA_DIMMER_LUT_SIZE 100

namespace Mycila {
  namespace DimmerInternal {
    static const uint16_t delay50HzLUT[] PROGMEM = {10000, 9900, 9800, 9700, 9600, 9500, 9400, 9300, 9200, 9100, 9000, 8900, 8800, 8700, 8600, 8500, 8400, 8300, 8200, 8100, 8000, 7900, 7800, 7700, 7600, 7500, 7400, 7300, 7200, 7100, 7000, 6900, 6800, 6700, 6600, 6500, 6400, 6300, 6200, 6100, 6000, 5900, 5800, 5700, 5600, 5500, 5400, 5300, 5200, 5100, 5000, 4900, 4800, 4700, 4600, 4500, 4400, 4300, 4200, 4100, 4000, 3900, 3800, 3700, 3600, 3500, 3400, 3300, 3200, 3100, 3000, 2900, 2800, 2700, 2600, 2500, 2400, 2300, 2200, 2100, 2000, 1900, 1800, 1700, 1600, 1500, 1400, 1300, 1200, 1100, 1000, 900, 800, 700, 600, 500, 400, 300, 200, 100, 0};
    static const uint16_t delay60HzLUT[] PROGMEM = {8333, 8250, 8167, 8084, 8000, 7917, 7834, 7750, 7667, 7584, 7500, 7417, 7334, 7250, 7167, 7084, 7000, 6917, 6834, 6750, 6667, 6584, 6500, 6417, 6334, 6250, 6167, 6084, 6000, 5917, 5834, 5750, 5667, 5584, 5500, 5417, 5334, 5250, 5167, 5084, 5000, 4917, 4834, 4750, 4667, 4584, 4500, 4417, 4334, 4250, 4167, 4084, 4000, 3917, 3834, 3750, 3667, 3584, 3500, 3417, 3334, 3250, 3167, 3084, 3000, 2917, 2834, 2750, 2667, 2584, 2500, 2417, 2334, 2250, 2167, 2084, 2000, 1917, 1834, 1750, 1667, 1584, 1500, 1417, 1334, 1250, 1167, 1084, 1000, 917, 834, 750, 667, 584, 500, 417, 334, 250, 167, 84, 0};
    static const float vrms50HzLUT[] PROGMEM = {0.000000, 0.002565, 0.007252, 0.013317, 0.020488, 0.028608, 0.037565, 0.047277, 0.057676, 0.068706, 0.080319, 0.092471, 0.105124, 0.118241, 0.131792, 0.145743, 0.160066, 0.174732, 0.189716, 0.204991, 0.220533, 0.236316, 0.252317, 0.268515, 0.284884, 0.301405, 0.318055, 0.334813, 0.351659, 0.368572, 0.385532, 0.402519, 0.419514, 0.436499, 0.453455, 0.470363, 0.487205, 0.503965, 0.520626, 0.537169, 0.553580, 0.569843, 0.585941, 0.601860, 0.617585, 0.633102, 0.648398, 0.663459, 0.678272, 0.692825, 0.707107, 0.721106, 0.734811, 0.748213, 0.761302, 0.774068, 0.786504, 0.798602, 0.810354, 0.821754, 0.832796, 0.843474, 0.853785, 0.863724, 0.873287, 0.882473, 0.891279, 0.899705, 0.907749, 0.915412, 0.922695, 0.929599, 0.936128, 0.942284, 0.948072, 0.953496, 0.958562, 0.963276, 0.967645, 0.971676, 0.975380, 0.978764, 0.981839, 0.984616, 0.987106, 0.989323, 0.991277, 0.992985, 0.994459, 0.995715, 0.996769, 0.997637, 0.998335, 0.998882, 0.999294, 0.999591, 0.999790, 0.999911, 0.999974, 0.999997, 1.000000};
    static const float vrms60HzLUT[] PROGMEM = {0.000000, 0.002565, 0.007231, 0.013264, 0.020488, 0.028574, 0.037491, 0.047277, 0.057633, 0.068615, 0.080319, 0.092421, 0.105020, 0.118241, 0.131737, 0.145630, 0.160066, 0.174673, 0.189595, 0.204991, 0.220470, 0.236189, 0.252317, 0.268449, 0.284753, 0.301405, 0.317988, 0.334679, 0.351659, 0.368504, 0.385396, 0.402519, 0.419446, 0.436363, 0.453455, 0.470295, 0.487071, 0.503965, 0.520559, 0.537037, 0.553580, 0.569778, 0.585813, 0.601860, 0.617522, 0.632979, 0.648398, 0.663399, 0.678154, 0.692825, 0.707050, 0.720995, 0.734811, 0.748160, 0.761198, 0.774068, 0.786455, 0.798506, 0.810354, 0.821709, 0.832709, 0.843474, 0.853745, 0.863646, 0.873287, 0.882437, 0.891210, 0.899705, 0.907717, 0.915352, 0.922695, 0.929572, 0.936077, 0.942284, 0.948050, 0.953454, 0.958562, 0.963257, 0.967611, 0.971676, 0.975365, 0.978738, 0.981839, 0.984605, 0.987088, 0.989323, 0.991270, 0.992972, 0.994459, 0.995711, 0.996762, 0.997637, 0.998333, 0.998878, 0.999294, 0.999590, 0.999789, 0.999911, 0.999974, 0.999997, 1.000000};
  } // namespace DimmerInternal
} // namespace Mycila

uint16_t Mycila::Dimmer::_lookupFiringDelay(uint8_t level, float frequency) {
  if (level == 0)
    return 500000 / frequency; // semi-period in microseconds

  if (level >= MYCILA_DIMMER_LUT_SIZE)
    return 0;

  return frequency == 60 ? (uint16_t)pgm_read_word(&Mycila::DimmerInternal::delay60HzLUT[level]) : (uint16_t)pgm_read_word(&Mycila::DimmerInternal::delay50HzLUT[level]);
}

float Mycila::Dimmer::_lookupVrmsFactor(uint8_t level, float frequency) {
  if (level == 0)
    return 0;

  if (level >= MYCILA_DIMMER_LUT_SIZE)
    return 1;

  return frequency == 60 ? static_cast<float>(pgm_read_float(&Mycila::DimmerInternal::vrms60HzLUT[level])) : static_cast<float>(pgm_read_float(&Mycila::DimmerInternal::vrms50HzLUT[level]));
}
