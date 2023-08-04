---
layout: default
title: HTTP API
description: HTTP API
---

# Web Endpoints

- [Application](#application)
- [Configuration](#configuration)
- [Grid Electricity](#grid-electricity)
- [Network](#network)
- [NTP](#ntp)
- [Relays](#relays)
- [Router](#router)
- [System](#system)

## Application

```bash
curl -X GET http://<esp-ip>/api/app
```

```json
{
  "build_date": "2024-02-25T22:18:32.561669+00:00",
  "build_hash": "eeb8c4b",
  "debug": true,
  "firmware": "YaSolR-main-trial-esp32-debug.bin",
  "id": "A1C48",
  "manufacturer": "Mathieu Carbou",
  "model": "Trial",
  "name": "YaSolR",
  "trial": true,
  "version": "main_eeb8c4b_modified"
}
```

## Configuration

```bash
curl -X GET http://<esp-ip>/api/config
```

```json
{
  "admin_pwd": "",
  "ap_mode_enable": "false",
  "button_action": "restart",
  "button_enable": "true",
  "button_pin": "23",
  "buzzer_enable": "false",
  "buzzer_pin": "19",
  "debug_enable": "true",
  "dp_clock_pin": "22",
  "dp_data_pin": "21",
  "dp_enable": "true",
  "dp_powsave": "0",
  "dp_rotation": "0",
  "dp_type": "SH1106",
  "grid_freq": "0",
  "grid_pow_mqtt": "homeassistant/states/sensor/grid_power/state",
  "grid_volt": "230",
  "grid_volt_mqtt": "",
  "ha_disco_enable": "true",
  "ha_disco_topic": "homeassistant/discovery",
  "hostname": "yasolr-a1c48",
  "jsy_enable": "true",
  "jsy_rx_pin": "17",
  "jsy_tx_pin": "16",
  "lights_enable": "true",
  "lights_g_pin": "0",
  "lights_r_pin": "15",
  "lights_y_pin": "2",
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
  "out1_aut_enable": "false",
  "out1_bp_enable": "true",
  "out1_bp_pin": "32",
  "out1_bp_type": "NO",
  "out1_dim_auto": "false",
  "out1_dim_enable": "true",
  "out1_dim_limit": "100",
  "out1_dim_pin": "25",
  "out1_dim_type": "TRIAC",
  "out1_end_time": "06:00",
  "out1_pz_enable": "true",
  "out1_start_time": "22:00",
  "out1_tmp_enable": "true",
  "out1_tmp_high": "60",
  "out1_tmp_low": "50",
  "out1_tmp_pin": "18",
  "out1_wdays": "sun,mon,tue,wed,thu,fri,sat",
  "out2_aut_enable": "false",
  "out2_bp_enable": "false",
  "out2_bp_pin": "33",
  "out2_bp_type": "NO",
  "out2_dim_auto": "false",
  "out2_dim_enable": "true",
  "out2_dim_limit": "100",
  "out2_dim_pin": "26",
  "out2_dim_type": "SSR_RANDOM",
  "out2_end_time": "06:00",
  "out2_pz_enable": "true",
  "out2_start_time": "20:00",
  "out2_tmp_enable": "true",
  "out2_tmp_high": "60",
  "out2_tmp_low": "50",
  "out2_tmp_pin": "5",
  "out2_wdays": "sun,mon,tue,wed,thu,fri,sat",
  "portal_timeout": "180",
  "pzem_rx_pin": "27",
  "pzem_tx_pin": "14",
  "relay1_enable": "true",
  "relay1_pin": "13",
  "relay1_power": "0",
  "relay1_type": "NO",
  "relay2_enable": "true",
  "relay2_pin": "12",
  "relay2_power": "0",
  "relay2_type": "NO",
  "sys_tmp_enable": "true",
  "sys_tmp_pin": "4",
  "wifi_pwd": "",
  "wifi_ssid": "IoT",
  "wifi_timeout": "15",
  "zcd_enable": "true",
  "zcd_pin": "35"
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

```bash
# Backup configuration config.txt:
curl -X GET http://<esp-ip>/api/config/backup
```

```bash
# Restore configuration config.txt:
curl -X POST -F "data=@./path/to/config.txt" http://<esp-ip>/api/config/restore
```

## Grid Electricity

```bash
curl -X GET http://<esp-ip>/api/grid
```

```json
{
  "energy_returned": 0,
  "energy": 0,
  "frequency": 49.97000122,
  "online": true,
  "power": 808.9590454,
  "power_factor": 0,
  "voltage": 233.246994,
  "jsy": {
    "current1": 0.0056,
    "current2": 0,
    "enabled": true,
    "energy_returned1": 0.709999979,
    "energy_returned2": 0,
    "energy1": 0.639999986,
    "energy2": 0,
    "frequency": 49.97000122,
    "power_factor1": 0.765999973,
    "power_factor2": 0,
    "power1": 0.894400001,
    "power2": 0,
    "voltage1": 233.246994,
    "voltage2": 233.246994,
    "time": 268704
  },
  "zcd": {
    "enabled": true,
    "frequency": 49.95404053
  }
}
```

## Network

```bash
curl -X GET http://<esp-ip>/api/network
```

```json
{
  "ip_address": "192.168.125.120",
  "mac_address": "B0:B2:1C:0A:1C:48",
  "state": "STA_CONNECTED",
  "wifi_bssid": "00:17:13:37:28:C0",
  "wifi_signal": 78,
  "wifi_rssi": -43,
  "wifi_ssid": "IoT"
}
```

## NTP

```bash
curl -X GET http://<esp-ip>/api/ntp
```

```json
{
  "synced": true
}
```

## Relays

```bash
curl -X GET http://<esp-ip>/api/relays
```

```json
{
  "relay1": {
    "enabled": true,
    "state": "off",
    "switch_count": 2
  },
  "relay2": {
    "enabled": false,
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
  http://<esp-ip>/api/relays/relay1
```

## Router

```bash
curl -X GET http://<esp-ip>/api/router
```

```json
{
  "energy": 0.342999995,
  "power_factor": 0,
  "power": 0,
  "thdi": 0,
  "virtual_grid_power": 5068.793457,
  "output1": {
    "apparent_power": 0,
    "current": 0,
    "enabled": true,
    "energy": 0.089000002,
    "power_factor": 0,
    "power": 0,
    "resistance": 0,
    "state": "Idle",
    "thdi": 0,
    "voltage_in": 219.3000031,
    "voltage_out": 0,
    "bypass_relay": {
      "enabled": true,
      "state": "off",
      "switch_count": 0
    },
    "dimmer": {
      "angle_deg": 180,
      "angle_rad": 3.141592741,
      "delay": 10000,
      "enabled": true,
      "level": 0,
      "state": "off",
      "vrms_factor": 0,
      "vrms_230V": 0
    },
    "pzem": {
      "address": 1,
      "current": 0,
      "enabled": true,
      "energy": 0.089000002,
      "frequency": 50,
      "power_factor": 0,
      "power": 0,
      "voltage": 219.3000031,
      "time": 253138
    },
    "temp_sensor": {
      "enabled": true,
      "temperature": 23.05999947,
      "valid": true
    }
  },
  "output2": {
    "apparent_power": 0,
    "current": 0,
    "enabled": true,
    "energy": 0.254000008,
    "power_factor": 0,
    "power": 0,
    "resistance": 0,
    "state": "Idle",
    "thdi": 0,
    "voltage_in": 219.5,
    "voltage_out": 0,
    "bypass_relay": {
      "enabled": false,
      "state": "off",
      "switch_count": 0
    },
    "dimmer": {
      "angle_deg": 180,
      "angle_rad": 3.141592741,
      "delay": 10000,
      "enabled": true,
      "level": 0,
      "state": "off",
      "vrms_factor": 0,
      "vrms_230V": 0
    },
    "pzem": {
      "address": 2,
      "current": 0,
      "enabled": true,
      "energy": 0.254000008,
      "frequency": 50,
      "power_factor": 0,
      "power": 0,
      "voltage": 219.5,
      "time": 253073
    },
    "temp_sensor": {
      "enabled": false,
      "temperature": 0,
      "valid": false
    }
  }
}
```

```bash
# Set the dimmer to 0-100%
curl -X POST \
  -F "level=100" \
  http://<esp-ip>/api/router/output1/dimmer
```

```bash
# Change bypass relay state
curl -X POST \
  -F "state=on" \
  http://<esp-ip>/api/router/output1/bypass_relay
```

## System

```bash
curl -X GET http://<esp-ip>/api/system
```

```json
{
  "boots": 6654,
  "chip_cores": 2,
  "chip_model": "ESP32-D0WD-V3",
  "chip_revision": 3,
  "cpu_freq": 240,
  "heap_total": 261184,
  "heap_usage": 46.97000122,
  "heap_used": 122668,
  "uptime": 293,
  "buzzer": {
    "enabled": false
  },
  "lights": {
    "code": "🟢 ⚫ ⚫",
    "green": "on",
    "red": "off",
    "yellow": "off"
  },
  "stack": {
    "async_tcp": {
      "stackHighWaterMark": 1228,
      "priority": 3
    },
    "loopTask": {
      "stackHighWaterMark": 804,
      "priority": 1
    },
    "mqtt_task": {
      "stackHighWaterMark": 1436,
      "priority": 2
    },
    "meterTask": {
      "stackHighWaterMark": 1868,
      "priority": 1
    },
    "routerTask": {
      "stackHighWaterMark": 1836,
      "priority": 1
    }
  },
  "task_managers": [
    {
      "name": "loopTask",
      "tasks": [
        {
          "name": "YaSolR.updateWebsite()",
          "type": "FOREVER",
          "paused": false,
          "enabled": true,
          "interval": 1000000,
          "stats": {
            "count": 269,
            "unit": "ms",
            "bins": [0, 0, 0, 0, 63, 205, 0, 1, 0, 0, 0, 0]
          }
        },
        {
          "name": "YaSolR.publishMQTT()",
          "type": "FOREVER",
          "paused": false,
          "enabled": true,
          "interval": 5000000,
          "stats": {
            "count": 55,
            "unit": "ms",
            "bins": [0, 0, 0, 0, 0, 0, 0, 32, 23, 0, 0, 0]
          }
        },
        {
          "name": "YaSolR.publishHADiscovery()",
          "type": "ONCE",
          "paused": true,
          "enabled": true,
          "interval": 0,
          "stats": {
            "count": 1,
            "unit": "ms",
            "bins": [0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0]
          }
        }
      ]
    },
    {
      "name": "meterTask",
      "tasks": [
        {
          "name": "output1PZEM.read()",
          "type": "FOREVER",
          "paused": false,
          "enabled": true,
          "interval": 0,
          "stats": {
            "count": 2264,
            "unit": "ms",
            "bins": [0, 0, 0, 0, 0, 1264, 1000, 0]
          }
        },
        {
          "name": "output2PZEM.read()",
          "type": "FOREVER",
          "paused": false,
          "enabled": true,
          "interval": 0,
          "stats": {
            "count": 2263,
            "unit": "ms",
            "bins": [1, 0, 0, 0, 0, 381, 1881, 0]
          }
        }
      ]
    },
    {
      "name": "routerTask",
      "tasks": [
        {
          "name": "jsy.read()",
          "type": "FOREVER",
          "paused": false,
          "enabled": true,
          "interval": 0,
          "stats": {
            "count": 6052,
            "unit": "ms",
            "bins": [0, 0, 0, 0, 0, 6042, 10, 0]
          }
        }
      ]
    }
  ],
  "temp_sensor": {
    "enabled": true,
    "temperature": 24.44000053,
    "valid": true
  }
}
```

```bash
# System Restart
curl -X POST http://<esp-ip>/api/system/restart
```
