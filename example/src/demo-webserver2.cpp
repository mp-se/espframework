/*
MIT License

Copyright (c) 2025 Magnus

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
#if defined(ESPFWK_PSYCHIC_HTTP)

#include <LittleFS.h>

#include <demo-config.hpp>
#include <demo-push.hpp>
#include <demo-webserver2.hpp>
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
      (PsychicHttpRequestCallback)std::bind(&DemoWebServer::webHandleStatus,
                                            this, std::placeholders::_1, std::placeholders::_2));
  _server->on(
      "/api/config", HTTP_GET,
      (PsychicHttpRequestCallback)std::bind(&DemoWebServer::webHandleConfigRead,
                                            this, std::placeholders::_1, std::placeholders::_2));
  _server->on("/api/config", HTTP_POST,
              (PsychicJsonRequestCallback)std::bind(
                  &DemoWebServer::webHandleConfigWrite, this,
                  std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
}

esp_err_t DemoWebServer::webHandleConfigRead(PsychicRequest *request, PsychicResponse *response) {
  if (!isAuthenticated(request)) {
    return ESP_FAIL;
  }

  Log.notice(F("WEB : webServer callback for /api/config(read)." CR));
  
  JsonDocument doc;
  JsonObject obj = doc.to<JsonObject>();
  _webConfig->createJson(obj);
  
  response->addHeader("Content-Type", "application/json");
  String jsonStr;
  serializeJson(doc, jsonStr);
  return response->send(200, "application/json", jsonStr.c_str());
}

esp_err_t DemoWebServer::webHandleConfigWrite(PsychicRequest *request, PsychicResponse *response,
                                              JsonVariant &json) {
  if (!isAuthenticated(request)) {
    return ESP_FAIL;
  }

  Log.notice(F("WEB : webServer callback for /api/config(write)." CR));
  JsonObject obj = json.as<JsonObject>();

  _webConfig->parseJson(obj);
  _webConfig->saveFile();

  JsonDocument doc;
  doc[PARAM_SUCCESS] = true;
  doc[PARAM_MESSAGE] = "Configuration updated";
  
  response->addHeader("Content-Type", "application/json");
  String jsonStr;
  serializeJson(doc, jsonStr);
  return response->send(200, "application/json", jsonStr.c_str());
}

esp_err_t DemoWebServer::webHandleStatus(PsychicRequest *request, PsychicResponse *response) {
  Log.notice(F("WEB : webServer callback for /api/status." CR));
  JsonDocument doc;

  doc[PARAM_ID] = _webConfig->getID();
  doc[PARAM_MDNS] = _webConfig->getMDNS();
#if defined(ESP32C3)
  doc[PARAM_PLATFORM] = "esp32c3";
#elif defined(ESP32S2)
  doc[PARAM_PLATFORM] = "esp32s2";
#elif defined(ESP32S3)
  doc[PARAM_PLATFORM] = "esp32s3";
#else  // esp32 mini
  doc[PARAM_PLATFORM] = "esp32";
#endif
  doc[PARAM_RSSI] = WiFi.RSSI();
  doc[PARAM_SSID] = WiFi.SSID();
  doc[PARAM_APP_VER] = CFG_APPVER;
  doc[PARAM_APP_BUILD] = CFG_GITREV;
  doc[PARAM_TOTAL_HEAP] = ESP.getHeapSize();
  doc[PARAM_FREE_HEAP] = ESP.getFreeHeap();
  doc[PARAM_IP] = WiFi.localIP().toString();
  doc[PARAM_WIFI_SETUP] = _wifiSetup;
  
  response->addHeader("Content-Type", "application/json");
  String jsonStr;
  serializeJson(doc, jsonStr);
  return response->send(200, "application/json", jsonStr.c_str());
}

#endif  // ESPFWK_PSYCHIC_HTTP

// EOF
