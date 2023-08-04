const SCRIPT_ID = JSON.stringify(Shelly.getCurrentScriptId());

const KV = {
  DEBUG: "SOLAR_DIVERTER_DEBUG",
  // PID Set point (usually 0 in the case of a router)
  PID_SETPOINT: "SOLAR_DIVERTER_PID_SETPOINT",
  // PID Kp term
  PID_KP: "SOLAR_DIVERTER_PID_KP",
  // PID Ki term
  PID_KI: "SOLAR_DIVERTER_PID_KI",
  // PID Kd term
  PID_KD: "SOLAR_DIVERTER_PID_KD",
  // Read interval in seconds
  READ_INTERVAL_S: "SOLAR_DIVERTER_READ_INTERVAL_S",
}

const DEFAULTS = {
  DEBUG: true,
  PID_SETPOINT: 0,
  PID_KP: 0.6,
  PID_KI: 0.001,
  PID_KD: 0.8,
  PID_OUTPUT_MIN: 0,
  READ_INTERVAL_S: 1,
}

let CONFIG = {
  DEBUG: DEFAULTS.DEBUG,
  PID_SETPOINT: DEFAULTS.PID_SETPOINT,
  PID_KP: DEFAULTS.PID_KP,
  PID_KI: DEFAULTS.PID_KI,
  PID_KD: DEFAULTS.PID_KD,
  PID_OUTPUT_MIN: DEFAULTS.PID_OUTPUT_MIN,
  READ_INTERVAL_S: DEFAULTS.READ_INTERVAL_S,
}

let PID = {
  lastError: 0,
  errorSum: 0,
}

let divertPower = 0;

function calculatePID(input) {
  const error = CONFIG.PID_SETPOINT - input;
  const dError = error - PID.lastError;
  PID.errorSum = Math.max(PID.errorSum + error, CONFIG.PID_OUTPUT_MIN);
  const pTerm = error * CONFIG.PID_KP;
  const iTerm = PID.errorSum * CONFIG.PID_KI;
  const dTerm = dError * CONFIG.PID_KD;
  PID.lastError = error;
  return pTerm + iTerm + dTerm;
}

function onPowerUpdate(result, err) {
  if (err)
    return;
  if (CONFIG.DEBUG)
    print("Data: ", JSON.stringify(result));
  const output = calculatePID(result.act_power);
  if (CONFIG.DEBUG)
    print("PID output: ", output);
  divertPower = Math.max(0, divertPower + output);
  if (CONFIG.DEBUG)
    print("Divert Power: ", divertPower, " W");
}

function loadConfig(cb) {
  cb();
  Shelly.call("KVS.Get", { key: KV.PID_KP }, function (result, err) {
    if (!err) CONFIG.PID_KP = parseFloat(result.value);
    print("CONFIG.PID_KP =  ", CONFIG.PID_KP);
    Shelly.call("KVS.Get", { key: KV.PID_KI }, function (result, err) {
      if (!err) CONFIG.PID_KI = parseFloat(result.value);
      print("CONFIG.PID_KI =  ", CONFIG.PID_KI);
      Shelly.call("KVS.Get", { key: KV.PID_KD }, function (result, err) {
        if (!err) CONFIG.PID_KD = parseFloat(result.value);
        print("CONFIG.PID_KD =  ", CONFIG.PID_KD);
        Shelly.call("KVS.Get", { key: KV.SETPOINT }, function (result, err) {
          if (!err) CONFIG.PID_SETPOINT = parseFloat(result.value);
          print("CONFIG.PID_SETPOINT =  ", CONFIG.PID_SETPOINT, " W");
          Shelly.call("KVS.Get", { key: KV.READ_INTERVAL_S }, function (result, err) {
            if (!err) CONFIG.READ_INTERVAL_S = parseInt(result.value);
            if (!CONFIG.READ_INTERVAL_S) CONFIG.READ_INTERVAL_S = DEFAULTS.READ_INTERVAL_S;
            print("CONFIG.READ_INTERVAL_S =  ", CONFIG.READ_INTERVAL_S, " s");
            cb();
          });
        });
      });
    });
  });
}

function saveConfig(cb) {
  Shelly.call("KVS.Set", { key: KV.PID_KP, value: ("" + CONFIG.PID_KP) }, function (result, err) {
    Shelly.call("KVS.Set", { key: KV.PID_KI, value: ("" + CONFIG.PID_KI) }, function (result, err) {
      Shelly.call("KVS.Set", { key: KV.PID_KD, value: ("" + CONFIG.PID_KD) }, function (result, err) {
        Shelly.call("KVS.Set", { key: KV.PID_SETPOINT, value: ("" + CONFIG.PID_SETPOINT) }, function (result, err) {
          Shelly.call("KVS.Set", { key: KV.READ_INTERVAL_S, value: ("" + CONFIG.READ_INTERVAL_S) }, function (result, err) {
            cb();
          });
        });
      });
    });
  });
}

loadConfig(function () {
  saveConfig(function () {
    // Shelly.addStatusHandler(function (evt) {
    //   if (evt.id == 0 && evt.name === 'em1' && evt.delta.act_power !== undefined) {
    //     onPowerUpdate(evt.delta);
    //   }
    // });
    Timer.set(CONFIG.READ_INTERVAL_S * 1000, true, function () {
      Shelly.call("EM1.GetStatus", { id: 0 }, onPowerUpdate);
    });
  });
});
