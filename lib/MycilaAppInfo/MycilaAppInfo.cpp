// SPDX-License-Identifier: MIT
/*
 * Copyright (C) 2023-2024 Mathieu Carbou and others
 */
#include <MycilaAppInfo.h>

#include <Esp.h>

#ifndef APP_NAME
#define APP_NAME "YaSolR"
#endif

#ifndef APP_MANUFACTURER
#define APP_MANUFACTURER "Mathieu Carbou"
#endif

#ifdef APP_VERSION_PRO
#ifdef APP_VERSION_TRIAL
#define APP_MODEL "Trial"
#else
#define APP_MODEL "Pro"
#endif
#endif

#ifdef APP_VERSION_OSS
#define APP_MODEL "OSS"
#endif

#ifndef APP_VERSION
#define APP_VERSION ""
#endif

#ifndef BUILD_HASH
#define BUILD_HASH ""
#endif

#ifndef BUILD_BRANCH
#define BUILD_BRANCH ""
#endif

#ifndef BUILD_TIMESAMP
#define BUILD_TIMESAMP ""
#endif

#ifndef BUILD_NAME
#define BUILD_NAME ""
#endif

Mycila::AppInfoClass::AppInfoClass() : id(_getEspId()),
                                       name(APP_NAME),
                                       version(APP_VERSION),
                                       model(APP_MODEL),
                                       manufacturer(APP_MANUFACTURER),
                                       firmware(String(APP_NAME) + "-" + (version.indexOf("_") >= 0 ? version.substring(0, version.indexOf("_")) : version) + "-" + BUILD_NAME + ".bin"),
                                       buildBranch(BUILD_BRANCH),
                                       buildHash(BUILD_HASH),
                                       buildDate(strlen(BUILD_TIMESAMP) == 0 ? __DATE__ " " __TIME__ : BUILD_TIMESAMP),
                                       debug(firmware.indexOf("debug") >= 0),
                                       trial(firmware.indexOf("trial") >= 0) {}

void Mycila::AppInfoClass::toJson(const JsonObject& root) const {
  root["build_date"] = buildDate;
  root["build_hash"] = buildHash;
  root["debug"] = debug;
  root["firmware"] = firmware;
  root["id"] = id;
  root["manufacturer"] = manufacturer;
  root["model"] = model;
  root["name"] = name;
  root["trial"] = trial;
  root["version"] = version;
}

String Mycila::AppInfoClass::_getEspId() const {
  uint32_t chipId = 0;
  for (int i = 0; i < 17; i += 8) {
    chipId |= ((ESP.getEfuseMac() >> (40 - i)) & 0xff) << i;
  }
  String espId = String(chipId, HEX);
  espId.toUpperCase();
  return espId;
}

namespace Mycila {
  AppInfoClass AppInfo;
} // namespace Mycila
