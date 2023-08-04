---
layout: default
title: MQTT API
description: MQTT API
---

# MQTT Topics

MQTT is supported if activated.

- `<prefix>` is set to the hostname by default.
- `<prefix>/status`: Will topic set to `online` or `offline`
- SSL is supported, but not securely: any server certificate is blindly trusted

Topics:

- [Application](#application)
- [Configuration](#configuration)
- [Grid Electricity](#grid-electricity)
- [Network](#network)
- [NTP](#ntp)
- [Relays](#relays)
- [Router](#router)
- [System](#system)

## Application

Base Topic: `<prefix>/app/`

```properties
firmware = YaSolR-main-pro-esp32-debug
id = A1C48
manufacturer = Mathieu Carbou
model = Pro
name = YaSolR
trial = false
version = main_0ed7d852_modified
```

## Configuration

Base Topic: `<prefix>/config/`

**Read-Only**

```properties
# `********` if set, blank (missing) otherwise
admin_pwd = ********

# Stay in AP mode ? `true` or `false`
ap_mode_enable = false

# Push button: restart` or `bypass` to toggle Bypass Relay
button_action = restart
# Enable Push button `true` or `false`
button_enable = true
# Push button pin
button_pin = 23

# Enable Buzzer `true` or `false`
buzzer_enable = false
# Buzzer pin
buzzer_pin = 19

# Enable debug logs (can slow down the router)
debug_enable = true

# Display Clock pin
dp_clock_pin = 22
# Display Data pin
dp_data_pin = 21
# Display Enabled: `true` or `false`
dp_enable = false
# Delay in seconds for power saving mode when no update
dp_powsave = 300
# Rotation of the screen: `0`, `90`, `180` or `270`.
dp_rotation = 0

# grid frequency: `50` or `60`
grid_freq = 50

# mqtt topic where to read grid power (optional)
grid_pow_mqtt = homeassistant/states/sensor/grid_power/state

# Enable Home Assistant Discovery `true` or `false`
ha_disco_enable = true
# Home Assistant Discovery Topic: `homeassistant/discovery`
ha_disco_topic = homeassistant/discovery

# `yasolr-wxyz` by default
hostname = yasolr-a1c48

# JSY
jsy_enable = true
jsy_rx_pin = 17
jsy_tx_pin = 16

# Enable Physical LEDs `true` or `false`
lights_enable = true
# Green Light pin
lights_g_pin = 0
# Red Light pin
lights_r_pin = 15
# Yellow Light pin
lights_y_pin = 2

# Enable MQTT `true` or `false`
mqtt_enable = true
# `1883` by default
mqtt_port = 1883
# Publish interval: `5` seconds by default
mqtt_pub_itvl = 5
mqtt_pwd = ********
mqtt_secure = false
mqtt_server = 192.168.125.90
mqtt_topic = yasolr_a1c48
mqtt_user = homeassistant

# Server: `pool.ntp.org` by default
ntp_server = pool.ntp.org
# Timezone (`Europe/Paris` by default)
ntp_timezone = Europe/Paris

# Enable router output 1 Auto Bypass ? `true` or `false`
out1_aut_enable = true
# Enable router output 1 Bypass Relay ? `true` or `false`
out1_bp_enable = true
# Router output 1 Bypass Relay pin? `true` or `false`
out1_bp_pin = 32
# `NO` or `NC` relay type
out1_bp_type = NO
# Automatic routing based on measured power or manual control over dimmer
out1_dim_auto = false
# Output 1 dimmer enabled ? `true` or `false`
out1_dim_enable = true
# Limit the dimmer power output (1-100%)
out1_dim_limit = 100
out1_dim_pin = 25
# `TRIAC`, `SSR_RANDOM`, `SSR_ZC`
out1_dim_type = TRIAC
# Auto Bypass Stop Hour `HH:mm`
out1_end_time = 06:00
# Auto Bypass Start Hour `HH:mm`
out1_start_time = 22:00
# Enable router output 1 Temperature Sensor ? `true` or `false`
out1_tmp_enable = true
# Auto Bypass Stop Temperature ? `60`
out1_tmp_high = 60
# Auto Bypass Trigger Temperature ? `50`
out1_tmp_low = 50
# Temperature Sensor pin ? `50`
out1_tmp_pin = 18
# Output 1 Auto Bypass Week Days: `sun`, `mon`, `tue`, `wed`, `thu`, `fri`, `sat`
out1_wdays = sun,mon,tue,wed,thu,fri,sat

# Same goes for second output
out2_aut_enable = false
out2_bp_enable = false
out2_bp_pin = 33
out2_bp_type = NO
out2_dim_auto = false
out2_dim_enable = true
out2_dim_limit = 100
out2_dim_pin = 26
out2_dim_type = TRIAC
out2_end_time = 06:00
out2_start_time = 22:00
out2_tmp_enable = false
out2_tmp_high = 60
out2_tmp_low = 50
out2_tmp_pin = 5
out2_wdays = sun,mon,tue,wed,thu,fri,sat

# Time to stay in portal mode when configured WiFi is not available or no WiFi configured
# `180` seconds by default (3 min)
portal_timeout = 180

# Enable External Relay #1 `true` or `false`
relay1_enable = true
# Relay pin
relay1_pin = 13
# `NO` or `NC` relay type
relay1_type = NO

# Enable External Relay #1 `true` or `false`
relay2_enable = false
# Relay pin
relay2_pin = 12
# `NO` or `NC` relay type
relay2_type = NO

# Enable System Temperature Sensor `true` or `false`
sys_tmp_enable = true
# Temperature Sensor pin
sys_tmp_pin = 4

wifi_ssid = IoT
# Timeout in seconds for WiFi connection before giving up and opening the portal, at startup
wifi_timeout = 15
wifi_pwd = ********

# ZCD
zcd_enable = true
zcd_pin = 35
```

**Write-Only**

```properties
# Set a configuration key to a new value
<prefix>/config/<key>/set
```

## Grid Electricity

Base Topic: `<prefix>/electricity/`

**Read-Only**

```properties
# will be zero if unable to detect
frequency = 50
# Whether some grid current is detected
online = true
# grid active power, measured by JSY or read from MQTT (Shelly EM for example). Can be negative
power = 6600.61
power_factor = 0
voltage = 233.80
# Grid Energy
energy = 0
# Production Excess Energy returned to the grid
energy_returned = 0

```

## Network

Base Topic: `<prefix>/network/`

**Read-Only**

```properties
ip_address = 192.168.125.119
mac_address = B0:B2:1C:0A:1C:48
wifi_bssid = 00:17:13:37:28:C0
#  0-100 (percentage), equivalent of RSSI -90 (very bad) to -30 (very good)
wifi_quality = 100
wifi_rssi = -21
wifi_ssid = IoT
```

## NTP

Base Topic: `<prefix>/ntp/`

**Read-Only**

```properties
# `true` or `false`
synced = true
```

## Relays

Base Topic:

- `<prefix>/relays/relay1/`
- `<prefix>/relays/relay1/`

**Read-Only**

```properties
# `true` or `false`
enabled = true
# `on` or `off`
state = off
# number of relay activations (on and off) since the router is running
switch_count = 0
```

**Write-Only**

```properties
# `on`, `off`, `on=20000`, `off=30000` (to only change the state for a specific duration in milliseconds and switch back to the other state)
<prefix>/relays/relay1/state/set
<prefix>/relays/relay2/state/set
```

## Router

Base Topic:

- `<prefix>/router/`

```properties
# ROUTER

# total router energy
energy = 0.05
# total routed power
power = 0.00
# power factor of the router input
power_factor = 0.00
# virtual grid power (power which can be used for an EV system to still be able to start)
virtual_grid_power = -286.26

# OUTPUT 1

# true | false
output1/enabled = true
output1/power = 1000
# DISABLED: output disabled
# ROUTING: routing mode
# BYPASS_MANUAL: manual bypass triggered by button or MQTT
# BYPASS_AUTO: auto bypass triggered by time or temperature
# IDLE: does nothing
output1/state = ROUTING

# OUTPUT 1 DIMMER

# `true` or `false`
output1/dimmer/enabled = true
# Dim level 0-100
output1/dimmer/level = 0
`on` if dimmer is active, `off` otherwise
output1/dimmer/state = on

# OUTPUT 1 BYPASS RELAY

# `true` or `false`
output1/bypass_relay/enabled = true
# `on` or `off`
output1/bypass_relay/state = off
# number of relay activations (on and off) since the router is running
output1/bypass_relay/switch_count = 0

# OUTPUT 1 TEMPERATURE

# `true` or `false`
output1/temp_sensor/enabled = true
# last temperature reading
output1/temp_sensor/temperature = 23.60
# `true` or `false`: if the last temperature reading can be considered as valid
# In case some readings fail consecutively during 1 minute, valid will be set to false
output1/temp_sensor/valid = true

# OUTPUT 2

output2/enabled = true
output2/power = 1000
output2/state = ROUTING
output2/dimmer/enabled = true
output2/dimmer/level = 0
output2/dimmer/state = on
output2/bypass_relay/enabled = true
output2/bypass_relay/state = off
output2/bypass_relay/switch_count = 0
output2/temp_sensor/enabled = true
output2/temp_sensor/temperature = 23.60
output2/temp_sensor/valid = true
```

**Write-Only**

```properties
# `on`, `off`, `on=20000`, `off=30000` (to only change the state for a specific duration in milliseconds and switch back to the other state)
<prefix>/router/output1/dimmer/level/set
<prefix>/router/output2/dimmer/level/set
```

```properties
# `on`, `off`
<prefix>/router/output1/bypass_relay/state/set
<prefix>/router/output2/bypass_relay/state/set
```

## System

Base Topic: `<prefix>/system/`

**Read-Only**

```properties
# number of times the router has been restarted
boots = 1
chip_cores = 2
chip_model = ESP32-D0WD-V3
chip_revision = 3
cpu_freq = 240
heap_total = 269404
# 0 to 100 (percentage)
heap_usage = 47.63
heap_used = 128316
uptime = 1675

# See manual for the meaning of the code
lights/code = 🟢 ⚫ ⚫
# `on` or `off`
lights/green = on
lights/red = off
lights/yellow = off

# `true` or `false`
temp_sensor/enabled = true
# last temperature reading
temp_sensor/temperature = 23.60
# `true` or `false`: if the last temperature reading can be considered as valid
# In case some readings fail consecutively during 1 minute, valid will be set to false
temp_sensor/valid = true
```
