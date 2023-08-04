---
layout: default
title: Build
description: Build
---

# How to build your router

- [Disclaimer](#disclaimer)
- [Build Types](#build-types)
  - [Dimmers: Solid State Relay or Robodyn ?](#dimmers-solid-state-relay-or-robodyn-)
  - [Relays: Solid State Relay or Electromagnetic Relay ?](#relays-solid-state-relay-or-electromagnetic-relay-)
- [Compatible ESP32 Boards](#compatible-esp32-boards)
- [Compatible Hardware](#compatible-hardware)
  - [How to choose your SSR ?](#how-to-choose-your-ssr-)
  - [Examples of Shopping Lists](#examples-of-shopping-lists)
- [Default pinout per board](#default-pinout-per-board)
- [Pictures of some routers](#pictures-of-some-routers)

## Disclaimer

This website is provided for informational purposes only. By accessing this site and using the information contained herein, you accept the terms set forth in this disclaimer.

1. Accuracy of Information: We strive to provide accurate and up-to-date information on this site, but we cannot guarantee the completeness or accuracy of this information. The information provided is subject to change without notice.
2. Use of Information: Use of the information provided on this site is at your own risk. We decline all responsibility for the consequences arising from the use of this information. It is recommended that you consult a competent professional for advice specific to your situation.
3. External Links: This site may contain links to external websites which are provided for your reference and convenience. We have no control over the content of these external sites and we accept no responsibility for their content and their use.
4. Limitation of Liability: To the fullest extent permitted by applicable law, we disclaim all liability for any direct, indirect, incidental, consequential or special damages arising out of the use of, or inability to use, this website, even if we have been advised of the possibility of such damage.
5. Intellectual property: All content on this site, including but not limited to text, images, logos and videos, is protected by intellectual property laws. Any unauthorized use of this content is strictly prohibited.
   By using this site, you agree to not target the owners, administrators and authors of this site from any claims arising from your use of this website. If you do not agree to these terms, please do not use this site.

**_THIS PROJECT REQUIRES ELECTRICITY KNOWLEDGE AND WIRING FOR 230V._**
**_DO NOT CONSIDER THIS PROJECT IF YOU HAVE ANY DOUBT OR DON'T KNOW WHAT YOU ARE DOING._**
**_YOU UNDERSTAND THAT ROUTING POWER IS DONE BY MODIFYING THE GRID ALTERNATIVE CURRENT (AC) AND CAN CAUSE DAMAGES._**
**_BY USING THIS PROJECT AND FOLLOWING ITS INSTRUCTIONS, YOU ACCEPT THAT WE CANNOT BE HELD RESPONSIBLE FOR ANY DAMAGES, INJURIES, OR LOSSES CAUSED OR RELATED TO THE USE OF THIS PROJECT._**

## Build Types

YaS☀️lR supports many builds and routing algorithms.
Before building your router, you need to decide which type of technology you want to use to dim the voltage (_Burst Mode_ or _Phase Control_).
Here is a compatibility matrix for the main pieces of hardware depending on the router type you want to build.

Once you have picked up your build type, you can look at the Wiring Schema to know how to wire it and see the [Compatible Hardware](#compatible-hardware) section to know what you need to buy.

| Hardware                        | Phase Control _(\*3)_ | Burst Mode |     Nominal Load _(\*1)_      | Wiring Schemas             |
| :------------------------------ | :-------------------: | :--------: | :---------------------------: | :------------------------- |
| Robodyn 24A<br>_(alone)_        |          ✅           |     ✅     |           < 2000 W            | // TODO: add wiring schema |
| Robodyn 40A<br>_(alone)_        |          ✅           |     ✅     |           < 3000 W            | // TODO: add wiring schema |
| Random SSR<br>+ ZCD Circuit     |          ✅           |     ✅     | 1/3 of SSR rated load _(\*2)_ | // TODO: add wiring schema |
| Zero-Cross SSR<br>+ ZCD Circuit |          ❌           |     ✅     | 1/3 of SSR rated load _(\*2)_ | // TODO: add wiring schema |

- **(\*1)**: In example, a 24A Triac would support a maximum peak of 24A but it is advised to not go over 1/3 - 1/2 for the nominal load. Some people also replace the TRIAC with a better one (see [Compatible Hardware](#compatible-hardware) below)\_
- **(\*2)**: _Max Load for SSR depends on the supported SSR load. Use 1/3 rule for safety_
- **(\*3)**: Generate _harmonics_, an effect of phase control system. This is not wrong if properly maintained at an acceptable level as per the regulations.\_

**Hint:** Remember that according to regulations about harmonic, the maximum power should be at around 750W.
More than that, you are supposed to put in place filtering systems to reduce the harmonics.
So if you want to build a router that is compliant with the regulations, any build type will be fine as long as you pay attention to the generated harmonic level.

**Bypass:** Whatever the solution you pick, you can always add an optional bypass relay to send the full power to the load instead of dimming it, free of harmonics.
If no bypass relay is added, the dimmer will be used instead at 100% when pressing the bypass.

### Dimmers: Solid State Relay or Robodyn ?

Here are some pros and cons of each phase control system:

**Robodyn (TRIAC):**

- Pros:
  - cheap and easy to wire
  - 40A model comes with a heat sink and fan
  - All in one device: phase control, ZCD, heat sink, fan
- Cons:
  - limited in load to 1/3 - 1/2 of the announced load
  - 16A / 24A models comes with heat sink which is too small for its supported maximum load
  - no solution ready to attach them on a DIN rail.
  - The heat sink often has to be upgraded, except for the one on the 40 model which is already good for small loads below 2000W.
  - The ZCD circuit [is less accurate](https://github.com/fabianoriccardi/dimmable-light/wiki/About-dimmer-boards) and pulses can be harder to detect [on some boards](https://github.com/fabianoriccardi/dimmable-light/wiki/Notes-about-specific-architectures#interrupt-issue)
  - You need to go over some modifications to ([improve wiring / soldering and heat sink](https://sites.google.com/view/le-professolaire/routeur-professolaire))
  - You might need to replace the Triac or move it

**Solid State Relays:**

- Pros:
  - cheap and easy to wire
  - support higher loads
  - can be attached to a DIN rail with standard SSR clips
  - lot of heat sink models available
- Cons:
  - limited in load to 1/3 - 1/2 of the announced load
  - require an external ZCD module, heat sink and/or fan

**Heat Sink:**

In any case, do not forget to correctly dissipate the heat of your Triac / SSR using an appropriate heat sink.
Robodyn heat sink is not enough and require some tweaking (like adding a flan or de-soldering the triac and heat sink and put teh triac on a bigger heat sink).

It is best to take a vertical heat sink for heat dissipation.
In case of the Robodyn 40A, you can install it vertically.

### Relays: Solid State Relay or Electromagnetic Relay ?

The choice is yours.
The constraint is that they have to be controllable through a 3.3V DC signal.
It is easier to find SSR supporting high loads that can be controlled by a 3.3V DC signal than electromagnetic relays.
Also, SSR with a DIN Rail clip are easy to install.
On the other hand, SSR can be more affected by harmonics than electromagnetic relays and they are more expensive.

Here are some links where to fine pros and cons of each relay type:

- [Solid State Relays, Types & Usage](https://www.sound-au.com/articles/ss-relays.htm)
- [Solid State Relay Guide](https://www.phidgets.com/docs/Solid_State_Relay_Guide)
- [https://www.celduc-relais.com/fr/thyristor-vs-triac/](https://www.celduc-relais.com/fr/thyristor-vs-triac/)
- [Fonctionnement du relais statique](https://www.geya.net/fr/solid-state-relay-working-how-does-a-solid-state-relay-work/)

## Compatible ESP32 Boards

The full list of ESP32 boards can be found [here](https://docs.platformio.org/en/stable/boards/index.html#espressif-32).
Here are the boards we know are compatible and those we have tested.

| **Board**          | **Compatible** | **Tested** | **Ethernet** | **Typical Name**                                                                                                                        |
| :----------------- | :------------: | :--------: | :----------: | :-------------------------------------------------------------------------------------------------------------------------------------- |
| esp32              |       ✅       |     ✅     |              | [ESP32 NodeMCU Dev Kit C](https://docs.platformio.org/en/stable/boards/espressif32/esp32dev.html)                                       |
| esp32s             |       ✅       |     ✅     |              | [ESP32S NodeMCU Dev Kit C](https://docs.platformio.org/en/stable/boards/espressif32/nodemcu-32s.html)                                   |
| esp32c3            |       ✅       |            |              | [Espressif ESP32-C3-DevKitC-02](https://docs.platformio.org/en/stable/boards/espressif32/esp32-c3-devkitc-02.html)                      |
| esp32s3            |       ✅       |     ✅     |              | [Espressif ESP32-S3-DevKitC-1-N8 (8 MB QD, No PSRAM)](https://docs.platformio.org/en/stable/boards/espressif32/esp32-s3-devkitc-1.html) |
| d1_mini32          |       ❌       |            |              | [WEMOS D1 MINI ESP32](https://docs.platformio.org/en/stable/boards/espressif32/wemos_d1_mini32.html)                                    |
| lolin32_lite       |       ✅       |            |              | [WEMOS LOLIN32 Lite](https://docs.platformio.org/en/stable/boards/espressif32/lolin32_lite.html)                                        |
| lolin_c3_mini      |       ❌       |            |              | [WEMOS LOLIN C3 Mini](https://docs.platformio.org/en/stable/boards/espressif32/lolin_c3_mini.html)                                      |
| lolin_s2_mini      |       ✅       |            |              | [WEMOS LOLIN S2 Mini](https://docs.platformio.org/en/stable/boards/espressif32/lolin_s2_mini.html)                                      |
| esp32_poe          |       ✅       |            |      ✅      | [OLIMEX ESP32-PoE](https://docs.platformio.org/en/stable/boards/espressif32/esp32-poe.html)                                             |
| wt32_eth01         |       ✅       |     ✅     |      ✅      | [Wireless-Tag WT32-ETH01 Ethernet Module](https://docs.platformio.org/en/stable/boards/espressif32/wt32-eth01.html)                     |
| lilygo_eth_lite_s3 |       ✅       |     ✅     |      ✅      | [T-ETH-Lite ESP32 S3](https://www.lilygo.cc/products/t-eth-lite?variant=43120880779445)                                                 |
| m5stack-atom       |       ✅       |            |              | [M5Stack-ATOM](https://docs.platformio.org/en/stable/boards/espressif32/m5stack-atom.html)                                              |
| m5stack-atoms3     |       ✅       |            |              | [M5Stack AtomS3](https://docs.platformio.org/en/stable/boards/espressif32/m5stack-atoms3.html)                                          |

_Compatible_ means a firmware for this board can at least be built and flashed.
_Not Compatible_ means that the firmware cannot be built for this board or that the board has been unsuccessfully tested.

_Tested_ means someone has verified that firmware is working or partially working on this board.
_No Tested_ means that we do not have the ability to test, but the board is at least compatible.

_🚧_ means _In Progress_

## Compatible Hardware

Here is the non exhaustive hardware that is compatible with YaS☀️lR firmware.
Links are provided for reference only, you can find them on other websites.

YaS☀️lR supports many builds and routing algorithms.
To know what you need to buy, please read the Wiring section below to chose the right hardware depending on which router you want to build.

| **ESP32 Boards (\*)**                                                            | Micro-controllers                                                                                                                                                                                                                                                                                                                                                                             |
| :------------------------------------------------------------------------------- | :-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| <img src="./assets/img/hardware/ESP32_NodeMCU.jpeg" style="width:150px">         | ESP32 NodeMCU [Dev Kit C](https://www.az-delivery.de/en/products/esp32-developmentboard), [Dev Kit C](https://fr.aliexpress.com/item/1005005033683862.html), [Dev Kit C v4](https://fr.aliexpress.com/item/1005004936575415.html), [Dev Kit C v4](https://fr.aliexpress.com/item/1005005060818892.html)                                                                                       |
| <img src="./assets/img/hardware/Nodemcu-ESP-32S.jpeg" style="width:150px">       | ESP-32S NodeMCU [ESP-32S](https://www.az-delivery.de/fr/products/nodemcu-esp-32s-kit), [ESP-32S](https://fr.aliexpress.com/item/1005005057893916.html), [ESP-32S/WROOM-32E/WROOM-32UE/WROVER-E/WROVER-IE](https://fr.aliexpress.com/item/1005004571486357.html)                                                                                                                               |
| <img src="./assets/img/hardware/ESP32-S3.jpeg" style="width:150px">              | ESP32-S3 [N8R2/N16R8](https://fr.aliexpress.com/item/1005006266375800.html), [N16R8](https://fr.aliexpress.com/item/1005005078651330.html)                                                                                                                                                                                                                                                    |
| <img src="./assets/img/hardware/LILYGO-T-ETH-Lite.jpeg" style="width:150px">     | [LILYGO T-ETH-Lite ESP32-S3](https://www.lilygo.cc/products/t-eth-lite) (Ethernet)                                                                                                                                                                                                                                                                                                            |
| <img src="./assets/img/hardware/WT32-ETH01.jpeg" style="width:150px">            | [WT32-ETH01](https://fr.aliexpress.com/item/1005004436473683.html) v1.4 (Ethernet)                                                                                                                                                                                                                                                                                                            |
| <img src="./assets/img/hardware/Pigtail_Antenna.jpeg" style="width:150px">       | [WiFi Pigtail Antenna](https://fr.aliexpress.com/item/32957527411.html) for ESP32 boards supporting external WiFi antenna                                                                                                                                                                                                                                                                     |
| **Dimmers (\*)**                                                                 | Dimmers are required and must be selected carefully depending on the load and the routing algorithms                                                                                                                                                                                                                                                                                          |
| <img src="./assets/img/hardware/Robodyn_24A.jpeg" style="width:150px">           | [Robodyn AC Dimmer 24A/600V](https://www.aliexpress.com/item/1005001965951718.html) Includes ZCD, supports **Phase Control** and **Burst mode**                                                                                                                                                                                                                                               |
| <img src="./assets/img/hardware/Robodyn_40A.jpeg" style="width:150px">           | [Robodyn AC Dimmer 40A/800V](https://fr.aliexpress.com/item/1005006211999051.html) Includes ZCD, supports **Phase Control** and **Burst mode**                                                                                                                                                                                                                                                |
| <img src="./assets/img/hardware/BTA40-800B.jpeg" style="width:150px">            | Triac BTA40-800B RD91 [here](https://fr.aliexpress.com/item/32892486601.html) or [here](https://fr.aliexpress.com/item/1005001762265497.html) if you want / need to replace the Triac inside your Robodyn                                                                                                                                                                                     |
| <img src="./assets/img/hardware/Random_SSR.jpeg" style="width:150px">            | [LCTC Random Solid State Relay (SSR) that can be controlled by a 3.3V DC signal](https://www.aliexpress.com/item/1005004084038828.html), ([Other LCTC vendor link](https://fr.aliexpress.com/item/1005004863817921.html)). Supports **Phase Control** and **Burst mode**, See [How to choose your SSR ?](#how-to-choose-your-ssr-) below                                                      |
| <img src="./assets/img/hardware/Random_SSR_EARU.jpeg" style="width:150px">       | [EARU Random Solid State Relay (SSR) that can be controlled by a 3.3V DC signal](https://fr.aliexpress.com/item/1005005274491662.html). Supports **Burst mode**, [Not very good for **Phase Control**](https://forum-photovoltaique.fr/viewtopic.php?f=110&t=40617&p=758386#p758386), See [How to choose your SSR ?](#how-to-choose-your-ssr-) below                                                                                                                                         |
| <img src="./assets/img/hardware/SSR_40A_DA.jpeg" style="width:150px">            | [Zero-Cross Solid State Relay (SSR) that can be controlled by a 3.3V DC signal](https://fr.aliexpress.com/item/1005003216482476.html) Supports **Burst mode**, See [How to choose your SSR ?](#how-to-choose-your-ssr-) below                                                                                                                                                                 |
| **Zero-Cross Detection (\*)**                                                    | ZDC circuits are used to detect when the AC voltage crosses the 0V. It is either included in dimmers like Robodyn, or need to be added as an external device. This is required for a router to properly work and control the output power.                                                                                                                                                    |
| <img src="./assets/img/hardware/ZCD.jpeg" style="width:150px">                   | [Very good ZDC module for DIN Rail](https://www.pcbway.com/project/shareproject/120_220VAC_Opto_Isolated_Zero_Cross_Detector_Module_for_DIN_Rail_Carrier_Mount_w_57a9f83e.html) (used in conjunction with a Random SSR)                                                                                                                                                                       |
| **Power and Energy Monitoring (\*)**                                             | List of devices used to measure Grid Power and routed power. Measuring the Grid Power is required.                                                                                                                                                                                                                                                                                            |
| <img src="./assets/img/hardware/JSY-MK-194T_1.jpeg" style="width:150px">         | [JSY-MK-194T with 1 fixed tore and 1 remote clamp](https://www.aliexpress.com/item/1005005396796284.html) Used to measure the grid power and total routed power                                                                                                                                                                                                                               |
| <img src="./assets/img/hardware/JSY-MK-194T_2.jpeg" style="width:150px">         | [JSY-MK-194T with 2 remote clamps](https://fr.aliexpress.com/item/1005005529999366.html) Used to measure the grid power and total routed power                                                                                                                                                                                                                                                |
| <img src="./assets/img/hardware/PZEM-004T.jpeg" style="width:150px">             | Peacefair PZEM-004T V3 100A Openable (with clamp) [official](https://fr.aliexpress.com/item/33043137964.html), [with connector](https://fr.aliexpress.com/item/1005005984795952.html), [USB-TTL Cable](https://fr.aliexpress.com/item/1005006255175075.html). Can be used to measure each output individually and more precisely. Several PZEM-004T can be connected to the same Serial port. |
| <img src="./assets/img/hardware/Shelly_EM.png" style="width:150px">              | [Shelly EM](https://www.shelly.com/en-fr/products/product-overview/shelly-em-120a/shelly-em-2x-50a) (or any other alternative sending data to MQTT)                                                                                                                                                                                                                                           |
| **Temperature Sensors**                                                          | Used to activate some router features such as auto-bypass and monitor the heating of the router box                                                                                                                                                                                                                                                                                           |
| <img src="./assets/img/hardware/DS18B20.jpeg" style="width:150px">               | [DS18B20 Temperature Sensor + Adapter](https://fr.aliexpress.com/item/4000143479592.html) (easier to use to install in the water tank - take a long cable)                                                                                                                                                                                                                                    |
| **Bypass and External Relay**                                                    | Used to activate some router features such as controlling other loads, and also efficiently bypass the dimmers when forcing a planned heating                                                                                                                                                                                                                                                 |
| <img src="./assets/img/hardware/DIN_1_Relay.jpeg" style="width:150px">           | 1-Channel 5V DC / 30A Electromagnetic Relay on DIN Rail Support: [here](https://fr.aliexpress.com/item/1005004908430389.html), [here](https://fr.aliexpress.com/item/32999654399.html), [here](https://fr.aliexpress.com/item/1005005870389973.html), [here](https://fr.aliexpress.com/item/1005005883440249.html)                                                                            |
| <img src="./assets/img/hardware/DIN_2_Relay.jpeg" style="width:150px">           | 2-Channel 5V DC / 30A Dual Electromagnetic Relays on DIN Rail Support: [here](https://fr.aliexpress.com/item/1005004899369193.html), [here](https://fr.aliexpress.com/item/32999654399.html), [here](https://fr.aliexpress.com/item/1005005870389973.html), [here](https://fr.aliexpress.com/item/1005005883440249.html), [here](https://fr.aliexpress.com/item/1005001543232221.html)        |
| <img src="./assets/img/hardware/SSR_40A_DA.jpeg" style="width:150px">            | [Solid State Relay](https://fr.aliexpress.com/item/1005003216482476.html) (See [How to choose your SSR ?](#how-to-choose-your-ssr-) below)                                                                                                                                                                                                                                                    |
| **Screens**                                                                      | Optionally add a screen                                                                                                                                                                                                                                                                                                                                                                       |
| <img src="./assets/img/hardware/SSD1306.jpeg" style="width:150px">               | [SSD1306 OLED Display 4 pins 128x64 I2C](https://www.aliexpress.com/item/32638662748.html)                                                                                                                                                                                                                                                                                                    |
| <img src="./assets/img/hardware/SH1106.jpeg" style="width:150px">                | [SH1106 OLED Display 4 pins 128x64 I2C](https://www.aliexpress.com/item/1005001621782442.html)                                                                                                                                                                                                                                                                                                |
| <img src="./assets/img/hardware/SH1107.jpeg" style="width:150px">                | [SSD1307 OLED Display 4 pins 128x64 I2C](https://www.aliexpress.com/item/1005003297480376.html)                                                                                                                                                                                                                                                                                               |
| **Manual Control and Status**                                                    | Optionally add LEDs, push button, buzzer                                                                                                                                                                                                                                                                                                                                                      |
| <img src="./assets/img/hardware/PushButton.jpeg" style="width:150px">            | Push Buttons [Amazon](https://www.amazon.fr/dp/B0C2Y46BK6) (16mm) [AliExpress](https://fr.aliexpress.com/item/4001081212556.html) (12mm) for restart, manual bypass, reset                                                                                                                                                                                                                    |
| <img src="./assets/img/hardware/LEDs.jpeg" style="width:150px">                  | Traffic lights Lights module for system status [AZ-Delivery](https://www.az-delivery.de/en/products/led-ampel-modul), [AliExpress](https://fr.aliexpress.com/item/32957515484.html)                                                                                                                                                                                                           |
| <img src="./assets/img/hardware/Passive_Buzzer.jpeg" style="width:150px">        | [Passive Piezo Buzzer](https://www.az-delivery.de/en/products/buzzer-modul-passiv) (for alerts)                                                                                                                                                                                                                                                                                               |
| **Heat Dissipation**                                                             |                                                                                                                                                                                                                                                                                                                                                                                               |
| <img src="./assets/img/hardware/Heat_Sink.jpeg" style="width:150px">             | [Heat Sink for Random SSR and Triac](https://fr.aliexpress.com/item/1005004879389236.html) (there are many more types available: take a big heat sink placed vertically)                                                                                                                                                                                                                      |
| <img src="./assets/img/hardware/SSR_Heat_Sink.png" style="width:150px">          | [Heat Sink for SSR](https://fr.aliexpress.com/item/32739226601.html) (there are many more types available: take a big heat sink placed vertically)                                                                                                                                                                                                                                            |
| <img src="./assets/img/hardware/Raspberry_Fans.jpeg" style="width:150px">        | [Raspberry Fans](https://www.az-delivery.de/en/products/raspberry-pi-dc-burstenlose-lufter-kuhlkorper-kuhler-3-3-v-5-v) (for Robodyn AC dimmer)                                                                                                                                                                                                                                               |
| **Mounting Accessories**                                                         | Some useful accessories to help mount components together                                                                                                                                                                                                                                                                                                                                     |
| <img src="./assets/img/hardware/Electric_Box.jpeg" style="width:150px">          | [Electric Box](https://www.amazon.fr/gp/product/B0BWFGVV4S)                                                                                                                                                                                                                                                                                                                                   |
| <img src="./assets/img/hardware/Extension_Board.jpeg" style="width:150px">       | [Extension boards](https://www.amazon.fr/dp/B0BCWBW4SR) (pay attention to the distance between header, there are different models. This one fits the ESP32 NodeMCU above)                                                                                                                                                                                                                     |
| <img src="./assets/img/hardware/ZCD_DIN_Rail.jpeg" style="width:150px">          | [DIN Rail Mount for PCB 72mm x 20mm](https://fr.aliexpress.com/item/32276247838.html) for the ZCD module above to mount on DIN Rail. [Alternative link](https://fr.aliexpress.com/item/4000272944733.html)                                                                                                                                                                                    |
| <img src="./assets/img/hardware/ESP32S_Din_Rail_Mount.jpeg" style="width:150px"> | [DIN Rail Mount for ESP32 NodeMCU Dev Kit C](https://fr.aliexpress.com/item/1005005096107275.html)                                                                                                                                                                                                                                                                                            |
| <img src="./assets/img/hardware/Distrib_DIN.jpeg" style="width:150px">           | [Distribution Module](https://fr.aliexpress.com/item/1005005996836930.html)                                                                                                                                                                                                                                                                                                                   |
| <img src="./assets/img/hardware/DIN_SSR_Clip.png" style="width:150px">           | [DIN Rail Clips for SSR](https://fr.aliexpress.com/item/1005004396715182.html)                                                                                                                                                                                                                                                                                                                |
| <img src="./assets/img/hardware/DIN_HDR-15-5.jpeg" style="width:150px">          | AC-DC 5V 2.4A DIN Adapter HDR-15-5 [Amazon](https://www.amazon.fr/Alimentation-rail-Mean-Well-HDR-15-5/dp/B06XWQSJGW), [AliExpress](https://fr.aliexpress.com/item/4000513120668.html). Can be used to power the ESP when installed in an electric box on DIN rail                                                                                                                            |
| **Wires**                                                                        | Some useful accessories to help mount components together                                                                                                                                                                                                                                                                                                                                     |
| <img src="./assets/img/hardware/DupontWire.jpeg" style="width:150px">            | [Dupont Cable Kit](https://fr.aliexpress.com/item/1699285992.html)                                                                                                                                                                                                                                                                                                                            |
| <img src="./assets/img/hardware/RC_Snubber.jpeg" style="width:150px">            | [100 ohms 0.1uF RC Snubber](https://www.quintium.fr/shelly/168-shelly-rc-snubber.html) (for Robodyn AC dimmer and Random SSR: can be placed at dimmer output)                                                                                                                                                                                                                                 |

**(\*)** Required items

**IMPORTANT NOTES:**

1. It is possible to switch the TRIAC of an original Robodyn AC Dimmer with a higher one, for example a [BTA40-800B BOITIER RD-91](https://fr.farnell.com/stmicroelectronics/bta40-800b/triac-40a-800v-boitier-rd-91/dp/9801731)<br/>
   Ref: [Triacs gradateurs pour routeur photovoltaïque](https://f1atb.fr/fr/triac-gradateur-pour-routeur-photovoltaique/).

2. The heat sink must be chosen according to the SSR / Triac. Here is a good video about the theory: [Calcul du dissipateur pour le triac d'un routeur](https://www.youtube.com/watch?v=_zAx1Q2IvJ8) (from Pierre)

3. Make sure to [improve the Robodyn wiring/soldering](https://sites.google.com/view/le-professolaire/routeur-professolaire)

### How to choose your SSR ?

Solid State Relays can be used:

- for routing the power to the load (either random or ZC)
- for Bypass and External Relays

Things to consider:

- Make sure you add a heat sink to the SSR or pick one with a heat sink, especially if you use a Random SSR instead of a Robodyn
- Type of control: DA: (DC Control AC)
- Control voltage: 3.3V should be in the range (example: 3-32V DC)
- Verify that the output AC voltage is in the range (example: 24-480V AC)
- Verify the SSR amperage: usually, it should be 2-3 times the nominal current of your resistive load (example: 40A SSR for a 3000W resistance)
- **Zero Cross SSR** (which is the default): for the bypass relay or external relays or when using Burst modulation routing algorithm
- **Random SSR**: if you chose to not use the RobodDyn but a Random SSR for Phase Control

Other SSR:

- [Zero-Cross SSR DA](https://fr.aliexpress.com/item/1005002297502716.html)
- [Zero-Cross SSR DA + Heat Sink + Din Rail Clip](https://www.aliexpress.com/item/1005002503185415.html) (40A, 60A, very high - can prevent closing an electric box)
- [Zero-Cross SSR 120 DA](https://www.aliexpress.com/item/1005005020709764.html) (for very high load)

### Examples of Shopping Lists

For each example below, you can add:

- ESP32 NodeMCU Dev Kit C
- DIN Rail Mount for ESP32 NodeMCU Dev Kit C
- HDR-15-5 (to power the ESP)
- 1x or 2x 2-Channel 5V DC / 30A Dual Electromagnetic Relays on DIN Rail (for bypass relay + external relay)
- JSY (to measure the grid power and total routed power)
- 1x or 2x PZEM-004T (to measure each output individually and more precisely)
- Cables, LEDs, etc

**Example 1: For loads up to 2000 W**

- Robodyn 40A/800V (placed vertically)

**Example 2: For loads up to 2000 W**

- Vertical Heat Sink
- Robodyn 24A/600V (but we move the Triac on the Heat Sink above)

**Example 3: For loads up to 3000 W**

- Vertical Heat Sink
- Triac BTA40-800B RD91 (mounted on the heat sink)
- Robodyn 24A/600V (but we replace the Triac with the one above)

**Example 4: For any load - SSR based**

- Random Solid State Relay: rated 3x your load (i.e. 40DA for max 3000W)
- Heat Sink for SSR (matching your load, vertical ideally)
- DIN Rail clip for SSR
- ZCD module + DIN Rail mount

## Default pinout per board

The hardware and pinout are heavily inspired by [Routeur solaire PV monophasé Le Profes'Solaire](https://sites.google.com/view/le-professolaire/routeur-professolaire) from Anthony.
Please read all the information there first.
He did a very great job with some videos explaining the wiring.

Most of the features can be enabled or disabled through the app and the pinout can be changed also trough the app.

**Tested boards:**

| **FEATURE**                             | **ESP32** | **NodeMCU-32S** | **esp32s3** | **wt32_eth01** | **T-ETH-Lite** |
| :-------------------------------------- | :-------: | :-------------: | :---------: | :------------: | :------------: |
| Display CLOCK (CLK)                     |    22     |       22        |     38      |       -1       |       40       |
| Display DATA (SDA)                      |    21     |       21        |     21      |       -1       |       41       |
| JSY-MK-194T RX (Serial TX)              |    17     |       17        |     17      |       17       |       17       |
| JSY-MK-194T TX (Serial RX)              |    16     |       16        |     16      |       5        |       18       |
| Light Feedback (Green)                  |     0     |        0        |      0      |       -1       |       38       |
| Light Feedback (Red)                    |    15     |       15        |     15      |       -1       |       46       |
| Light Feedback (Yellow)                 |     2     |        2        |      2      |       -1       |       21       |
| OUTPUT #1 Bypass Relay                  |    32     |       32        |     40      |       12       |       20       |
| OUTPUT #1 Dimmer (Robodyn or SSR)       |    25     |       25        |     37      |       2        |       19       |
| OUTPUT #1 Temperature Sensor            |    18     |       18        |     18      |       15       |       3        |
| OUTPUT #2 Bypass Relay                  |    33     |       33        |     33      |       -1       |       15       |
| OUTPUT #2 Dimmer (Robodyn or SSR)       |    26     |       26        |     36      |       -1       |       7        |
| OUTPUT #2 Temperature Sensor            |     5     |        5        |      5      |       -1       |       16       |
| Passive Buzzer                          |    19     |       19        |      8      |       -1       |       2        |
| Push Button (restart and factory reset) |    23     |       23        |     39      |       -1       |       1        |
| RELAY #1                                |    13     |       13        |     13      |       14       |       5        |
| RELAY #2                                |    12     |       12        |     12      |       -1       |       6        |
| System Temperature Sensor               |     4     |        4        |      4      |       4        |       4        |
| ZCD (Robodyn or ZCD Sync)               |    35     |       35        |     35      |       35       |       8        |
| PZEM-004T v3 RX (Serial TX)             |    27     |       27        |     11      |       -1       |       -1       |
| PZEM-004T v3 TX (Serial RX)             |    14     |       14        |     14      |       -1       |       -1       |

**Compatible boards, but not tested yet:**

| **FEATURE**                             | **esp32-poe** | **ESP32-C3-DevKitC-02** | **lolin32_lite** | **lolin_s2_mini** | **m5stack-atom** | **m5stack-atoms3** |
| :-------------------------------------- | :-----------: | :---------------------: | :--------------: | :---------------: | :--------------: | :----------------: |
| Display CLOCK (CLK)                     |      16       |            6            |        22        |         9         |        -1        |         -1         |
| Display DATA (SDA)                      |      13       |            7            |        19        |         8         |        -1        |         -1         |
| JSY-MK-194T RX (Serial TX)              |      33       |           20            |        17        |        39         |        -1        |         -1         |
| JSY-MK-194T TX (Serial RX)              |      35       |           21            |        16        |        37         |        -1        |         -1         |
| Light Feedback (Green)                  |      -1       |           -1            |        0         |         3         |        -1        |         -1         |
| Light Feedback (Red)                    |      -1       |           -1            |        15        |         6         |        -1        |         -1         |
| Light Feedback (Yellow)                 |      -1       |           -1            |        2         |         2         |        -1        |         -1         |
| OUTPUT #1 Bypass Relay                  |       4       |            2            |        32        |        21         |        -1        |         -1         |
| OUTPUT #1 Dimmer (Robodyn or SSR)       |       2       |            1            |        25        |        10         |        -1        |         -1         |
| OUTPUT #1 Temperature Sensor            |       5       |            0            |        18        |        18         |        -1        |         -1         |
| OUTPUT #2 Bypass Relay                  |      -1       |            9            |        33        |        33         |        -1        |         -1         |
| OUTPUT #2 Dimmer (Robodyn or SSR)       |      -1       |            8            |        26        |        11         |        -1        |         -1         |
| OUTPUT #2 Temperature Sensor            |      -1       |            5            |        5         |         5         |        -1        |         -1         |
| Passive Buzzer                          |      -1       |           18            |        14        |        14         |        -1        |         -1         |
| Push Button (restart and factory reset) |      -1       |           10            |        23        |         7         |        -1        |         -1         |
| RELAY #1                                |      14       |           20            |        13        |        13         |        -1        |         -1         |
| RELAY #2                                |      15       |           21            |        12        |        12         |        -1        |         -1         |
| System Temperature Sensor               |       0       |            4            |        4         |         4         |        -1        |         -1         |
| ZCD (Robodyn or ZCD Sync)               |      36       |            3            |        35        |        35         |        -1        |         -1         |
| PZEM-004T v3 RX (Serial TX)             |      -1       |           -1            |        -1        |        -1         |        -1        |         -1         |
| PZEM-004T v3 TX (Serial RX)             |      -1       |           -1            |        -1        |        -1         |        -1        |         -1         |

`-1` means not mapped (probably because the board does not have enough pins)

**Minimal requirements:**

- a pin configured to the ZCD system: either the ZC pin of the Robodyn or any pin from any other ZC detection module
- a pin configured to the Phase Control system: PSM pin for the Robodyn or DC + side of the Random SSR

The website display the pinout configured, the pinout layout that is live at runtime and also displays some potential issues like duplicate pins or wrong pin configuration.

[![](./assets/img/screenshots/pinout_configured.jpeg)](./assets/img/screenshots/pinout_configured.jpeg)

## Pictures of some routers

> _TO BE COMPLETED_
