---
layout: default
title: Home
description: Home
---

# What is YaS☀️lR ?

YaS☀️lR is an Arduino / ESP32 firmware for Solar Routers, compatible with most of existing and easy to source hardware components.

If you have a Solar Router at home built by yourself or someone else, there is a good chance that YaS☀️lR will be compatible.

[![](./assets/img/screenshots/overview.jpeg)](./assets/img/screenshots/overview.jpeg)

A _Solar Router_ allows to redirect the solar production excess to some appliances instead of returning it to the grid. To work properly, it requires to be connected to a resistive load, such as the resistance of a water tank, because the application will adapt the power sent to this resistance by reducing or increasing its voltage.

A router can also do more things, like controlling (on/off) the activation of other appliances (with the grid normal voltage and not the dimmed voltage) in case the excess reaches a threshold. For example, one could activate a pump, pool heater, etc if the excess goes above a specific amount, so that this appliance gets the priority over heating the water tank.

A router can also schedule some forced heating of the water tank to ensure the water reaches a safe temperature, and consequently bypass the dimmed voltage. This is called a bypass relay.

## YaS☀️lR Vision

_YaSolR_ is built with this vision in mind:

- **OpenSource**: anybody can help and participate in the project

- **Good development practices**, PlatformIO project with versioning and version control, issue management, etc: this is big project and not a simple Arduino project (.ino file)

- **Flexible**: it is made in a way that you can only pick the components you need depending on the features you want to activate

- **Hardware compatibility**: supports several phase control systems like Random SSR, Robodyn, many ESP32 boards with WiFi and Ethernet

- **Easy to build**: I wanted to propose a router that everyone can build, with few components to assemble, but at the same time be able to use more powerful components

- **Easy to repair**: any piece can be easily repaired: there is no complex PCB to solder

- **Easy to use**: The firmware should be easy to install and use

- **Little to No Harmonics**: Try to be smart about harmonic effects and help you take actions to reduce them or totally remove them with Burst Fire mode

- **Automation and Integration**: Web API, MQTT Api, Home Assistant auto-discovery, etc

- **230V 50hz and 110V 60hz**: the dimmer implementation and proposed hardware all support both frequencies ad voltages

- **Statistics / API / Home Automation**: The router exposes a lot of statistics and information through MQTT and REST API and provides a very good integration with Home Assistant or other home automation systems.
  The router can be completely controlled remotely through a Home Automation System.

A Pro version (which includes the use of some commercial libraries) is also provided to get even more features and support.

## YaS☀️lR Features

_🚧_ means _In Progress_

### Up to 2 Routing Outputs

A routing output is connected to a resistive load and controls its power. Each output is composed of:

- A **Dimmer** (_required_) to control the power sent to the load. Supported dimmers:

  - `Robodyn` (Phase Control and Burst mode)
  - `Random Solid Sate Relays` (Phase Control and Burst mode) associated with a Zero-Cross Detection module
  - `Zero-Cross Solid Sate Relays` (Burst mode) associated with a `Zero-Cross Detection` module (ZCD)

  _Supported routing algorithms:_

  - `Phase Control` (🚧) (for Robodyn, Random SSR, Triac): more accurate but generates harmonics, should be limited to light loads
  - `Burst Fire Control` (🚧) (for Robodyn, Random SSR, Triac, Zero-Cross SSR): no harmonic, but a little less accurate

- A **Bypass Relay** (_recommended_) to force a heating at full power by bypassing the dimmer.
  If not installed, the dimmer will be used and set to 100% or 0% to simulate the relay.

  _Supported relays for bypass:_

  - `Zero-Cross Solid Sate Relays`
  - `Random Solid Sate Relays`
  - `Electromagnetic Relays`

- A **Temperature Sensor** (_optional_) to monitor the temperature of the water boiler for example.
  If not installed, Auto Bypass based on temperature won't be possible.
  The Auto Bypass will work but temperature settings will be ignored.
  Supported sensors: `DS18B20`

- A **Measurement device** (_recommended_) to monitor the power sent to the load. Supported measurement devices:

  - `JSY-MK-194T`: to monitor the global routed power and grid power. JSY is limited to only one per UART, but has 2 channels and supports 2-way measurements
  - `PZEM-004T V3`: to precisely monitor each output. Several PZ are supported per UART, but only 1 channel and 1-way measurement, which is perfect to measure routed power

Each output supports the following features:

- `Auto Bypass`: Plan when to force a heating (days, hours, temperature range)
- `Auto Dimmer`: Take control over the routing yourself or let the router automatically dim based on measured production excess
- `Dimmer Limit`: Set a limit to the dimmer to avoid routing too much power
- `Auto calibration`: Detect and measure the resistive load to provide more accurate routing and statistics
- `Remote output` (🚧): Remotely control the router output (dimmer + relay + sensor), in case you need the router control to stay at a place and outputs to be setup remotely
- `Statistics`: Harmonic information, power factor, energy, routed power, etc
- `Independent Outputs` (🚧): Outputs are sequential by default meaning that the second output will be activated only when the first one is at 100% and there is still excess power available.
  It is possible to switch to `50%/50%` mode or `25%/75%` mode in order to independently activate both outputs and they will both receive the corresponding power.
  This is useful to heat 2 water tank at once for example, to avoid one being left at a lower temperature.
  `Independent Outputs` requires to install a PZEM measurement module on each output.

### Up to 2 Relays to connect additional loads

Relays can be uses to commute (on/off) a load based on a threshold.
Features for each relay:

- `NO / NC` relay type
- `Relay Threshold`: Set a power threshold to automatically switch the relay on/off based on the virtual excess
- `Remote relay` (🚧): Remotely control relays

### Grid Power Measurement

Measuring the grid power is essential to know how much power is available to route.
The measurement device needs to support 2-way measurement: positive power when importing, negative power when exporting and eventually positive and returned energy counters.
Supported measurement methods:

- `JSY-MK-194T`: accurate, will also store energy data
- `Remote JSY-MK-194T` (🚧): Read the measurement from a JSY board remotely
- `MQTT`: Read the excess power remotely from a remote MQTT topic, for example from a **Shelly EM**, **Home Assistant**, **Jeedom**, etc.
  This is less accurate but still works and facilitate the building of a router

Thanks to power measurement, the router also provides these features:

- `Virtual Excess`: Expose the virtual excess (MQTT, REST API, web) which is composed of the current excess plus the routing power
- `EV Charger Compatibility` (i.e OpenEVSE): Don't prevent an EV charge to start (router can have lower priority than an EV box to consume available production excess)

Note that the `JSY-MK-194T` has 2 channels, so it can be used both to measure the grid power but also to measure the total routed power of the router (2 outputs combined).
It cannot be used though to independently measure each router output.

### Home Automation Systems

The router exposes a lot of statistics and information through MQTT and REST API and provides a very good integration with Home Assistant or other home automation systems.
The router can be completely controlled remotely through a Home Automation System.

- `REST API`: extensive REST API support
- `MQTT`: extensive MQTT API (with `TLS` support)
- [Home Assistant Integration](https://www.home-assistant.io/integrations/mqtt/#mqtt-discovery): Activate Home Assistant Auto Discovery to automatically create a YaS☀️lR device in Home Assistant with all the sensors

### Monitoring and Management

- `Manual Override`: Override anything remotely (MQTT, REST, Website)
- `Statistics`: Harmonic information, power factor, energy, routed power, grid power, grid frequency and voltage, etc
- `Charts` (🚧): Live charts displayed for some important metrics
- `Restart`, `Factory Reset`, `Config Backup & Restore`, `Debug Logging`
- `Live Activation`: every router component can be activated and configured live without restart
- `Health Status`: configuration mistakes are detected as much as possible and issues displayed when a component was found to not properly work.
- `Pinout Map`: 2 pinout maps show the view of configured pins and activated pins, to report issues, duplications or invalid pins.
- `Web console`: View ESP logs live from a Web interface
- `Web OTA Firmware Update`: Update firmware through the Web interface

Additional components can be installed in the router box:

- `Temperature Sensor`: for router box heat monitoring (Supported type: `DS18B20`)
- `Push Button`: Add a push button to restart, reset, activate bypass
- `LEDs & Buzzer`: Add LEDs and Buzzer for visual and sound alerts
- `Display` Add I2C OLED Display (`SSD1307`, `SH1106`, `SH1107`)

### Networking features

- `Ethernet & Wifi`: **ESP32 boards with Ethernet and WiFi are supported** (see [Compatible ESP32 Boards](/build#compatible-esp32-boards))
- `Captive Portal` a captive portal is started first time to help you connect the router
- `Access Point Mode`: router can work in AP mode without WiFi and Internet
- `Admin Password`: to protect the website, API and Access Point
- `NTP` support to synchronize time and date with Internet.
  If not activated, it is still possible to manually sync with your browser.
- `DNS & mDNS`: Discover the device on the network through mDNS (Bonjour) or DNS

### Pro features

The Pro version is based on [ESP-DASH Pro](https://www.espdash.pro) and provides a better Web interface from which the router can be completely configured and managed.
THe Free version is based on ESP-DASH OSS and is just showing the overview page.
The configuration can still be done online through a "Debug Configuration" page.

- `Configuration Pages`: Configure components directly on website
- `Health Page`: Show status and potential issues with components
- `Management Page`: Manage router configuration (backup and restore) and reset, restarts, update
- `Pinout Pages`: Show pinout mapping and potential issues
- `Help and Support`: Help and support will be done through a private space accessible by persons who bought the pro version

# Alternatives and Inspirations

This project was inspired by the following awesome projects:

**Solar Routers based on RobotDyn AC Dimmer (or similar TRIAC based systems)**

- [Routeur solaire PV monophasé Le Profes'Solaire](https://sites.google.com/view/le-professolaire/routeur-professolaire) from Anthony
- [Solar Router](https://github.com/benoit-cty/solar-router) from [Benoit Courty](http://courty.fr)
- [Construction d'un Power Router pas à pas](https://ptiwatt.kyna.eu/post/2022/07/09/Construction-d-un-Power-Router-pas-à-pas) de Association P'TIWATT
- Mk2 PV Router
  - [MK2 PV Router](https://www.mk2pvrouter.co.uk) (old website)
  - [MK2 PV Router](https://mk2pvrouter.com) (new owners)
  - [PVRouter-Single](https://github.com/FredM67/PVRouter-Single) (adapted by Frédéric)
  - [PVRouter-3-phase](https://github.com/FredM67/PVRouter-3-phase) (adapted by Frédéric)
  - [Diverting surplus PV Power, by Robin Emley](https://docs.openenergymonitor.org/pv-diversion/mk2/index.html)
- André (F1ATB):
  - [Routeur photovoltaïque Multi-Sources Multi-Modes et Modulaire](https://f1atb.fr/fr/realisation-dun-routeur-photovoltaique-multi-sources-multi-modes-et-modulaire/)
  - [Routeur Photovoltaïque Multi-Sources Multi-Modes et Modulaire - Version 8 (DIY)](https://www.youtube.com/watch?v=6aGttWXzGhY)
- Remy (Domo rem81):
  - [HA-Routeur Solaire](https://domo.rem81.com/2023/04/14/ha-routeur-solaire/)
  - [PV-Routeur Solaire ESP Home](https://domo.rem81.com/2023/07/18/pv-routeur-solaire/)
- Cyril
  - [https://ota.apper-solaire.org](https://ota.apper-solaire.org)
  - [https://pvrouteur.apper-solaire.org](https://pvrouteur.apper-solaire.org)
  - [pv-router-esp32](https://github.com/xlyric/pv-router-esp32)
  - [PV-discharge-Dimmer-AC-Dimmer-KIT-Robotdyn](https://github.com/xlyric/PV-discharge-Dimmer-AC-Dimmer-KIT-Robotdyn)
  - [FAQ Dimmer de Cyril](https://wiki.apper-solaire.org/index.php?page=faq-dimmer)

**Solar Routers based on Random SSR**

- [MaxPV](https://github.com/Jetblack31/MaxPV)
- [Routeur Tignous](https://forum-photovoltaique.fr/viewtopic.php?f=110&t=40512)
- Seb Salva:
  - [PV-discharge-Dimmer-AC-Dimmer-KIT-Robotdyn](https://github.com/sebsalva/PV-discharge-Dimmer-AC-Dimmer-KIT-Robotdyn) (fork)
  - [MaxPV-relaisext](https://github.com/sebsalva/MaxPV-relaisext) (fork)

**Solar Routers supporting Burst Mode**

- Mk2 PV Router
  - [MK2 PV Router](https://www.mk2pvrouter.co.uk) (old website)
  - [MK2 PV Router](https://mk2pvrouter.com) (new owners)
  - [PVRouter-Single](https://github.com/FredM67/PVRouter-Single) (adapted by Frédéric)
  - [PVRouter-3-phase](https://github.com/FredM67/PVRouter-3-phase) (adapted by Frédéric)
  - [Diverting surplus PV Power, by Robin Emley](https://docs.openenergymonitor.org/pv-diversion/mk2/index.html)
- Cyril
  - [https://ota.apper-solaire.org](https://ota.apper-solaire.org)
  - [https://pvrouteur.apper-solaire.org](https://pvrouteur.apper-solaire.org)
  - [pv-router-esp32](https://github.com/xlyric/pv-router-esp32)
  - [PV-discharge-Dimmer-AC-Dimmer-KIT-Robotdyn](https://github.com/xlyric/PV-discharge-Dimmer-AC-Dimmer-KIT-Robotdyn)
  - [FAQ Dimmer de Cyril](https://wiki.apper-solaire.org/index.php?page=faq-dimmer)

**Others**

- [Tous les routeurs disponibles/connus sur le forum](https://forum-photovoltaique.fr/viewtopic.php?f=49&t=57944&p=640191#p640205)
