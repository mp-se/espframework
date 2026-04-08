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
#ifndef SRC_BASEPUSH_HPP_
#define SRC_BASEPUSH_HPP_

#ifndef ESPFWK_DISABLE_WIFI

#include <memory>

#if defined(ESP8266)
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecure.h>
#else
#include <HTTPClient.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiClientSecure.h>
#endif
#include <interface.hpp>

class BasePush {
 protected:
  std::unique_ptr<WiFiClient> _wifi;
  std::unique_ptr<WiFiClientSecure> _wifiSecure;
  std::unique_ptr<HTTPClient> _http;
  int _lastResponseCode = 0;
  bool _lastSuccess = false;
  PushConfigInterface* _config;

  void probeMFLN(String serverPath);
  void addHttpHeader(String header);
  bool isSecure(String target) { return target.startsWith("https://"); }

  void allocateSecure() {
    if (_wifiSecure == nullptr) _wifiSecure.reset(new WiFiClientSecure());
  }

  String sendTcp(String& payload, const char* target);

 public:
  explicit BasePush(PushConfigInterface* config) {
    _config = config;
    _wifi.reset(new WiFiClient());
    _http.reset(new HTTPClient());
  }

  int getLastResponseCode() const { return _lastResponseCode; }
  bool wasLastSuccessful() const { return _lastSuccess; }

  String sendHttpPost(String& payload, const char* target, const char* header1,
                      const char* header2, bool sendTcpFlag = false);
  String sendHttpPost2(String& payload, const char* target, const char* header1,
                       const char* header2);
  String sendHttpGet(String& payload, const char* target, const char* header1,
                     const char* header2);
  void sendInfluxDb2(String& payload, const char* target, const char* org,
                     const char* bucket, const char* token);
  void sendMqtt(String& payload, const char* target, int port, const char* user,
                const char* pass);

  bool sendHttpPost(String& payload);
  bool sendHttpPost2(String& payload);
  bool sendHttpGet(String& payload);
  bool sendInfluxDb2(String& payload);
  bool sendMqtt(String& payload);
};

#endif  // !ESPFWK_DISABLE_WIFI

#endif  // SRC_BASEPUSH_HPP_

// EOF
