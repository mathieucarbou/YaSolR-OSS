// SPDX-License-Identifier: GPL-3.0-or-later
/*
 * Copyright (C) 2023-2024 Mathieu Carbou and others
 */
#pragma once

#include <YaSolR.h>

#include <map>

#ifdef APP_MODEL_PRO
  #define PUSH_BUTTON_CARD_CB ()
#else
  #define LINE_CHART          BAR_CHART
  #define AREA_CHART          BAR_CHART
  #define ENERGY_CARD         GENERIC_CARD
  #define PUSH_BUTTON_CARD    BUTTON_CARD
  #define PUSH_BUTTON_CARD_CB (int32_t value)
#endif

namespace YaSolR {
  class WebsiteClass {
    public:
      void initLayout();
      void initCards();
      void updateCards();
      void updateCharts();

    private:
      // statistics
      Statistic _appName = Statistic(&dashboard, YASOLR_LBL_001);
      Statistic _appModel = Statistic(&dashboard, YASOLR_LBL_002);
      Statistic _appVersion = Statistic(&dashboard, YASOLR_LBL_003);
      Statistic _appManufacturer = Statistic(&dashboard, YASOLR_LBL_004);

      Statistic _deviceBootCount = Statistic(&dashboard, YASOLR_LBL_005);
      Statistic _deviceCores = Statistic(&dashboard, YASOLR_LBL_006);
      Statistic _deviceHeapTotal = Statistic(&dashboard, YASOLR_LBL_007);
      Statistic _deviceHeapUsage = Statistic(&dashboard, YASOLR_LBL_008);
      Statistic _deviceHeapUsed = Statistic(&dashboard, YASOLR_LBL_009);
      Statistic _deviceID = Statistic(&dashboard, YASOLR_LBL_010);
      Statistic _deviceModel = Statistic(&dashboard, YASOLR_LBL_011);
      Statistic _deviceRev = Statistic(&dashboard, YASOLR_LBL_012);

      Statistic _firmwareBuildHash = Statistic(&dashboard, YASOLR_LBL_013);
      Statistic _firmwareBuildTimestamp = Statistic(&dashboard, YASOLR_LBL_014);
      Statistic _firmwareFilename = Statistic(&dashboard, YASOLR_LBL_015);

      Statistic _gridEnergy = Statistic(&dashboard, YASOLR_LBL_016);
      Statistic _gridEnergyReturned = Statistic(&dashboard, YASOLR_LBL_017);
      Statistic _gridFrequency = Statistic(&dashboard, YASOLR_LBL_018);

      Statistic _networkHostname = Statistic(&dashboard, YASOLR_LBL_019);
      Statistic _networkInterface = Statistic(&dashboard, YASOLR_LBL_020);
      Statistic _networkAPIP = Statistic(&dashboard, YASOLR_LBL_021);
      Statistic _networkAPMAC = Statistic(&dashboard, YASOLR_LBL_022);
      Statistic _networkEthIP = Statistic(&dashboard, YASOLR_LBL_023);
      Statistic _networkEthMAC = Statistic(&dashboard, YASOLR_LBL_024);
      Statistic _networkWiFiIP = Statistic(&dashboard, YASOLR_LBL_025);
      Statistic _networkWiFiMAC = Statistic(&dashboard, YASOLR_LBL_026);
      Statistic _networkWiFiSSID = Statistic(&dashboard, YASOLR_LBL_027);
      Statistic _networkWiFiRSSI = Statistic(&dashboard, YASOLR_LBL_028);
      Statistic _networkWiFiSignal = Statistic(&dashboard, YASOLR_LBL_029);

      Statistic _output1RelaySwitchCount = Statistic(&dashboard, YASOLR_LBL_030);
      Statistic _output2RelaySwitchCount = Statistic(&dashboard, YASOLR_LBL_031);
      Statistic _relay1SwitchCount = Statistic(&dashboard, YASOLR_LBL_032);
      Statistic _relay2SwitchCount = Statistic(&dashboard, YASOLR_LBL_033);

      Statistic _udpMessageRateBuffer = Statistic(&dashboard, YASOLR_LBL_157);

      Statistic _time = Statistic(&dashboard, YASOLR_LBL_034);
      Statistic _uptime = Statistic(&dashboard, YASOLR_LBL_035);
#ifdef APP_MODEL_TRIAL
      Statistic _trialRemainingTime = Statistic(&dashboard, "Trial Remaining Time");
#endif

      // home
      Card _routerPower = Card(&dashboard, ENERGY_CARD, YASOLR_LBL_036, "W");
      Card _routerApparentPower = Card(&dashboard, ENERGY_CARD, YASOLR_LBL_037, "VA");
      Card _routerPowerFactor = Card(&dashboard, ENERGY_CARD, YASOLR_LBL_038);
      Card _routerTHDi = Card(&dashboard, ENERGY_CARD, YASOLR_LBL_039, "%");
      Card _routerVoltage = Card(&dashboard, ENERGY_CARD, YASOLR_LBL_040, "V");
      Card _routerCurrent = Card(&dashboard, ENERGY_CARD, YASOLR_LBL_041, "A");
      Card _routerResistance = Card(&dashboard, ENERGY_CARD, YASOLR_LBL_042, "Ω");
      Card _routerEnergy = Card(&dashboard, ENERGY_CARD, YASOLR_LBL_043, "kWh");
      Card _gridPower = Card(&dashboard, ENERGY_CARD, YASOLR_LBL_044, "W");
      Card _routerDS18State = Card(&dashboard, TEMPERATURE_CARD, YASOLR_LBL_045, "°C");

      int _historyX[YASOLR_GRAPH_POINTS] = {0};
      int _gridPowerHistoryY[YASOLR_GRAPH_POINTS] = {0};
      int _routedPowerHistoryY[YASOLR_GRAPH_POINTS] = {0};
      int _routerTHDiHistoryY[YASOLR_GRAPH_POINTS] = {0};
      Chart _gridPowerHistory = Chart(&dashboard, LINE_CHART, YASOLR_LBL_044 " (W)");
      Chart _routedPowerHistory = Chart(&dashboard, AREA_CHART, YASOLR_LBL_036 " (W)");
      Chart _routerTHDiHistory = Chart(&dashboard, BAR_CHART, YASOLR_LBL_039 " (%)");

#ifdef APP_MODEL_PRO
      // tabs icons:
      // https://en.wikipedia.org/wiki/List_of_Unicode_characters#Miscellaneous_Symbols
      // https://en.wikipedia.org/wiki/List_of_Unicode_characters#Dingbats

      Tab _output1Tab = Tab(&dashboard, "\u2600 " YASOLR_LBL_046);
      Card _output1State = Card(&dashboard, STATUS_CARD, YASOLR_LBL_047, DASH_STATUS_IDLE);
      Card _output1DS18State = Card(&dashboard, TEMPERATURE_CARD, YASOLR_LBL_048, "°C");
      Card _output1DimmerSlider = Card(&dashboard, SLIDER_CARD, YASOLR_LBL_049, "", 0, YASOLR_DIMMER_MAX_LEVEL, 1);
      Card _output1DimmerSliderRO = Card(&dashboard, PROGRESS_CARD, YASOLR_LBL_050, "", 0, YASOLR_DIMMER_MAX_LEVEL, 1);
      Card _output1Bypass = Card(&dashboard, BUTTON_CARD, YASOLR_LBL_051);
      Card _output1BypassRO = Card(&dashboard, STATUS_CARD, YASOLR_LBL_051);
      Card _output1Power = Card(&dashboard, ENERGY_CARD, YASOLR_LBL_052, "W");
      Card _output1ApparentPower = Card(&dashboard, ENERGY_CARD, YASOLR_LBL_053, "VA");
      Card _output1PowerFactor = Card(&dashboard, ENERGY_CARD, YASOLR_LBL_054);
      Card _output1THDi = Card(&dashboard, ENERGY_CARD, YASOLR_LBL_055, "%");
      Card _output1Voltage = Card(&dashboard, ENERGY_CARD, YASOLR_LBL_056, "V");
      Card _output1Current = Card(&dashboard, ENERGY_CARD, YASOLR_LBL_057, "A");
      Card _output1Resistance = Card(&dashboard, ENERGY_CARD, YASOLR_LBL_058, "Ω");
      Card _output1Energy = Card(&dashboard, ENERGY_CARD, YASOLR_LBL_059, "kWh");
      Card _output1DimmerAuto = Card(&dashboard, BUTTON_CARD, YASOLR_LBL_060);
      Card _output1DimmerRatio = Card(&dashboard, SLIDER_CARD, YASOLR_LBL_061, "%", 0, 100, 1);
      Card _output1DimmerDutyLimiter = Card(&dashboard, SLIDER_CARD, YASOLR_LBL_062, "", 0, YASOLR_DIMMER_MAX_LEVEL, 1);
      Card _output1DimmerTempLimiter = Card(&dashboard, TEXT_INPUT_CARD, YASOLR_LBL_063, "°C");
      Card _output1BypassAuto = Card(&dashboard, BUTTON_CARD, YASOLR_LBL_064);
      Card _output1AutoStartTemp = Card(&dashboard, TEXT_INPUT_CARD, YASOLR_LBL_065, "°C");
      Card _output1AutoStoptTemp = Card(&dashboard, TEXT_INPUT_CARD, YASOLR_LBL_066, "°C");
      Card _output1AutoStartTime = Card(&dashboard, TEXT_INPUT_CARD, YASOLR_LBL_067);
      Card _output1AutoStoptTime = Card(&dashboard, TEXT_INPUT_CARD, YASOLR_LBL_068);
      Card _output1AutoStartWDays = Card(&dashboard, WEEK_SELECTOR_CARD, YASOLR_LBL_069);

      Tab _output2Tab = Tab(&dashboard, "\u2600 " YASOLR_LBL_070);
      Card _output2State = Card(&dashboard, STATUS_CARD, YASOLR_LBL_047, DASH_STATUS_IDLE);
      Card _output2DS18State = Card(&dashboard, TEMPERATURE_CARD, YASOLR_LBL_048, "°C");
      Card _output2DimmerSlider = Card(&dashboard, SLIDER_CARD, YASOLR_LBL_049, "", 0, YASOLR_DIMMER_MAX_LEVEL, 1);
      Card _output2DimmerSliderRO = Card(&dashboard, PROGRESS_CARD, YASOLR_LBL_050, "", 0, YASOLR_DIMMER_MAX_LEVEL, 1);
      Card _output2Bypass = Card(&dashboard, BUTTON_CARD, YASOLR_LBL_051);
      Card _output2BypassRO = Card(&dashboard, STATUS_CARD, YASOLR_LBL_051);
      Card _output2Power = Card(&dashboard, ENERGY_CARD, YASOLR_LBL_052, "W");
      Card _output2ApparentPower = Card(&dashboard, ENERGY_CARD, YASOLR_LBL_053, "VA");
      Card _output2PowerFactor = Card(&dashboard, ENERGY_CARD, YASOLR_LBL_054);
      Card _output2THDi = Card(&dashboard, ENERGY_CARD, YASOLR_LBL_055, "%");
      Card _output2Voltage = Card(&dashboard, ENERGY_CARD, YASOLR_LBL_056, "V");
      Card _output2Current = Card(&dashboard, ENERGY_CARD, YASOLR_LBL_057, "A");
      Card _output2Resistance = Card(&dashboard, ENERGY_CARD, YASOLR_LBL_058, "Ω");
      Card _output2Energy = Card(&dashboard, ENERGY_CARD, YASOLR_LBL_059, "kWh");
      Card _output2DimmerAuto = Card(&dashboard, BUTTON_CARD, YASOLR_LBL_060);
      Card _output2DimmerRatio = Card(&dashboard, SLIDER_CARD, YASOLR_LBL_061, "%", 0, 100, 1);
      Card _output2DimmerDutyLimiter = Card(&dashboard, SLIDER_CARD, YASOLR_LBL_062, "", 0, YASOLR_DIMMER_MAX_LEVEL, 1);
      Card _output2DimmerTempLimiter = Card(&dashboard, TEXT_INPUT_CARD, YASOLR_LBL_063, "°C");
      Card _output2BypassAuto = Card(&dashboard, BUTTON_CARD, YASOLR_LBL_064);
      Card _output2AutoStartTemp = Card(&dashboard, TEXT_INPUT_CARD, YASOLR_LBL_065);
      Card _output2AutoStoptTemp = Card(&dashboard, TEXT_INPUT_CARD, YASOLR_LBL_066);
      Card _output2AutoStartTime = Card(&dashboard, TEXT_INPUT_CARD, YASOLR_LBL_067);
      Card _output2AutoStoptTime = Card(&dashboard, TEXT_INPUT_CARD, YASOLR_LBL_068);
      Card _output2AutoStartWDays = Card(&dashboard, WEEK_SELECTOR_CARD, YASOLR_LBL_069);

      Tab _relaysTab = Tab(&dashboard, "\u2600 " YASOLR_LBL_071);
      Card _relay1Load = Card(&dashboard, TEXT_INPUT_CARD, YASOLR_LBL_072);
      Card _relay1Switch = Card(&dashboard, BUTTON_CARD, YASOLR_LBL_073);
      Card _relay1SwitchRO = Card(&dashboard, STATUS_CARD, YASOLR_LBL_074);
      Card _relay2Load = Card(&dashboard, TEXT_INPUT_CARD, YASOLR_LBL_075);
      Card _relay2Switch = Card(&dashboard, BUTTON_CARD, YASOLR_LBL_076);
      Card _relay2SwitchRO = Card(&dashboard, STATUS_CARD, YASOLR_LBL_077);

      Tab _managementTab = Tab(&dashboard, "\u2764 " YASOLR_LBL_078);
      Card _configBackup = Card(&dashboard, LINK_CARD, YASOLR_LBL_079);
      Card _configRestore = Card(&dashboard, FILE_UPLOAD_CARD, YASOLR_LBL_080, ".txt");
      Card _otaLink = Card(&dashboard, LINK_CARD, YASOLR_LBL_081);
      Card _restart = Card(&dashboard, PUSH_BUTTON_CARD, YASOLR_LBL_082);
      Card _debugMode = Card(&dashboard, BUTTON_CARD, YASOLR_LBL_083);
      Card _consoleLink = Card(&dashboard, LINK_CARD, YASOLR_LBL_084);
      Card _energyReset = Card(&dashboard, PUSH_BUTTON_CARD, YASOLR_LBL_085);
      Card _reset = Card(&dashboard, PUSH_BUTTON_CARD, YASOLR_LBL_086);

      Tab _networkConfigTab = Tab(&dashboard, "\u2728 " YASOLR_LBL_087);
      Card _adminPwd = Card(&dashboard, PASSWORD_CARD, YASOLR_LBL_088);
      Card _ntpServer = Card(&dashboard, TEXT_INPUT_CARD, YASOLR_LBL_089);
      Card _ntpTimezone = Card(&dashboard, ASYNC_DROPDOWN_CARD, YASOLR_LBL_090);
      Card _ntpSync = Card(&dashboard, TIME_SYNC_CARD, YASOLR_LBL_091);
      Card _wifiSSID = Card(&dashboard, TEXT_INPUT_CARD, YASOLR_LBL_092);
      Card _wifiPwd = Card(&dashboard, PASSWORD_CARD, YASOLR_LBL_093);
      Card _apMode = Card(&dashboard, BUTTON_CARD, YASOLR_LBL_094);

      Tab _mqttConfigTab = Tab(&dashboard, "\u2728 " YASOLR_LBL_095);
      Card _mqttServer = Card(&dashboard, TEXT_INPUT_CARD, YASOLR_LBL_096);
      Card _mqttPort = Card(&dashboard, TEXT_INPUT_CARD, YASOLR_LBL_097);
      Card _mqttUser = Card(&dashboard, TEXT_INPUT_CARD, YASOLR_LBL_098);
      Card _mqttPwd = Card(&dashboard, PASSWORD_CARD, YASOLR_LBL_099);
      Card _mqttSecured = Card(&dashboard, BUTTON_CARD, YASOLR_LBL_100);
      Card _mqttServerCert = Card(&dashboard, FILE_UPLOAD_CARD, YASOLR_LBL_101, ".pem,crt,der");
      Card _mqttPublishInterval = Card(&dashboard, SLIDER_CARD, YASOLR_LBL_102, "s", 5, 30, 1);
      Card _mqttTopic = Card(&dashboard, TEXT_INPUT_CARD, YASOLR_LBL_103);
      Card _haDiscovery = Card(&dashboard, BUTTON_CARD, YASOLR_LBL_104);
      Card _haDiscoveryTopic = Card(&dashboard, TEXT_INPUT_CARD, YASOLR_LBL_105);
      Card _mqttGridVoltage = Card(&dashboard, TEXT_INPUT_CARD, YASOLR_LBL_106);
      Card _mqttGridPower = Card(&dashboard, TEXT_INPUT_CARD, YASOLR_LBL_107);

      Tab _pinConfigTab = Tab(&dashboard, "\u21C6 " YASOLR_LBL_108);
      Card _pinDisplayClock = Card(&dashboard, TEXT_INPUT_CARD, YASOLR_LBL_111);
      Card _pinDisplayData = Card(&dashboard, TEXT_INPUT_CARD, YASOLR_LBL_112);
      Card _pinJsyRX = Card(&dashboard, TEXT_INPUT_CARD, YASOLR_LBL_116);
      Card _pinJsyTX = Card(&dashboard, TEXT_INPUT_CARD, YASOLR_LBL_117);
      Card _pinLEDGreen = Card(&dashboard, TEXT_INPUT_CARD, YASOLR_LBL_118);
      Card _pinLEDRed = Card(&dashboard, TEXT_INPUT_CARD, YASOLR_LBL_119);
      Card _pinLEDYellow = Card(&dashboard, TEXT_INPUT_CARD, YASOLR_LBL_120);
      Card _pinDimmerO1 = Card(&dashboard, TEXT_INPUT_CARD, YASOLR_LBL_131);
      Card _pinDS18O1 = Card(&dashboard, TEXT_INPUT_CARD, YASOLR_LBL_132);
      Card _pinRelayO1 = Card(&dashboard, TEXT_INPUT_CARD, YASOLR_LBL_134);
      Card _pinDimmerO2 = Card(&dashboard, TEXT_INPUT_CARD, YASOLR_LBL_135);
      Card _pinDS18O2 = Card(&dashboard, TEXT_INPUT_CARD, YASOLR_LBL_136);
      Card _pinRelayO2 = Card(&dashboard, TEXT_INPUT_CARD, YASOLR_LBL_138);
      Card _pinPZEMRX = Card(&dashboard, TEXT_INPUT_CARD, YASOLR_LBL_121);
      Card _pinPZEMTX = Card(&dashboard, TEXT_INPUT_CARD, YASOLR_LBL_122);
      Card _pinRelay1 = Card(&dashboard, TEXT_INPUT_CARD, YASOLR_LBL_074);
      Card _pinRelay2 = Card(&dashboard, TEXT_INPUT_CARD, YASOLR_LBL_077);
      Card _pinDS18Router = Card(&dashboard, TEXT_INPUT_CARD, YASOLR_LBL_139);
      Card _pinZCD = Card(&dashboard, TEXT_INPUT_CARD, YASOLR_LBL_125);

      Tab _hardwareEnableTab = Tab(&dashboard, "\u2699 " YASOLR_LBL_126);
      Card _display = Card(&dashboard, BUTTON_CARD, YASOLR_LBL_127);
      Card _jsy = Card(&dashboard, BUTTON_CARD, YASOLR_LBL_128);
      Card _led = Card(&dashboard, BUTTON_CARD, YASOLR_LBL_129);
      Card _mqtt = Card(&dashboard, BUTTON_CARD, YASOLR_LBL_095);
      Card _output1Dimmer = Card(&dashboard, BUTTON_CARD, YASOLR_LBL_131);
      Card _output1DS18 = Card(&dashboard, BUTTON_CARD, YASOLR_LBL_132);
      Card _output1PZEM = Card(&dashboard, BUTTON_CARD, YASOLR_LBL_133);
      Card _output1Relay = Card(&dashboard, BUTTON_CARD, YASOLR_LBL_134);
      Card _output2Dimmer = Card(&dashboard, BUTTON_CARD, YASOLR_LBL_135);
      Card _output2DS18 = Card(&dashboard, BUTTON_CARD, YASOLR_LBL_136);
      Card _output2PZEM = Card(&dashboard, BUTTON_CARD, YASOLR_LBL_137);
      Card _output2Relay = Card(&dashboard, BUTTON_CARD, YASOLR_LBL_138);
      Card _relay1 = Card(&dashboard, BUTTON_CARD, YASOLR_LBL_074);
      Card _relay2 = Card(&dashboard, BUTTON_CARD, YASOLR_LBL_077);
      Card _routerDS18 = Card(&dashboard, BUTTON_CARD, YASOLR_LBL_139);
      Card _zcd = Card(&dashboard, BUTTON_CARD, YASOLR_LBL_125);

      Tab _hardwareConfigTab = Tab(&dashboard, "\u2699 " YASOLR_LBL_140);
      Card _gridFreq = Card(&dashboard, DROPDOWN_CARD, YASOLR_LBL_141);
      Card _displaySpeed = Card(&dashboard, SLIDER_CARD, YASOLR_LBL_142, "s", 1, 10, 1);
      Card _displayType = Card(&dashboard, DROPDOWN_CARD, YASOLR_LBL_143);
      Card _displayRotation = Card(&dashboard, DROPDOWN_CARD, YASOLR_LBL_144);
      Card _output1ResistanceInput = Card(&dashboard, TEXT_INPUT_CARD, YASOLR_LBL_145);
      Card _output2ResistanceInput = Card(&dashboard, TEXT_INPUT_CARD, YASOLR_LBL_146);
      Card _output1PZEMSync = Card(&dashboard, PUSH_BUTTON_CARD, YASOLR_LBL_147);
      Card _output2PZEMSync = Card(&dashboard, PUSH_BUTTON_CARD, YASOLR_LBL_148);
      Card _output1RelayType = Card(&dashboard, DROPDOWN_CARD, YASOLR_LBL_149);
      Card _output2RelayType = Card(&dashboard, DROPDOWN_CARD, YASOLR_LBL_150);
      Card _relay1Type = Card(&dashboard, DROPDOWN_CARD, YASOLR_LBL_151);
      Card _relay2Type = Card(&dashboard, DROPDOWN_CARD, YASOLR_LBL_152);
#endif

    private:
      void _boolConfig(Card& card, const char* key);
      void _daysConfig(Card& card, const char* key);
      void _numConfig(Card& card, const char* key);
      void _pinConfig(Card& card, const char* key);
      void _passwordConfig(Card& card, const char* key);
      void _sliderConfig(Card& card, const char* key);
      void _textConfig(Card& card, const char* key);

      void _outputDimmerSlider(Card& card, Mycila::RouterOutput& output);
      void _outputBypassSwitch(Card& card, Mycila::RouterOutput& output);
      void _relaySwitch(Card& card, Mycila::RouterRelay& relay);

      void _pinout(Card& card, int32_t pin, std::map<int32_t, Card*>& pinout);
      void _status(Card& card, const char* key, bool enabled, bool state = true, const char* err = "");
      void _temperature(Card& card, Mycila::DS18& sensor);
  };

  extern WebsiteClass Website;
} // namespace YaSolR
