// SPDX-License-Identifier: MIT
/*
 * Copyright (C) 2023-2024 Mathieu Carbou
 * 
 * ======================================
 * CHANGELOG
 * 
 * - v1: Initial version
 * 
 * ======================================
 */
const scriptName = "solar_diverter.js";

// Config

const CONFIG = {
  // Debug mode
  DEBUG: 1,
  // Grid Power Read Interval (s)
  READ_INTERVAL_S: 1,
  PID: {
    // Target Grid Power (W)
    SET_POINT: 0,
    // PID Proportional Gain
    KP: 0.8,
    // PID Integral Gain
    KI: 0,
    // PID Derivative Gain
    KD: 0.8,
    // PID Output Minimum Clamp (W)
    OUTPUT_MIN: 0,
  },
  DIMMERS: {
    "192.168.125.93": {
      // Resistance (in Ohm) of the load connecter to the dimmer + voltage regulator
      // 0 will disable the dimmer
      RESISTANCE: 24
    },
    "192.168.125.97": {
      RESISTANCE: 0
    }
  }
};

// PID Controller 

let PID = {
  // PID Input
  input: 0,
  // PID Output
  output: 0,
  // current error value
  error: 0,
  // difference between current and last error
  dError: 0,
  // Proportional Term
  pTerm: 0,
  // Integral Term
  iTerm: 0,
  // Derivative Term
  dTerm: 0,
}

// Divert Control

let DIVERT = {
  lastTime: 0,
  voltage: 0,
  gridPower: CONFIG.PID.SET_POINT,
  divertPower: CONFIG.PID.SET_POINT,
  dimmers: {}
}

function validateConfig(cb) {
  print(scriptName, ":", "Validating Config...")

  if (CONFIG.DIMMERS.length === 0) {
    print(scriptName, ":", "ERR: No dimmer configured");
    return;
  }

  for (const ip in CONFIG.DIMMERS) {
    if (CONFIG.DIMMERS[ip].RESISTANCE < 0) {
      print(scriptName, ":", "ERR: Dimmer resistance should be greater than 0");
      return;
    }

    if (CONFIG.DIMMERS[ip].RESISTANCE === 0) {
      print(scriptName, ":", "Dimmer", ip, "is disabled");
      continue;
    }

    print(scriptName, ":", "Dimmer", ip, "is enabled");
    DIVERT.dimmers[ip] = {
      divertPower: 0
    }
  }

  cb();
}

function calculatePID(input) {
  PID.input = input;
  const error = CONFIG.PID.SET_POINT - PID.input;
  PID.dError = input === CONFIG.PID.SET_POINT ? 0 : error - PID.error;
  PID.pTerm = error * CONFIG.PID.KP;
  PID.iTerm = input === CONFIG.PID.SET_POINT ? 0 : Math.max(PID.iTerm + error * CONFIG.PID.KI, CONFIG.PID.OUTPUT_MIN);
  PID.dTerm = PID.dError * CONFIG.PID.KD;
  PID.error = error;
  PID.output = PID.pTerm + PID.iTerm + PID.dTerm;
  return PID.output;
}

function callDimmers(cb) {
  for (const ip in DIVERT.dimmers) {
    const dimmer = DIVERT.dimmers[ip];

    // ignore contacted dimmers
    if (dimmer.rpc !== "pending") {
      continue;
    }

    // build url
    const url = "http://" + ip + "/rpc/Light.Set?id=0&on=" + (dimmer.dutyCycle > 0 ? "true" : "false") + "&brightness=" + (dimmer.dutyCycle * 100) + "&transition_duration=0.5";

    if (CONFIG.DEBUG > 1)
      print(scriptName, ":", "Calling Dimmer: ", url);

    // call dimmer
    Shelly.call("HTTP.GET", { url: url, timeout: 5 }, function (result, err) {
      if (err) {
        print(scriptName, ":", "ERR:", err);
        dimmer.rpc = "failed";
      } else if (result.code !== 200) {
        const rpcResult = JSON.parse(result.body);
        print(scriptName, ":", "ERR", rpcResult.code, ":", rpcResult.message);
        dimmer.rpc = "failed";
      } else {
        dimmer.rpc = "success";
      }

      // once done, call ourself again until no dimmer is left
      callDimmers(cb);
    });

    // exit the loop immediately to avoid multiple calls in case the yare made in parallel
    return;
  }

  // if we are here, all dimmers have been contacted
  cb();
}

function divert(voltage, gridPower) {
  DIVERT.voltage = voltage;
  DIVERT.gridPower = gridPower;
  const correction = calculatePID(DIVERT.gridPower);
  DIVERT.divertPower = Math.max(0, DIVERT.divertPower + correction);

  if (CONFIG.DEBUG > 0)
    print(scriptName, ":", "Grid:", voltage, "V,", gridPower, "W. Correction:", correction, "W. Total Divert Power:", DIVERT.divertPower, "W");

  let remaining = DIVERT.divertPower;

  for (const ip in DIVERT.dimmers) {
    const dimmer = DIVERT.dimmers[ip];
    dimmer.nominalPower = voltage * voltage / CONFIG.DIMMERS[ip].RESISTANCE;
    dimmer.divertPower = Math.min(remaining, dimmer.nominalPower);
    dimmer.dutyCycle = dimmer.divertPower / dimmer.nominalPower;
    dimmer.powerFactor = Math.sqrt(dimmer.dutyCycle);
    dimmer.dimmedVoltage = dimmer.powerFactor * voltage;
    dimmer.current = dimmer.dimmedVoltage / CONFIG.DIMMERS[ip].RESISTANCE;
    dimmer.apparentPower = dimmer.current * voltage;
    dimmer.thdi = dimmer.dutyCycle === 0 ? 0 : Math.sqrt(1 / dimmer.dutyCycle - 1);
    dimmer.rpc = "pending";

    remaining -= dimmer.divertPower;

    if (CONFIG.DEBUG > 0)
      print(scriptName, ":", "Dimmer", ip, "=>", dimmer.divertPower, "W");
  }

  callDimmers(throttleReadPower);
}

function onEM1GetStatus(result, err) {
  if (err)
    return;
  if (CONFIG.DEBUG > 1)
    print(scriptName, ":", "EM1.GetStatus:", JSON.stringify(result));
  divert(result.voltage, result.act_power);
}

function readPower() {
  DIVERT.lastTime = Date.now();
  Shelly.call("EM1.GetStatus", { id: 0 }, onEM1GetStatus);
}

function throttleReadPower() {
  const now = Date.now();
  const diff = now - DIVERT.lastTime;
  if (diff > 1000) {
    readPower();
  } else {
    Timer.set(1000 - diff, false, readPower);
  }
}

// HTTP handlers

function onGetStatus(request, response) {
  response.code = 200;
  response.headers = {
    "Content-Type": "application/json"
  }
  response.body = JSON.stringify({
    pid: PID,
    divert: DIVERT
  });
  response.send();
}

// Main

validateConfig(function () {
  print(scriptName, ":", "Starting Shelly Solar Diverter...")
  HTTPServer.registerEndpoint("status", onGetStatus);
  readPower();
});
