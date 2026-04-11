/*
 * ESPFramework
 * Copyright (c) 2025-2026 Magnus
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 */
#ifndef SRC_WIFICONNECTION_HPP_
#define SRC_WIFICONNECTION_HPP_

#if defined(ESP8266)
#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>
#else
#include <HTTPClient.h>
#include <WiFi.h>
#endif
#include <DNSServer.h>
#include <LittleFS.h>
#include <WiFiUdp.h>

#include <interface.hpp>

#if defined(ESP8266)
#define wifi_mode_t WiFiMode_t
#endif

#include <improvWiFi/ImprovWiFi.h>

class WifiConnection {
 private:
  String _apSSID;
  String _apPWD;
  String _apMDNS;
  String _userSSID;
  String _userPWD;
  WifiConfigInterface* _wifiConfig;
  DNSServer* _dnsServer = NULL;
  ImprovWiFi* _improvWiFi = NULL;
  bool _enableImprov = false;
  uint32_t _timer = 0;
  uint32_t _timeout = 3000;  // 3 seconds
  uint8_t _resetCounter = 0;
  uint8_t _initialResetCounter = 0;
  const uint8_t _minResetCount = 2;

  void connectAsync(String ssid, String pass, wifi_mode_t mode);
  bool waitForConnection(int maxTime);
  void readReset();
  void writeReset();
  const uint8_t* findStrongestAP(String& ssid);

  void improveSetWifiCredentials(const char* ssid, const char* password);
  void improveInfo(const char* info);
  void improveDebug(const char* debug);

 public:
  WifiConnection(WifiConfigInterface* cfg, String apSSID, String apPWD,
                 String apMDNS, String userSSID = "", String userPWD = "");
  void init();
  void timeSync(String timeZone = "");

  void enableImprov(bool f) { _enableImprov = f; }

  bool connect(bool wifiDirect, wifi_mode_t mode = WIFI_STA);
  bool connect(wifi_mode_t mode = WIFI_STA) { return connect(false, mode); }
  bool disconnect();
  bool isConnected();
  bool isDoubleResetDetected();
  void stopDoubleReset();
  void setAP(String apSSID, String apPWD) {
    _apSSID = apSSID;
    _apPWD = apPWD;
  }
  void startAP(wifi_mode_t mode = WIFI_AP);
  bool hasConfig();
  String getIPAddress();

  void loop();
};

#endif  // SRC_WIFICONNECTION_HPP_

// EOF
