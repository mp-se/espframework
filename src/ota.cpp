/*
  Copyright (c) 2021-2026 Magnus
  https://github.com/mp-se/espframework

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program. If not, see <https://www.gnu.org/licenses/>.
*/
#ifndef ESPFWK_DISABLE_WIFI

#ifndef ESPFWK_DISABLE_OTA

#if defined(ESP8266)
#include <ESP8266WiFi.h>
#include <ESP8266httpUpdate.h>
#else
#include <HTTPUpdate.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiClientSecure.h>
#define ESPhttpUpdate httpUpdate
#endif

#include <climits>
#include <cstdio>
#include <cstdlib>
#include <espframework.hpp>
#include <log.hpp>
#include <ota.hpp>

// File on the OTA server that contains the latest version information. Can be
// overridden at build time, e.g. -D ESPFWK_OTA_VERSION_FILE=\"version32c3.json\"
#if !defined(ESPFWK_OTA_VERSION_FILE)
#define ESPFWK_OTA_VERSION_FILE "version.json"
#endif

static bool isSafeRelativeFileName(const String& filename) {
  return filename.length() > 0 && !filename.startsWith("/") &&
         filename.indexOf("..") == -1 && filename.indexOf('\\') == -1;
}

OtaUpdate::OtaUpdate(OtaConfigInterface *cfg, String ver, String fileName) {
  _otaConfig = cfg;
  _curVer = ver;
  _fileName = fileName;

  if (_fileName.length() == 0) {
#if defined(ESP8266)
    _fileName = "firmware.bin";
#elif defined(ESP32S3)
    _fileName = "firmware32s3.bin";
#elif defined(ESP32S2)
    _fileName = "firmware32s2.bin";
#elif defined(ESP32C3)
    _fileName = "firmware32c3.bin";
#elif defined(ESP32)
    _fileName = "firmware32.bin";
#else
#warning \
    "OTA filename is not defined or can be extrapolated, OTA will not work in this build"
#endif
  }
}

bool OtaUpdate::updateFirmware() {
  if (!_newFirmware) {
    Log.notice(F("OTA : No newer version exist, skipping update." CR));
    return false;
  }
#if LOG_LEVEL == 6
  Log.verbose(F("OTA : Updating firmware." CR));
#endif

  if (_fileName.length() == 0) {
    Log.notice(F("OTA : Filename is not defined, skipping update." CR));
    return false;
  }

  WiFiClient wifi;
  WiFiClientSecure wifiSecure;
  HTTPUpdateResult ret;
  String serverPath = _otaConfig->getOtaURL();
  if (!serverPath.endsWith("/")) serverPath += "/";
  serverPath += _fileName;

  Log.notice(F("OTA : Performing ota with %s." CR), serverPath.c_str());

  if (serverPath.startsWith("https://")) {
    wifiSecure.setInsecure();
    Log.notice(F("OTA: OTA, SSL enabled without validation." CR));
    ret = ESPhttpUpdate.update(wifiSecure, serverPath);
  } else {
    ret = ESPhttpUpdate.update(wifi, serverPath);
  }

  switch (ret) {
    case HTTP_UPDATE_FAILED: {
      Log.error(F("OTA : OTA update failed %d" CR),
                ESPhttpUpdate.getLastError());
    } break;
    case HTTP_UPDATE_NO_UPDATES:
      break;
    case HTTP_UPDATE_OK: {
      Log.notice(F("OTA : OTA Update sucesfull, rebooting." CR));
      delay(100);
      ESP_RESET();
      break;
    }
  }

  return false;
}

bool OtaUpdate::downloadFile(const String& url, const String& filename) {
  WiFiClient wifi;
  WiFiClientSecure wifiSecure;
  HTTPClient http;

  if (url.startsWith("https://")) {
    wifiSecure.setInsecure();
    if (!http.begin(wifiSecure, url)) return false;
  } else if (!http.begin(wifi, url)) {
    return false;
  }

  int httpResponseCode = http.GET();

  if (httpResponseCode == 200) {
    File f = LittleFS.open(filename, "w");
    if (!f) {
      Log.error(F("OTA : Failed to open file %s for writing." CR),
                filename.c_str());
      http.end();
      return false;
    }
    int written = http.writeToStream(&f);
    f.close();
    http.end();
    if (written < 0) {
      LittleFS.remove(filename);
      Log.error(F("OTA : Failed downloading file %s." CR), filename.c_str());
      return false;
    }
    Log.notice(F("OTA : Downloaded file %s." CR), filename.c_str());
    return true;
  } else {
    Log.error(F("OTA : Failed to download file %s, response=%d" CR),
              filename.c_str(), httpResponseCode);
    http.end();
    return false;
  }
}

bool OtaUpdate::checkFirmwareVersion() {
  _newFirmware = false;
  if (!_otaConfig->isOtaActive()) {
    Log.verbose(F("OTA : No OTA url defined, skipping." CR));
    return false;
  }

#if LOG_LEVEL == 6
  Log.verbose(F("OTA : Checking if new version exist, current ver=%s." CR),
              _curVer.c_str());
#endif
  WiFiClient wifi;
  WiFiClientSecure wifiSecure;
  HTTPClient http;
  String serverPath = _otaConfig->getOtaURL();
  if (!serverPath.endsWith("/")) serverPath += "/";
  String versionPath = serverPath + ESPFWK_OTA_VERSION_FILE;

  // Your Domain name with URL path or IP address with path
  if (_otaConfig->isOtaSSL()) {
    wifiSecure.setInsecure();
    Log.notice(F("OTA : OTA, SSL enabled without validation." CR));
    if (!http.begin(wifiSecure, versionPath)) return false;
  } else {
    if (!http.begin(wifi, versionPath)) return false;
  }

  // Send HTTP GET request
  JsonDocument ver;
  int httpResponseCode = http.GET();

  if (httpResponseCode == 200) {
    Log.notice(F("OTA : Found %s, response=%d" CR), ESPFWK_OTA_VERSION_FILE,
               httpResponseCode);

    String payload = http.getString();
#if LOG_LEVEL == 6
    Log.verbose(F("OTA : Payload %s." CR), payload.c_str());
#endif
    DeserializationError err = deserializeJson(ver, payload);
    if (err) {
      Log.error(F("OTA : Failed to parse %s" CR), ESPFWK_OTA_VERSION_FILE);
    } else {
#if LOG_LEVEL == 6
      Log.verbose(F("OTA : Project %s version %s." CR),
                  (const char *)ver["project"], (const char *)ver["version"]);
#endif
      int newVer[3];
      int curVer[3];

      if (parseFirmwareVersionString(newVer, (const char *)ver["version"])) {
        if (parseFirmwareVersionString(curVer, _curVer.c_str())) {
#if LOG_LEVEL == 6
          Log.verbose(F("OTA : OTA checking new=%d.%d.%d cur=%d.%d.%d" CR),
                      newVer[0], newVer[1], newVer[2], curVer[0], curVer[1],
                      curVer[2]);
#endif
          // Compare major version
          if (newVer[0] > curVer[0]) _newFirmware = true;
          // Compare minor version
          else if (newVer[0] == curVer[0] && newVer[1] > curVer[1])
            _newFirmware = true;
          // Compare patch version
          else if (newVer[0] == curVer[0] && newVer[1] == curVer[1] &&
                   newVer[2] > curVer[2])
            _newFirmware = true;
        }
      }

      // Download new html files to filesystem if they are present.
      if (!ver["files"].isNull() && _newFirmware) {
        Log.notice(F("OTA : OTA checking if files should be downloaded." CR));
        JsonArray htmlFiles = ver["files"].as<JsonArray>();
        for (JsonVariant v : htmlFiles) {
          String s = v;
          if (!isSafeRelativeFileName(s)) {
            Log.error(F("OTA : Ignoring unsafe file path %s." CR), s.c_str());
            continue;
          }
#if LOG_LEVEL == 6
          Log.verbose(F("OTA : Dowloading file %s" CR), s.c_str());
#endif
          downloadFile(serverPath + s, "/" + s);
        }
      }
    }
  } else {
    Log.error(F("OTA : OTA error checking %s, response=%d" CR),
              ESPFWK_OTA_VERSION_FILE, httpResponseCode);
  }
  http.end();

#if LOG_LEVEL == 6
  Log.verbose(F("OTA : New version %s." CR),
              _newFirmware ? "found" : "NOT found");
#endif

  return _newFirmware;
}

bool OtaUpdate::parseFirmwareVersionString(int (&num)[3], const char *version) {
#if LOG_LEVEL == 6
  Log.verbose(F("OTA : Parsing version number string %s." CR), version);
#endif
  if (!version || !*version) return false;

  char temp[80];
  int written = snprintf(temp, sizeof(temp), "%s", version);
  if (written < 0 || written >= static_cast<int>(sizeof(temp))) return false;

  char *part = temp;
  char *save = nullptr;
  for (int i = 0; i < 3; ++i) {
    char *token = strtok_r(part, ".", &save);
    part = nullptr;
    if (!token || !*token) return false;

    char *end = nullptr;
    long value = strtol(token, &end, 10);
    if (*end != '\0' || value < 0 || value > INT_MAX) return false;
    num[i] = static_cast<int>(value);
  }

  return strtok_r(nullptr, ".", &save) == nullptr;
}

#endif  // ESPFWK_DISABLE_OTA

#endif  // ESPFWK_DISABLE_WIFI

// EOF
