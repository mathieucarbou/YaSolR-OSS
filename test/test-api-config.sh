# SPDX-License-Identifier: GPL-3.0-or-later
#
# Copyright (C) 2023-2024 Mathieu Carbou
#
#!/usr/bin/env bash

if [ "$1" == "" ]; then
  echo "Usage: $0 <host>"
  exit 1
fi
HOST="$1"
DIR=$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" &>/dev/null && pwd)
source "${DIR}/utils/api.sh"

assert "$(keys /api/config)" "admin_pwd,ap_mode_enable,button_action,button_enable,button_pin,buzzer_enable,buzzer_pin,debug_enable,dp_clock_pin,dp_data_pin,dp_enable,dp_powsave,dp_rotation,dp_type,grid_freq,grid_pow_mqtt,ha_disco_enable,ha_disco_topic,hostname,jsy_enable,jsy_rx_pin,jsy_tx_pin,lights_enable,lights_g_pin,lights_r_pin,lights_y_pin,mqtt_enable,mqtt_port,mqtt_pub_itvl,mqtt_pwd,mqtt_secure,mqtt_server,mqtt_topic,mqtt_user,ntp_server,ntp_timezone,out1_aut_enable,out1_bp_enable,out1_bp_pin,out1_bp_type,out1_dim_auto,out1_dim_enable,out1_dim_limit,out1_dim_pin,out1_dim_type,out1_end_time,out1_start_time,out1_tmp_enable,out1_tmp_high,out1_tmp_low,out1_tmp_pin,out1_wdays,out2_aut_enable,out2_bp_enable,out2_bp_pin,out2_bp_type,out2_dim_auto,out2_dim_enable,out2_dim_limit,out2_dim_pin,out2_dim_type,out2_end_time,out2_start_time,out2_tmp_enable,out2_tmp_high,out2_tmp_low,out2_tmp_pin,out2_wdays,portal_timeout,relay1_enable,relay1_pin,relay1_power,relay1_type,relay2_enable,relay2_pin,relay2_power,relay2_type,sys_tmp_enable,sys_tmp_pin,wifi_pwd,wifi_ssid,wifi_timeout,zcd_enable,zcd_pin"
assert "$(value /api/config 'admin_pwd')" ""
assert "$(value /api/config 'ap_mode_enable')" "false"
assert "$(value /api/config 'button_action')" "restart"
assert "$(value /api/config 'button_enable')" "true"
assert "$(value /api/config 'button_pin')" "23"
assert "$(value /api/config 'buzzer_enable')" "false"
assert "$(value /api/config 'buzzer_pin')" "19"
assert "$(value /api/config 'debug_enable')" "true"
assert "$(value /api/config 'dp_clock_pin')" "22"
assert "$(value /api/config 'dp_data_pin')" "21"
assert "$(value /api/config 'dp_enable')" "true"
assert "$(value /api/config 'dp_powsave')" "0"
assert "$(value /api/config 'dp_rotation')" "0"
assert "$(value /api/config 'dp_type')" "SH1106"
assert "$(value /api/config 'grid_pow_mqtt')" "homeassistant/states/sensor/grid_power/state"
assert "$(value /api/config 'ha_disco_enable')" "true"
assert "$(value /api/config 'ha_disco_topic')" "homeassistant/discovery"
assert "$(value /api/config 'hostname')" "yasolr-a1c48"
assert "$(value /api/config 'jsy_enable')" "true"
assert "$(value /api/config 'jsy_rx_pin')" "17"
assert "$(value /api/config 'jsy_tx_pin')" "16"
assert "$(value /api/config 'lights_enable')" "true"
assert "$(value /api/config 'lights_g_pin')" "0"
assert "$(value /api/config 'lights_r_pin')" "15"
assert "$(value /api/config 'lights_y_pin')" "2"
assert "$(value /api/config 'mqtt_enable')" "true"
assert "$(value /api/config 'mqtt_port')" "1883"
assert "$(value /api/config 'mqtt_pub_itvl')" "5"
assert "$(value /api/config 'mqtt_pwd')" "********"
assert "$(value /api/config 'mqtt_secure')" "false"
assert "$(value /api/config 'mqtt_user')" "homeassistant"
assert "$(value /api/config 'ntp_server')" "pool.ntp.org"
assert "$(value /api/config 'ntp_timezone')" "Europe/Paris"
assert "$(value /api/config 'out1_aut_enable')" "true", "false"
assert "$(value /api/config 'out1_bp_enable')" "true"
assert "$(value /api/config 'out1_bp_pin')" "32"
assert "$(value /api/config 'out1_bp_type')" "NO"
assert "$(value /api/config 'out1_dim_enable')" "true"
assert "$(value /api/config 'out1_dim_limit')" "100"
assert "$(value /api/config 'out1_dim_pin')" "25"
assert "$(value /api/config 'out1_dim_type')" "TRIAC"
assert "$(value /api/config 'out1_end_time')" "06:00"
assert "$(value /api/config 'out1_start_time')" "22:00"
assert "$(value /api/config 'out1_tmp_enable')" "true"
assert "$(value /api/config 'out1_tmp_high')" "60"
assert "$(value /api/config 'out1_tmp_low')" "50"
assert "$(value /api/config 'out1_tmp_pin')" "18"
assert "$(value /api/config 'out1_wdays')" "sun,mon,tue,wed,thu,fri,sat"
assert "$(value /api/config 'out2_aut_enable')" "true"
assert "$(value /api/config 'out2_bp_enable')" "true"
assert "$(value /api/config 'out2_bp_pin')" "33"
assert "$(value /api/config 'out2_bp_type')" "NO"
assert "$(value /api/config 'out2_dim_enable')" "true"
assert "$(value /api/config 'out2_dim_limit')" "100"
assert "$(value /api/config 'out2_dim_pin')" "26"
assert "$(value /api/config 'out2_dim_type')" "TRIAC"
assert "$(value /api/config 'out2_end_time')" "06:00"
assert "$(value /api/config 'out2_start_time')" "22:00"
assert "$(value /api/config 'out2_tmp_enable')" "true"
assert "$(value /api/config 'out2_tmp_high')" "60"
assert "$(value /api/config 'out2_tmp_low')" "50"
assert "$(value /api/config 'out2_tmp_pin')" "5"
assert "$(value /api/config 'out2_wdays')" "sun,mon,tue,wed,thu,fri,sat"
assert "$(value /api/config 'portal_timeout')" "180"
assert "$(value /api/config 'relay1_enable')" "true"
assert "$(value /api/config 'relay1_pin')" "13"
assert "$(value /api/config 'relay1_type')" "NO"
assert "$(value /api/config 'relay2_enable')" "true"
assert "$(value /api/config 'relay2_pin')" "12"
assert "$(value /api/config 'relay2_type')" "NO"
assert "$(value /api/config 'sys_tmp_enable')" "true"
assert "$(value /api/config 'sys_tmp_pin')" "4"
assert "$(value /api/config 'wifi_pwd')" ""
assert "$(value /api/config 'wifi_ssid')" "IoT"
assert "$(value /api/config 'wifi_timeout')" "15"
assert "$(value /api/config 'zcd_enable')" "true"
assert "$(value /api/config 'zcd_pin')" "35"
check "$(value /api/config 'grid_freq')" "!=60"

assert "$(body /api/config/backup | wc -l | xargs)" "84"

echo "===================================================================="
echo "SUCCESS: $0"
echo "===================================================================="
