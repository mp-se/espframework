/*
MIT License

Copyright (c) 2021-22 Magnus

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
 */
#include <LittleFS.h>

#include <baseconfig.hpp>
#include <espframework.hpp>

BaseConfig::BaseConfig(String baseMDNS, String fileName) {
  char buf[30];
  snprintf(buf, sizeof(buf), "%6x", (unsigned int)ESP.getChipId());
  _id = String(buf);
  snprintf(buf, sizeof(buf), "%s%s", baseMDNS.c_str(), _id.c_str());
  _mDNS = String(buf);
  _fileName = fileName;
  _saveNeeded = false;
#if LOG_LEVEL == 6
  Log.verbose(F("CFG : Creating hostname=%s, id=%s." CR), _mDNS.c_str(),
              _id.c_str());
#endif
}

void BaseConfig::createJsonWifi(DynamicJsonDocument& doc, bool skipSecrets) {
#if LOG_LEVEL == 6
  Log.verbose(F("CFG : Create json (wifi)." CR));
#endif
  doc[PARAM_MDNS] = getMDNS();
  doc[PARAM_SSID] = getWifiSSID(0);
  doc[PARAM_SSID2] = getWifiSSID(1);

  if (!skipSecrets) {
    doc[PARAM_PASS] = getWifiPass(0);
    doc[PARAM_PASS2] = getWifiPass(1);
  }
}

void BaseConfig::parseJsonWifi(DynamicJsonDocument& doc) {
#if LOG_LEVEL == 6
  Log.verbose(F("CFG : Parsing json (wifi)." CR));
#endif
  if (!doc[PARAM_MDNS].isNull()) setMDNS(doc[PARAM_MDNS]);

  if (!doc[PARAM_SSID].isNull()) setWifiSSID(doc[PARAM_SSID], 0);
  if (!doc[PARAM_PASS].isNull()) setWifiPass(doc[PARAM_PASS], 0);

  if (!doc[PARAM_SSID2].isNull()) setWifiSSID(doc[PARAM_SSID2], 1);
  if (!doc[PARAM_PASS2].isNull()) setWifiPass(doc[PARAM_PASS2], 1);

  _saveNeeded = true;
}

void BaseConfig::createJsonOta(DynamicJsonDocument& doc, bool skipSecrets) {
#if LOG_LEVEL == 6
  Log.verbose(F("CFG : Create json (ota)." CR));
#endif
  doc[PARAM_OTA_URL] = getOtaURL();
}

void BaseConfig::parseJsonOta(DynamicJsonDocument& doc) {
#if LOG_LEVEL == 6
  Log.verbose(F("CFG : Parsing json (ota)." CR));
#endif
  if (!doc[PARAM_OTA_URL].isNull()) setOtaURL(doc[PARAM_OTA_URL]);

  _saveNeeded = true;
}

void BaseConfig::createJsonPush(DynamicJsonDocument& doc, bool skipSecrets) {
#if LOG_LEVEL == 6
  Log.verbose(F("CFG : Create json (push)." CR));
#endif
  JsonArray headers;

  doc[PARAM_TARGET_HTTP_POST] = getTargetHttpPost();
  doc[PARAM_HEADER1_HTTP_POST] = getHeader1HttpPost();
  doc[PARAM_HEADER2_HTTP_POST] = getHeader2HttpPost();
  doc[PARAM_TARGET_HTTP_GET] = getTargetHttpGet();
  doc[PARAM_HEADER1_HTTP_GET] = getHeader1HttpGet();
  doc[PARAM_HEADER2_HTTP_GET] = getHeader2HttpGet();
  doc[PARAM_TARGET_INFLUXDB2] = getTargetInfluxDB2();
  doc[PARAM_ORG_INFLUXDB2] = getOrgInfluxDB2();
  doc[PARAM_BUCKET_INFLUXDB2] = getBucketInfluxDB2();
  doc[PARAM_TOKEN_INFLUXDB2] = getTokenInfluxDB2();
  doc[PARAM_TARGET_MQTT] = getTargetMqtt();
  doc[PARAM_PORT_MQTT] = getPortMqtt();
  doc[PARAM_USER_MQTT] = getUserMqtt();
  doc[PARAM_PASS_MQTT] = getPassMqtt();
}

void BaseConfig::parseJsonPush(DynamicJsonDocument& doc) {
#if LOG_LEVEL == 6
  Log.verbose(F("CFG : Parsing json (push)." CR));
#endif

  if (!doc[PARAM_TARGET_HTTP_POST].isNull()) {
    setTargetHttpPost(doc[PARAM_TARGET_HTTP_POST]);
  }
  if (!doc[PARAM_HEADER1_HTTP_POST].isNull()) {
    setHeader1HttpPost(doc[PARAM_HEADER1_HTTP_POST]);
  }
  if (!doc[PARAM_HEADER2_HTTP_POST].isNull()) {
    setHeader2HttpPost(doc[PARAM_HEADER2_HTTP_POST]);
  }
  if (!doc[PARAM_HEADER1_HTTP_GET].isNull()) {
    setHeader1HttpGet(doc[PARAM_HEADER1_HTTP_GET]);
  }
  if (!doc[PARAM_HEADER2_HTTP_GET].isNull()) {
    setHeader2HttpGet(doc[PARAM_HEADER2_HTTP_GET]);
  }
  if (!doc[PARAM_TARGET_HTTP_GET].isNull()) {
    setTargetHttpGet(doc[PARAM_TARGET_HTTP_GET]);
  }
  if (!doc[PARAM_TARGET_INFLUXDB2].isNull()) {
    setTargetInfluxDB2(doc[PARAM_TARGET_INFLUXDB2]);
  }
  if (!doc[PARAM_ORG_INFLUXDB2].isNull()) {
    setOrgInfluxDB2(doc[PARAM_ORG_INFLUXDB2]);
  }
  if (!doc[PARAM_BUCKET_INFLUXDB2].isNull()) {
    setBucketInfluxDB2(doc[PARAM_BUCKET_INFLUXDB2]);
  }
  if (!doc[PARAM_TOKEN_INFLUXDB2].isNull()) {
    setTokenInfluxDB2(doc[PARAM_TOKEN_INFLUXDB2]);
  }
  if (!doc[PARAM_TARGET_MQTT].isNull()) {
    setTargetMqtt(doc[PARAM_TARGET_MQTT]);
  }
  if (!doc[PARAM_PORT_MQTT].isNull()) {
    setPortMqtt(doc[PARAM_PORT_MQTT].as<int>());
  }
  if (!doc[PARAM_USER_MQTT].isNull()) {
    setUserMqtt(doc[PARAM_USER_MQTT]);
  }
  if (!doc[PARAM_PASS_MQTT].isNull()) {
    setPassMqtt(doc[PARAM_PASS_MQTT]);
  }

  _saveNeeded = true;
}

void BaseConfig::createJsonBase(DynamicJsonDocument& doc, bool skipSecrets) {
#if LOG_LEVEL == 6
  Log.verbose(F("CFG : Create json (base)." CR));
#endif
  doc[PARAM_ID] = getID();
  doc[PARAM_TEMP_FORMAT] = String(getTempFormat());
}

void BaseConfig::parseJsonBase(DynamicJsonDocument& doc) {
#if LOG_LEVEL == 6
  Log.verbose(F("CFG : Parsing json (base)." CR));
#endif

  // Dont parse ID since this is always set based on device ID.

  if (!doc[PARAM_TEMP_FORMAT].isNull()) {
    String s = doc[PARAM_TEMP_FORMAT];
    setTempFormat(s.charAt(0));
  }

  _saveNeeded = true;
}

bool BaseConfig::saveFile() {
  if (!_saveNeeded) {
#if LOG_LEVEL == 6
    Log.verbose(F("CFG : Skipping save, not needed." CR));
#endif
    return true;
  }

  File configFile = LittleFS.open(_fileName.c_str(), "w");

  if (!configFile) {
    Log.error(F("CFG : Failed to open file %s for save." CR),
              _fileName.c_str());
    return false;
  }

  DynamicJsonDocument doc(2048);
  createJson(doc, false);  // Include secrets
#if LOG_LEVEL == 6
  serializeJson(doc, Serial);
  Serial.print(CR);
#endif
  serializeJson(doc, configFile);
  configFile.flush();
  configFile.close();
  _saveNeeded = false;
  Log.notice(F("CFG : Configuration saved." CR));
  return true;
}

bool BaseConfig::loadFile() {
#if LOG_LEVEL == 6
  Log.verbose(F("CFG : Loading configuration from file." CR));
#endif

  if (!LittleFS.exists(_fileName.c_str())) {
    Log.error(F("CFG : Configuration file %s does not exist." CR),
              _fileName.c_str());
    return false;
  }

  File configFile = LittleFS.open(_fileName.c_str(), "r");

  if (!configFile) {
    Log.error(F("CFG : Failed to open %s." CR), _fileName.c_str());
    return false;
  }

  DynamicJsonDocument doc(2048);
  DeserializationError err = deserializeJson(doc, configFile);
#if LOG_LEVEL == 6
  serializeJson(doc, Serial);
  Serial.print(CR);
#endif
  configFile.close();

  if (err) {
    Log.error(F("CFG : Failed to parse file, Err: %s, %d." CR), err.c_str(),
              doc.capacity());
    return false;
  }

  parseJson(doc);
  Log.notice(F("CFG : Configuration file loaded." CR));
  return true;
}

void BaseConfig::formatFileSystem() {
#if LOG_LEVEL == 6
  Log.verbose(F("CFG : Formating filesystem." CR));
#endif
  LittleFS.format();
}

void BaseConfig::checkFileSystem() {
#if LOG_LEVEL == 6
  Log.verbose(F("CFG : Checking if filesystem is valid." CR));
#endif
  if (LittleFS.begin()) {
#if LOG_LEVEL == 6
    Log.verbose(F("CFG : Filesystem mounted." CR));
#endif
  } else {
    Log.error(F("CFG : Unable to mount file system, formatting..." CR));
    LittleFS.format();
  }
}

// EOF
