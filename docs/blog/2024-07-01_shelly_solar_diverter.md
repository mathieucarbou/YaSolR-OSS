---
layout: default
title: Shelly Solar Diverter
description: Shelly Solar Router / Diverter redirects the excess solar production to a water tank or heater
---

_Date: 2024-07-01_

# Shelly Solar Diverter

- [What is a Solar Router / Diverter ?](#what-is-a-solar-router--diverter-)
- [Shelly Solar Router](#shelly-solar-router)
- [Download](#download)
- [Hardware](#hardware)
- [Wiring](#wiring)
  - [Shelly Add-On + DS18B20](#shelly-add-on--ds18b20)
  - [Electric Circuit](#electric-circuit)
  - [RC Snubber](#rc-snubber)
  - [Add a second dimmer](#add-a-second-dimmer)
- [Setup](#setup)
  - [Shelly Dimmer Setup](#shelly-dimmer-setup)
  - [Shelly Pro EM 50 Setup](#shelly-pro-em-50-setup)
- [How to use](#how-to-use)
  - [Configuration](#configuration)
  - [Start / Stop Automatic Divert](#start--stop-automatic-divert)
  - [Solar Diverter Status](#solar-diverter-status)
- [Demo](#demo)

## What is a Solar Router / Diverter ?

A _Solar Router_ allows to redirect the solar production excess to some appliances instead of returning it to the grid.
The particularity of a solar router is that it will dim the voltage and power sent to the appliance in order to match the excess production, in contrary to a simple relay that would just switch on/off the appliance without controlling its power.

A _Solar Router_ is usually connected to the resistance of a water tank and will heat the water when there is production excess.

A solar router can also do more things, like controlling (on/off) the activation of other appliances (with the grid normal voltage and not the dimmed voltage) in case the excess reaches a threshold. For example, one could activate a pump, pool heater, etc if the excess goes above a specific amount, so that this appliance gets the priority over heating the water tank.

A router can also schedule some forced heating of the water tank to ensure the water reaches a safe temperature, and consequently bypass the dimmed voltage. This is called a bypass relay.

## Shelly Solar Router

This solar diverter based on Shelly and a Shelly script can control remotely dimmers and could even be enhanced with relays.
Shelly's being remotely controllable, such system offers a very good integration with Shelly App and Home Automation Systems like Home Assistant.

It is possible to put some rules based on temperature, time, days, etc and control everything from the Shelly App or Home Assistant.

The Shelly script, is activated, automatically adjusts the dimmers to the grid import or export (solar production excess).

## Download

- **[Shelly Solar Diverter Script](/downloads/solar_diverter_v1.js)**

## Hardware

All the components can be bought at [https://www.shelly.com/](https://www.shelly.com/), except the voltage regulator, where you can find some links [on this website](https://yasolr.carbou.me/build#compatible-hardware)

| [Shelly Pro EM - 50](https://www.shelly.com/fr/products/shop/proem-1x50a) | [Shelly Dimmer 0/1-10V PM Gen3](https://www.shelly.com/fr/products/shop/1xsd10pmgen3) | [Shelly Plus Add-On](https://www.shelly.com/fr/products/shop/shelly-plus-add-on) | [Temperature Sensor DS18B20](https://www.shelly.com/fr/products/shop/temperature-sensor-ds18B20) | Voltage Regulator<br>- [Loncont LSA-H3P50YB](https://fr.aliexpress.com/item/32606780994.html)<br>- [LCTC DTY-220V40P1](https://fr.aliexpress.com/item/1005005008018888.html) |
| :-----------------------------------------------------------------------: | :-----------------------------------------------------------------------------------: | :------------------------------------------------------------------------------: | :----------------------------------------------------------------------------------------------: | :--------------------------------------------------------------------------------------------------------------------------------------------------------------------------: |
|              ![](/assets/img/hardware/Shelly_Pro_EM_50.jpeg)              |                   ![](/assets/img/hardware/Shelly_Dimmer-10V.jpeg)                    |                   ![](/assets/img/hardware/Shelly_Addon.jpeg)                    |                            ![](/assets/img/hardware/Shelly_DS18.jpeg)                            |                               ![](/assets/img/hardware/LSA-H3P50YB.jpeg)<br>![](/assets/img/hardware/LCTC_Voltage_Regulator_DTY-220V40P1.jpeg)                               |

Some additional hardware are required depending on the installation.
Please select the amperage according to your needs.

- A 2A breaker for the Shelly electric circuit
- A 16A or 20A breaker for your water tank (resistance) electric circuit
- A 25A relay or contactor for the bypass relay (to force a heating) for the water tank electric circuit
- A protection box for the Shelly
- The LSA voltage regulator is far better than the LCTC one: the variation of voltage from 0 to 10V better matches the power curve of the output

## Wiring

### Shelly Add-On + DS18B20

First the easy part: the temperature sensor and the Shelly Add-On, which has to be put behind the Shelly Dimmer.

![](/assets/img/hardware/Shelly_Addon_DS18.jpeg)

### Electric Circuit

- Choose your breakers and wires according to your load
- Circuits can be split.
  For example, the Shelly EM can be inside the main electric box, and the Shelly Dimmer + Add-On can be in the water tank electric panel, while the contactor and dimmer can be placed neat the water tank.
  They communicate through the network.
- The dimmer will control the voltage regulator through the `COM` and `0-10V` ports
- The dimmer will also control the relay or contactor through the `A2` ports
- The wire from `Dimmer Output` to `Dimmer S1` is to set the switch mode to invert and make the dimmer detect when the contactor is OFF or ON and respectively disable or enable the dimming.
- The B clamp around the wire going from the voltage regulator to the water tank is to measure the current going through the water tank resistance is optional and for information purposes only.
- The A clamp should be put around the main phase entering the house
- The relay / contactor is optional and is used to schedule some forced heating of the water tank to ensure the water reaches a safe temperature, and consequently bypass the dimmed voltage.
- The neutral wire going to the voltage regulator can be a small one; it is only used for the voltage and Zero-Crossing detection.

[![](/assets/img/schemas/Solar_Router_Diverter.jpg)](/assets/img/schemas/Solar_Router_Diverter.jpg)

### RC Snubber

If switching the contactor / relay causes the Shelly device to reboot, place a [RC Snubber](https://www.shelly.com/fr/products/shop/shelly-rc-snubber) between the A1 and A2 ports of the contactor / relay.

### Add a second dimmer

If you want to control a second resistive load, it is possible to duplicate the circuit to add another dimmer and voltage regulator.

Modify the config accordingly to support many dimmers and they will be turned on/off sequentially to match the excess production.

## Setup

First make sure that your Shelly's are setup properly.

The script has to be installed inside the Shelly Pro EM 50, because this is where the measurements of the imported and exported grid power is done.
Also, this central place allows to control the 1, 2 or more dimmers remotely.

### Shelly Dimmer Setup

- Make sure the switch (input) are enabled and inverted. S1 should replicate the inverse state of the relay / contactor, so that when you activate the contactor through the Dimmer output remotely, the dimmer will deactivate itself.
- Set static IP addresses

### Shelly Pro EM 50 Setup

- Set static IP address
- Make sure to place the A clamp around the main phase entering the house in the right direction
- Add the `Shelly Solar Diverter` to the Shelly Pro EM
- Configure the settings in the `CONFIG` object
- Start teh script

## How to use

### Configuration

Edit the `CONFIG` object and pay attention to the values, especially the resistance value which should be accurate, otherwise the routing precision will be bad.

```javascript
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
      RESISTANCE: 24,
    },
    "192.168.125.97": {
      RESISTANCE: 0,
    },
  },
};
```

### Start / Stop Automatic Divert

You can start / stop the script manually from the interface or remotely by calling:

```
http://192.168.125.92/rpc/Script.Start?id=1
http://192.168.125.92/rpc/Script.Stop?id=1
```

- `192.168.125.92` begin the Shelly EM 50 static IP address.
- `1` being the script ID as seen in the Shelly interface

![](/assets/img/screenshots/shelly_script_id.jpeg)

Once the script is uploaded and started, it will automatically manage the power sent to the resistive load according to the rules above.

### Solar Diverter Status

You can view the status of the script by going to the script `status` endpoint, which is only available when the script is running.

```
http://192.168.125.92/script/1/status
```

```json
{
  "pid": {
    "input": 0,
    "output": 0,
    "error": 0,
    "dError": 0,
    "pTerm": 0,
    "iTerm": 0,
    "dTerm": 0
  },
  "divert": {
    "voltage": 237.9,
    "gridPower": 0,
    "divertPower": 173.76,
    "dimmers": {
      "192.168.125.93": {
        "divertPower": 86.88,
        "nominalPower": 2358.18375,
        "dutyCycle": 0.03684191276,
        "powerFactor": 0.19194247253,
        "dimmedVoltage": 45.66311421705,
        "current": 1.90262975904,
        "apparentPower": 452.63561967657,
        "thdi": 5.11302248812,
        "rpc": "pending"
      },
      "192.168.125.97": {
        "divertPower": 86.88,
        "nominalPower": 2358.18375,
        "dutyCycle": 0.03684191276,
        "powerFactor": 0.19194247253,
        "dimmedVoltage": 45.66311421705,
        "current": 1.90262975904,
        "apparentPower": 452.63561967657,
        "thdi": 5.11302248812,
        "rpc": "pending"
      }
    }
  }
}
```

## Demo

Here is the view of the Shelly device websites while the grid power is changing.

[![Shelly Solar Diverter Demo](http://img.youtube.com/vi/he5qPJx8_R4/0.jpg)](http://www.youtube.com/watch?v=he5qPJx8_R4 "Shelly Solar Diverter Demo")

Here is a PoC box I am using for my testing with all the components wired. I am still waiting for the dimmer gen 3 which works in **current sourcing** mode, but everything else is working.

![](/assets/img/hardware/shelly_solar_diverter_poc.jpeg)
