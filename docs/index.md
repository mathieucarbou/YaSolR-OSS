---
layout: default
title: Home
description: Home
---

# What is YaS☀️lR ?

YaS☀️lR is an Arduino / ESP32 firmware for Solar Routers, compatible with most of existing and easy to source hardware components.

If you already have a Solar Router at home based on ESP32, built by yourself or someone else, there is a good chance that YaS☀️lR will be compatible.

| [![](./assets/img/screenshots/overview.jpeg)](./assets/img/screenshots/overview.jpeg) | [![](./assets/img/screenshots/output1.jpeg)](./assets/img/screenshots/output1.jpeg) |

**What YaS☀️lR is not?**

YaS☀️lR **is not** a product and is not packaged with any hardware.
YaS☀️lR is **only** the software layer that will run on the Solar Router of your choice that you will have to build (or already have).

## Why YaS☀️lR ?

_YaSolR_ is built with this vision in mind:

- **OpenSource**: anybody can help and participate in the project

- **Good development practices**, PlatformIO project with versioning, CI, issue management, etc.
  This is big project and not a simple Arduino project (.ino file)

- **Hardware compatibility**: supports several phase control systems like Random SSR, Robodyn, many ESP32 boards with WiFi and Ethernet

- **Flexible**: it is made in a way that you can only pick the components you need depending on the features you want to activate

- **Easy to use**: The firmware should be easy to install and use

- **Harmonic Regulations**: The firmware gives the user the ability to monitor and limit the router in order to stay below the levels of harmonic regulations

- **110/230V 50/60Hz**: the dimmer implementation and proposed hardware all support both frequencies and voltages

- **3-Phase** support

- **Statistics, API, Home Automation**: Web API, MQTT API, Home Assistant auto-discovery, etc.
  The router exposes a lot of statistics and information through MQTT and REST API and provides a very good integration with Home Assistant or other home automation systems.

- **Optimized, Fast and Powerful**: the router logic is optimized to use the ESP32 hardware to its full potential and also react as fast as possible to the measurements, which are also taken in a dedicated loop in a few milliseconds.
  - Own optimized libraries to read hardware components (JSY, PPZEM)
  - JSY reading speed is increased to its maximum to give a fast reading
  - RMT peripheral is used for DS18 readings
  - PID Controller for dimmers with customizable Kp, KI, kd factors
  - Optimized dimmer library with ZCD pulse width (🚧)
  - Motor Control Pulse Width Modulator (MCPWM) for phase control (🚧)

## YaS☀️lR Features

(_🚧_ means _In Progress_)

- [2x Routing Outputs](#2x-routing-outputs)
  - [Dimmer (required)](#dimmer-required)
  - [Bypass Relay (optional)](#bypass-relay-optional)
  - [Temperature Sensor (optional)](#temperature-sensor-optional)
  - [Measurement device (optional)](#measurement-device-optional)
  - [Additional Features](#additional-features)
- [Grid Power Measurement](#grid-power-measurement)
  - [JSY-MK-194T](#jsy-mk-194t)
  - [Remote Grid Measurement](#remote-grid-measurement)
  - [3-Phase Support](#3-phase-support)
- [2x Relays](#2x-relays)
- [Monitoring and Management](#monitoring-and-management)
- [MQTT, REST API and Home Automation Systems](#mqtt-rest-api-and-home-automation-systems)
- [Networking / Offline](#networking--offline)
- [Remote Capabilities](#remote-capabilities)
- [Virtual Excess and EV Charger Compatibility](#virtual-excess-and-ev-charger-compatibility)
- [OSS vs PRO](#oss-vs-pro)

### 2x Routing Outputs

A routing output is connected to a resistive load and controls its power by dimming the voltage. Each output is composed of the following components.

#### Dimmer (required)

Controls the power sent to the load. Example of supported dimmers:

| Dimmer Type                                                                               | `Phase Control` | `Burst Fire Control` (🚧) |
| :---------------------------------------------------------------------------------------- | :-------------: | :-----------------------: |
| **Robodyn 24A** ![Robodyn 24A](./assets/img/hardware/Robodyn_24A.jpeg)                    |       ✅        |            ✅             |
| **Robodyn 40A** ![Robodyn 40A](./assets/img/hardware/Robodyn_40A.jpeg)                    |       ✅        |            ✅             |
| **Random SSR** (🚧) ![Random SSR](./assets/img/hardware/Random_SSR.jpeg)                  |       ✅        |            ✅             |
| **Zero-Cross SSR** ![Zero-Cross SSR](./assets/img/hardware/SSR_40A_DA.jpeg)               |       ❌        |            ✅             |
| **Voltage Regulator** (🚧) ![Loncont LSA-H3P50YB](./assets/img/hardware/LSA-H3P50YB.jpeg) |       ✅        |            ✅             |

#### Bypass Relay (optional)

Forces a heating at full power and bypass the dimmer at a given schedule or manually.
Keeping a dimmer `on` generates heat so a bypass relay can be installed to avoid using the dimmer.
If not installed, the dimmer will be used instead and will be set to 0-100% to simulate the relay.

|                      Electromagnetic Relay                       |                      Zero-Cross SSR                      |                      Random SSR                      |
| :--------------------------------------------------------------: | :------------------------------------------------------: | :--------------------------------------------------: |
| ![Electromagnetic Relay](./assets/img/hardware/DIN_2_Relay.jpeg) | ![Zero-Cross SSR](./assets/img/hardware/SSR_40A_DA.jpeg) | ![Random SSR](./assets/img/hardware/Random_SSR.jpeg) |

#### Temperature Sensor (optional)

To monitor the temperature of the water tanker and trigger automatic heating based on temperature thresholds. Supported sensor:

|                    DS18B20                     |
| :--------------------------------------------: |
| ![DS18B20](./assets/img/hardware/DS18B20.jpeg) |

#### Measurement device (optional)

|                     PZEM-004T V3                      |                                          JSY-MK-194T                                          |
| :---------------------------------------------------: | :-------------------------------------------------------------------------------------------: |
|    can precisely monitor each output independently    | can monitor the global routed power (sum of the two outputs) and grid power with its 2 clamps |
| ![PZEM-004T V3](./assets/img/hardware/PZEM-004T.jpeg) |                   ![JSY-MK-194T](./assets/img/hardware/JSY-MK-194T_2.jpeg)                    |

#### Additional Features

Each output supports the following features:

- `Automatic Bypass` / `Manual Bypass Control`: Automatically force a heating as needed based on days, hours, temperature range, or control it manually
- `Automatic Dimmer` (🚧) / `Manual Dimmer Control`: Automatically send the grid excess to the resistive load through the dimmer (or manually control the dimmer yourself if disabled), or control it manually
- `Dimmer Duty Limiter`: Set a limit to the dimmer power to avoid routing too much power
- `Dimmer Temperature Limiter`: Set a limit to the dimmer to stop it when a temperature is reached. This temperature can be different than the temperature used in auto bypass mode.
- `Statistics`: Harmonic information, power factor, energy, routed power, etc
- `Independent or Sequential Outputs with Grid Excess Sharing`: Outputs are sequential by default (second output activated after first one at 100%).
  **Independent outputs are also supported** thanks to the sharing feature to split the excess between outputs.

### Grid Power Measurement

Measuring the grid power is essential to know how much power is available to route.
One of the following method is required, either with a JSY-MK-194T or remotely.

#### JSY-MK-194T

Preferred method to measure the grid power and routed power. it is accurate and reliable and store energy data.
_Note that the `JSY-MK-194T` has 2 channels, so it can be used both to measure the grid power but also to measure the total routed power of the router (2 outputs combined)._
_It cannot be used though to independently measure each router output._

#### Remote Grid Measurement

- `Remote JSY`: Read the grid power from a remote JSY at a rate of **20 messages per second** (nearly as fast as having the JSY wired to the ESP). There is no impact on routing precision!
- `MQTT`: Read the excess power remotely from a remote MQTT topic, for example from a **Shelly EM**, **Home Assistant**, **Jeedom**, etc.
  This is less accurate but still works and facilitate the building of a router.

#### 3-Phase Support

The router outputs are mono-phase, but the router supports measuring power on a 3-phase systems:

- Either by receiving the voltage and aggregated power through `MQTT` from a `Shelly 3EM` for example
- Either by using a `JSY-MK-333` (🚧)

### 2x Relays

- `NO / NC` relay type
- `Automatic Control` / `Manual Control`: You can specify the resistive load power in watts connected to the relay.
  If you do so, the relay will be activated automatically based on the excess power.

### Monitoring and Management

- `Charts`: Live charts displayed for some important metrics
- `Display` Add I2C OLED Display (`SSD1307`, `SH1106`, `SH1107`)
- `Health Status`: configuration mistakes are detected as much as possible and issues displayed when a component was found to not properly work.
- `Languages (i18n)`: en / fr
- `LEDs`: Add LEDs for visual alerts
- `Manual Override`: Override anything remotely (MQTT, REST, Website)
- `Pinout Map`: 2 pinout maps show the view of configured pins and activated pins, to report issues, duplications or invalid pins.
- `Push Button`: Add a push button to restart the device
- `Restart`, `Factory Reset`, `Config Backup & Restore`, `Debug Logging`
- `Statistics`: Harmonic information, power factor, energy, routed power, grid power, grid frequency and voltage, etc
- `Temperature Sensor`: for router box heat monitoring (Supported type: `DS18B20`)
- `Web console`: View ESP logs live from a Web interface
- `Web OTA Firmware Update`: Update firmware through the Web interface

### MQTT, REST API and Home Automation Systems

The router exposes a lot of statistics and information through MQTT and REST API and provides a very good integration with Home Assistant or other home automation systems.
The router can be completely controlled remotely through a Home Automation System.

- `REST API`: extensive REST API support
- `MQTT`: extensive MQTT API (with `TLS` support)
- [Home Assistant Integration](https://www.home-assistant.io/integrations/mqtt/#mqtt-discovery): Activate Home Assistant Auto Discovery to automatically create a YaS☀️lR device in Home Assistant with all the sensors

### Networking / Offline

- `Access Point Mode`: router can **work in AP mode without WiFi and Internet**
- `Admin Password`: to protect the website, API and Access Point
- `Captive Portal` a captive portal is started first time to help you connect the router
- `DNS & mDNS`: Discover the device on the network through mDNS (Bonjour) or DNS
- `Ethernet & Wifi`: **ESP32 boards with Ethernet and WiFi are supported** (see [Compatible ESP32 Boards](/build#compatible-esp32-boards))
- `NTP` support to synchronize time and date with Internet. If not activated, it is still possible to manually sync with your browser.
- `Offline Mode`: **The router can work without WiFi, even teh features requiring time and date.**

### Remote Capabilities

You can split the router in several modules to facilitate the installation.
Modules can communicate through `UDP` very fast, or through `ESP-Now` when WiFi is not available for a long-range communication.

- **Setup 1**: measurement module on the main electric panel with a JSY, YaSolR router close to the loads:

  - `Remote JSY-MK-194T with UDP on same WiFi`
  - `Remote JSY-MK-194T with ESP-Now` (🚧)
  - `Remote JSY-MK-333 with UDP on same WiFi` (🚧)
  - `Remote JSY-MK-333 with ESP-Now` (🚧)

- **Setup 2**: YaSolR router with the main electric panel with measurement module, remote dimmers and relays close to the loads.
  - `Remote Relays with UDP on same WiFi` (🚧)
  - `Remote Relays with ESP-Now` (🚧)
  - `Remote Dimmers with UDP on same WiFi` (🚧)
  - `Remote Dimmers with ESP-Now` (🚧)

### Virtual Excess and EV Charger Compatibility

Thanks to power measurement, the router also provides these features:

- `Virtual Excess`: Expose the virtual excess (MQTT, REST API, web) which is composed of the current excess plus the routing power
- `EV Charger Compatibility` (i.e OpenEVSE): Don't prevent an EV charge to start (router can have lower priority than an EV box to consume available production excess)

### OSS vs PRO

OSS and Pro firmware are the same, except that the PRO version relies on commercial (paid) libraries and provides some additional features based on a better dashboard.

**The Pro version is only 25 euros** and gives access to all the perks of the Pro version below:

| Feature                    |                            OSS (Free)                            |                                                                                              PRO (Paid)                                                                                              |
| -------------------------- | :--------------------------------------------------------------: | :--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------: |
| Dashboard                  |                        Overview **only**                         |                                                                              Full Dashboard as seen in the screenshots                                                                               |
| Manual Dimmer Control      |              Home Assistant<br>MQTT API<br>REST API              |                                                                     **From Dashboard**<br>Home Assistant<br>MQTT API<br>REST API                                                                     |
| Manual Bypass Control      |              Home Assistant<br>MQTT API<br>REST API              |                                                                     **From Dashboard**<br>Home Assistant<br>MQTT API<br>REST API                                                                     |
| Manual Relay Control       |              Home Assistant<br>MQTT API<br>REST API              |                                                                     **From Dashboard**<br>Home Assistant<br>MQTT API<br>REST API                                                                     |
| Configuration              |                        Debug Config Page                         |                                                                               **From Dashboard**<br>Debug Config Page                                                                                |
| Health View from Dashboard |                                ❌                                |                                                                                                  ✅                                                                                                  |
| Statistics and Charts      |                                ❌                                |                                                                                                  ✅                                                                                                  |
| PZEM Pairing               |                                ❌                                |                                                                                                  ✅                                                                                                  |
| Help & Support             |     [Facebook Group](https://www.facebook.com/groups/yasolr)     | [Facebook Group](https://www.facebook.com/groups/yasolr) <br> [Forum](https://github.com/mathieucarbou/YaSolR-OSS/discussions) <br> [Bug Report](https://github.com/mathieucarbou/YaSolR-OSS/issues) |
| Web Console                | [WebSerial Lite](https://github.com/mathieucarbou/WebSerialLite) |                                                                              [WebSerial Pro](https://www.webserial.pro)                                                                              |
| Dashboard                  |      [ESP-DASH](https://github.com/ayushsharma82/ESP-DASH)       |                                                                                 [ESP-DASH Pro](https://espdash.pro)                                                                                  |
| OTA Firmware Update        |    [ElegantOTA](https://github.com/ayushsharma82/ElegantOTA)     |                                                                               [ElegantOTA Pro](https://elegantota.pro)                                                                               |

The money helps funding the hardware necessary to test and develop the firmware.

# Alternatives and Inspirations

This project was inspired by the following awesome Solar Router projects:

- [Routeur Solaire Apper](https://ota.apper-solaire.org) (Cyril P., _[xlyric](https://github.com/xlyric)_)
- [Routeur Solaire offgrid Réseautonome](https://github.com/SeByDocKy/routeur_solaire) (Sébastien P., _[SeByDocKy](https://github.com/SeByDocKy)_)
- [Routeur Solaire MK2 PV Router](https://www.mk2pvrouter.co.uk) (Robin Emley)
- [Routeur Solaire Mk2 PV Router](https://github.com/FredM67/Mk2PVRouter) (Frédéric M.)
- [Routeur Solaire Tignous](https://forum-photovoltaique.fr/viewtopic.php?f=110&t=40512) (Tignous)
- [Routeur Solaire MaxPV](https://github.com/Jetblack31/MaxPV) (Jetblack31)
- [Routeur solaire "Le Profes'Solaire"](https://sites.google.com/view/le-professolaire/routeur-professolaire) (Anthony G., _Le Profes'Solaire_)
- [Routeur solaire "Le Profes'Solaire"](https://github.com/benoit-cty/solar-router) (Adapation from Benoit C.)
- [Routeur solaire Multi-Sources Multi-Modes et Modulaire](https://f1atb.fr/fr/realisation-dun-routeur-photovoltaique-multi-sources-multi-modes-et-modulaire/) (André B., _[F1ATB](https://github.com/F1ATB)_)
- [Routeur solaire ESP Home](https://domo.rem81.com/index.php/2023/07/18/pv-routeur-solaire/) (Remy)
