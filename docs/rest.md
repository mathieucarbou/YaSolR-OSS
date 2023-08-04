---
layout: default
title: HTTP API
description: HTTP API
---

# Web Endpoints

- [`/api`](#api)
- [`/api/config`](#apiconfig)
- [`/api/config/backup`](#apiconfigbackup)
- [`/api/debug`](#apidebug)
- [`/api/grid`](#apigrid)
- [`/api/router`](#apirouter)
- [`/api/system`](#apisystem)
- [`/api/system/reset`](#apisystemreset)
- [`/api/restart`](#apirestart)

## `/api`

List all available endpoints

```bash
curl -X GET http://<esp-ip>/api
```

```json
{
  "config": "http://192.168.125.121/api/config",
  "config/backup": "http://192.168.125.121/api/config/backup",
  "debug": "http://192.168.125.121/api/debug",
  "grid": "http://192.168.125.121/api/grid",
  "router": "http://192.168.125.121/api/router",
  "system": "http://192.168.125.121/api/system",
  "system/restart": "http://192.168.125.121/api/system/restart"
}
```

## `/api/config`

Configuration view, update, backup and restore

```bash
curl -X GET http://<esp-ip>/api/config
```

```json
{
  "admin_pwd": "",
  "ap_mode_enable": "false",
  "debug_enable": "true",
  "disp_angle": "0",
  "disp_enable": "true",
  "disp_speed": "3",
  "disp_type": "SH1106",
  "ds18_sys_enable": "true",
  "grid_freq": "50",
  "grid_pow_mqtt": "homeassistant/states/sensor/grid_power/state",
  "grid_volt": "230",
  "grid_volt_mqtt": "",
  "ha_disco_enable": "true",
  "ha_disco_topic": "homeassistant/discovery",
  "jsy_enable": "true",
  "lights_enable": "true",
  "mqtt_enable": "true",
  "mqtt_port": "1883",
  "mqtt_pub_itvl": "5",
  "mqtt_pwd": "********",
  "mqtt_secure": "false",
  "mqtt_server": "192.168.125.90",
  "mqtt_topic": "yasolr_a1c48",
  "mqtt_user": "homeassistant",
  "ntp_server": "pool.ntp.org",
  "ntp_timezone": "Europe/Paris",
  "o1_ab_enable": "false",
  "o1_ad_enable": "false",
  "o1_days": "sun,mon,tue,wed,thu,fri,sat",
  "o1_dim_enable": "true",
  "o1_dim_limit": "100",
  "o1_ds18_enable": "true",
  "o1_pzem_enable": "true",
  "o1_relay_enable": "true",
  "o1_relay_type": "NO",
  "o1_temp_start": "50",
  "o1_temp_stop": "60",
  "o1_time_start": "22:00",
  "o1_time_stop": "06:00",
  "o2_ab_enable": "false",
  "o2_ad_enable": "false",
  "o2_days": "sun,mon,tue,wed,thu,fri,sat",
  "o2_dim_enable": "true",
  "o2_dim_limit": "27",
  "o2_ds18_enable": "true",
  "o2_pzem_enable": "true",
  "o2_relay_enable": "false",
  "o2_relay_type": "NO",
  "o2_temp_start": "50",
  "o2_temp_stop": "60",
  "o2_time_start": "22:00",
  "o2_time_stop": "06:00",
  "pin_disp_scl": "22",
  "pin_disp_sda": "21",
  "pin_ds18": "4",
  "pin_jsy_rx": "16",
  "pin_jsy_tx": "17",
  "pin_lights_g": "0",
  "pin_lights_r": "15",
  "pin_lights_y": "2",
  "pin_o1_dim": "25",
  "pin_o1_ds18": "18",
  "pin_o1_relay": "32",
  "pin_o2_dim": "26",
  "pin_o2_ds18": "5",
  "pin_o2_relay": "33",
  "pin_pzem_rx": "14",
  "pin_pzem_tx": "27",
  "pin_relay1": "13",
  "pin_relay2": "12",
  "pin_zcd": "35",
  "relay1_enable": "true",
  "relay1_load": "0",
  "relay1_type": "NO",
  "relay2_enable": "true",
  "relay2_load": "0",
  "relay2_type": "NO",
  "wifi_pwd": "",
  "wifi_ssid": "IoT",
  "zcd_enable": "true"
}
```

```bash
# Configuration Update:
curl -X POST \
  -F "hostname=foobarbaz" \
  -F "admin_password=" \
  -F "ntp_server=fr.pool.ntp.org" \
  -F "ntp_timezone=Europe/Paris" \
  [...]
  http://<esp-ip>/api/config
```

## `/api/config/backup`

```bash
# Backup configuration config.txt:
curl -X GET http://<esp-ip>/api/config/backup
```

```bash
# Restore configuration config.txt:
curl -X POST -F "data=@./path/to/config.txt" http://<esp-ip>/api/config/restore
```

## `/api/debug`

Display many internal information about each hardware component, system and tasks

```bash
curl -X GET http://<esp-ip>/api/debug
```

## `/api/grid`

Display grid electricity information

```bash
curl -X GET http://<esp-ip>/api/grid
```

```json
{
  "apparent_power": 0,
  "current": 0,
  "energy": 0.029999999,
  "energy_returned": 0,
  "frequency": 49.97999954,
  "online": true,
  "power": 723.661377,
  "power_factor": 0,
  "voltage": 233.9530029
}
```

## `/api/router`

Show the router information and allows to control the relays, dimmers and bypass

```bash
curl -X GET http://<esp-ip>/api/router
```

```json
{
  "energy": 0.193000004,
  "lights": "🟢 🟡 ⚫",
  "power": 334.1828918,
  "power_factor": 0.726000011,
  "temperature": 24.80999947,
  "thdi": 1.550398946,
  "virtual_grid_power": 671.1778564,
  "output1": {
    "bypass": "off",
    "state": "Routing",
    "temperature": 24.05999947,
    "dimmer": {
      "duty": 2502,
      "duty_cycle": 61.09890366,
      "state": "on"
    },
    "metrics": {
      "apparent_power": 0,
      "current": 0,
      "energy": 0.101000004,
      "power": 0,
      "power_factor": 0.781657875,
      "resistance": 0,
      "thdi": 1.550398946,
      "voltage_dimmed": 180.6411438
    },
    "relay": {
      "state": "off",
      "switch_count": 0
    }
  },
  "output2": {
    "bypass": "off",
    "state": "Routing",
    "temperature": 24.05999947,
    "dimmer": {
      "duty": 2502,
      "duty_cycle": 61.09890366,
      "state": "on"
    },
    "metrics": {
      "apparent_power": 0,
      "current": 0,
      "energy": 0.092,
      "power": 0,
      "power_factor": 0.541782916,
      "resistance": 0,
      "thdi": 1.550398946,
      "voltage_dimmed": 125.3685608
    },
    "relay": {
      "state": "off",
      "switch_count": 0
    }
  },
  "relay1": {
    "state": "off",
    "switch_count": 0
  },
  "relay2": {
    "state": "off",
    "switch_count": 0
  }
}
```

```bash
# Change relay state for a specific duration (duration is optional)
curl -X POST \
  -F "state=on" \
  -F "duration=20000" \
  http://<esp-ip>/api/router/relay1
```

```bash
# Set the duty of the dimmer
curl -X POST \
  -F "duty=4095" \
  http://<esp-ip>/api/router/output1/dimmer
```

```bash
# Set the duty cycle of the dimmer [0.0, 100.0]
curl -X POST \
  -F "duty_cycle=50.55" \
  http://<esp-ip>/api/router/output1/dimmer
```

```bash
# Change bypass relay state
curl -X POST \
  -F "state=on" \
  http://<esp-ip>/api/router/output1/bypass
```

## `/api/system`

System information: device, memory usage, network, application, router temperature, etc

```bash
curl -X GET http://<esp-ip>/api/system
```

```json
{
  "app": {
    "manufacturer": "Mathieu Carbou",
    "model": "Pro",
    "name": "YaSolR",
    "version": "main_927a10c_modified"
  },
  "device": {
    "boots": 980,
    "cores": 2,
    "cpu_freq": 240,
    "heap": {
      "total": 272852,
      "usage": 47.74000168,
      "used": 130260
    },
    "id": "A1C48",
    "model": "ESP32-D0WD",
    "revision": 301,
    "uptime": 5109
  },
  "firmware": {
    "build": {
      "branch": "main",
      "hash": "927a10c",
      "timestamp": "2024-06-08T10:17:11.607303+00:00"
    },
    "debug": true,
    "filename": "YaSolR-main-pro-esp32-debug.bin"
  },
  "network": {
    "eth": {
      "ip_address": "0.0.0.0",
      "mac_address": ""
    },
    "hostname": "yasolr-a1c48",
    "ip_address": "192.168.125.121",
    "mac_address": "B0:B2:1C:0A:1C:48",
    "mode": "wifi",
    "ntp": "on",
    "wifi": {
      "bssid": "00:17:13:37:28:C0",
      "ip_address": "192.168.125.121",
      "mac_address": "B0:B2:1C:0A:1C:48",
      "quality": 91,
      "rssi": -35,
      "ssid": "IoT"
    }
  }
}
```

## `/api/system/reset`

```bash
# System Restart
curl -X POST http://<esp-ip>/api/system/restart
```

## `/api/restart`

```bash
# System Factory Reset
curl -X POST http://<esp-ip>/api/system/reset
```
