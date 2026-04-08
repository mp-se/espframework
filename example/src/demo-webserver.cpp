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
#if !defined(ESPFWK_PSYCHIC_HTTP)

#include <LittleFS.h>

#include <demo-config.hpp>
#include <demo-push.hpp>
#include <demo-webserver.hpp>
#include <espframework.hpp>
#include <log.hpp>

// These are parameters that the example ui app uses. Part of the status
// response.
constexpr auto PARAM_PLATFORM = "platform";
constexpr auto PARAM_TOTAL_HEAP = "total_heap";
constexpr auto PARAM_FREE_HEAP = "free_heap";
constexpr auto PARAM_IP = "ip";
constexpr auto PARAM_WIFI_SETUP = "wifi_setup";
constexpr auto PARAM_APP_VER = "app_ver";
constexpr auto PARAM_APP_BUILD = "app_build";

DemoWebServer::DemoWebServer(WebConfigInterface *config, DemoPush *push)
    : BaseWebServer(config) {
  _push = push;
}

void DemoWebServer::setupWebHandlers() {
  Log.notice(F("WEB : Setting up web handlers." CR));
  BaseWebServer::setupWebHandlers();

  MDNS.addService("espfwk", "tcp", 80);

  _server->on(
      "/api/status", HTTP_GET,
      std::bind(&DemoWebServer::webHandleStatus, this, std::placeholders::_1));

  AsyncCallbackJsonWebHandler *handler;
  _server->on("/api/config", HTTP_GET,
              std::bind(&DemoWebServer::webHandleConfigRead, this,
                        std::placeholders::_1));
  handler = new AsyncCallbackJsonWebHandler(
      "/api/config", std::bind(&DemoWebServer::webHandleConfigWrite, this,
                               std::placeholders::_1, std::placeholders::_2));
  _server->addHandler(handler);
}

void DemoWebServer::webHandleConfigRead(AsyncWebServerRequest *request) {
  if (!isAuthenticated(request)) {
    return;
  }

  Log.notice(F("WEB : webServer callback for /api/config(read)." CR));
  AsyncJsonResponse *response = new AsyncJsonResponse(false);
  JsonObject obj = response->getRoot().as<JsonObject>();
  _webConfig->createJson(obj);
  response->setLength();
  request->send(response);
}

void DemoWebServer::webHandleConfigWrite(AsyncWebServerRequest *request,
                                         JsonVariant &json) {
  if (!isAuthenticated(request)) {
    return;
  }

  Log.notice(F("WEB : webServer callback for /api/config(write)." CR));
  JsonObject obj = json.as<JsonObject>();
  _webConfig->parseJson(obj);
  obj.clear();
  _webConfig->saveFile();

  AsyncJsonResponse *response = new AsyncJsonResponse(false);
  obj = response->getRoot().as<JsonObject>();
  obj[PARAM_SUCCESS] = true;
  obj[PARAM_MESSAGE] = "Configuration updated";
  response->setLength();
  request->send(response);
}

void DemoWebServer::webHandleStatus(AsyncWebServerRequest *request) {
  Log.notice(F("WEB : webServer callback for /api/status." CR));
  AsyncJsonResponse *response = new AsyncJsonResponse(false);
  JsonObject obj = response->getRoot().as<JsonObject>();

  obj[PARAM_ID] = _webConfig->getID();
  obj[PARAM_MDNS] = _webConfig->getMDNS();
#if defined(ESP8266)
  obj[PARAM_PLATFORM] = "esp8266";
#elif defined(ESP32C3)
  obj[PARAM_PLATFORM] = "esp32c3";
#elif defined(ESP32S2)
  obj[PARAM_PLATFORM] = "esp32s2";
#elif defined(ESP32S3)
  obj[PARAM_PLATFORM] = "esp32s3";
#else  // esp32 mini
  obj[PARAM_PLATFORM] = "esp32";
#endif
  obj[PARAM_RSSI] = WiFi.RSSI();
  obj[PARAM_SSID] = WiFi.SSID();
  obj[PARAM_APP_VER] = CFG_APPVER;
  obj[PARAM_APP_BUILD] = CFG_GITREV;
#if defined(ESP8266)
  obj[PARAM_TOTAL_HEAP] = 81920;
  obj[PARAM_FREE_HEAP] = ESP.getFreeHeap();
  obj[PARAM_IP] = WiFi.localIP().toString();
#else
  obj[PARAM_TOTAL_HEAP] = ESP.getHeapSize();
  obj[PARAM_FREE_HEAP] = ESP.getFreeHeap();
  obj[PARAM_IP] = WiFi.localIP().toString();
#endif
  obj[PARAM_WIFI_SETUP] = _wifiSetup;
  response->setLength();
  request->send(response);
}

#endif  // !ESPFWK_PSYCHIC_HTTP

// EOF
