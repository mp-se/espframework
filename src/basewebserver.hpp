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
#ifndef SRC_BASEWEBSERVER_HPP_
#define SRC_BASEWEBSERVER_HPP_

#include <memory>

#if defined(ESP32)
#include <freertos/FreeRTOS.h>
#endif

#if defined(ESP8266)
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#else
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <ESPmDNS.h>
#include <WiFi.h>
#endif

#include <ArduinoJson.h>
#include <AsyncJson.h>

#include <espframework.hpp>
#include <interface.hpp>
#include <log.hpp>

#if defined(ESP8266)
#include <incbin.hpp>
INCBIN_EXTERN(IndexHtml);
INCBIN_EXTERN(AppJs);
INCBIN_EXTERN(AppCss);
INCBIN_EXTERN(FaviconIco);
#else
extern const uint8_t indexHtmlStart[] asm("_binary_html_index_html_start");
extern const uint8_t indexHtmlEnd[] asm("_binary_html_index_html_end");
extern const uint8_t appJsStart[] asm("_binary_html_app_js_gz_start");
extern const uint8_t appJsEnd[] asm("_binary_html_app_js_gz_end");
extern const uint8_t appCssStart[] asm("_binary_html_app_css_gz_start");
extern const uint8_t appCssEnd[] asm("_binary_html_app_css_gz_end");
extern const uint8_t faviconIcoStart[] asm("_binary_html_favicon_ico_gz_start");
extern const uint8_t faviconIcoEnd[] asm("_binary_html_favicon_ico_gz_end");
#endif

class BaseWebServer {
 protected:
  std::unique_ptr<AsyncWebServer> _server;
  WebConfigInterface *_webConfig;

  bool _wifiSetup = false;
  uint32_t _wifiPortalTimer = 0;

  File _uploadFile;
  int _uploadReturn = 200;
  uint32_t _uploadedSize = 0;

  String _wifiScanData;
  volatile bool _wifiScanTask = false;

  volatile bool _rebootTask = false;
  uint32_t _rebootTimer = 0;

  void resetWifiPortalTimer() { _wifiPortalTimer = millis(); }
  bool isAuthenticated(AsyncWebServerRequest *request);

  void webReturnOK(AsyncWebServerRequest *request) const {
    request->send(_uploadReturn);
  }
#if defined(ESP8266)
  void webReturnIndexHtml(AsyncWebServerRequest *request) const {
    Log.notice(F("WEB : webServer callback for /index.html (Memory)." CR));
    request->send(200, "text/html", (const uint8_t *)gIndexHtmlData,
                  gIndexHtmlSize);
  }
  void webReturnAppJs(AsyncWebServerRequest *request) const {
    if (LittleFS.exists("/app.js.gz")) {
      Log.notice(F("WEB : webServer callback for /app.js (FileSystem)." CR));
      AsyncWebServerResponse *response = request->beginResponse(
          LittleFS, "/app.js.gz", "application/javascript");
      response->addHeader("Content-Encoding", "gzip");
      request->send(response);
    } else {
      Log.notice(F("WEB : webServer callback for /app.js (Memory)." CR));
      AsyncWebServerResponse *response =
          request->beginResponse(200, "application/javascript",
                                 (const uint8_t *)gAppJsData, gAppJsSize);
      response->addHeader("Content-Encoding", "gzip");
      request->send(response);
    }
  }
  void webReturnAppCss(AsyncWebServerRequest *request) const {
    Log.notice(F("WEB : webServer callback for /app.css (Memory)." CR));
    AsyncWebServerResponse *response = request->beginResponse(
        200, "text/css", (const uint8_t *)gAppCssData, gAppCssSize);
    response->addHeader("Content-Encoding", "gzip");
    request->send(response);
  }
  void webReturnFavicon(AsyncWebServerRequest *request) const {
    Log.notice(F("WEB : webServer callback for /favicon.ico (Memory)." CR));
    AsyncWebServerResponse *response = request->beginResponse(
        200, "image/x-icon", (const uint8_t *)gFaviconIcoData, gFaviconIcoSize);
    response->addHeader("Content-Encoding", "gzip");
    request->send(response);
  }
#else
  void webReturnIndexHtml(AsyncWebServerRequest *request) const {
    Log.notice(F("WEB : webServer callback for /index.html (Memory)." CR));
    request->send(200, "text/html", (const uint8_t *)indexHtmlStart,
                  reinterpret_cast<uint32_t>(&indexHtmlEnd[0]) -
                      reinterpret_cast<uint32_t>(&indexHtmlStart[0]));
  }
  void webReturnAppJs(AsyncWebServerRequest *request) const {
    if (LittleFS.exists("/app.js.gz")) {
      Log.notice(F("WEB : webServer callback for /app.js (FileSystem)." CR));
      AsyncWebServerResponse *response = request->beginResponse(
          LittleFS, "/app.js.gz", "application/javascript");
      response->addHeader("Content-Encoding", "gzip");
      request->send(response);
    } else {
      Log.notice(F("WEB : webServer callback for /app.js (Memory)." CR));
      AsyncWebServerResponse *response = request->beginResponse(
          200, "application/javascript", (const uint8_t *)appJsStart,
          reinterpret_cast<int32_t>(&appJsEnd[0]) -
              reinterpret_cast<int32_t>(&appJsStart[0]));
      response->addHeader("Content-Encoding", "gzip");
      request->send(response);
    }
  }
  void webReturnAppCss(AsyncWebServerRequest *request) const {
    Log.notice(F("WEB : webServer callback for /app.css (Memory)." CR));
    AsyncWebServerResponse *response =
        request->beginResponse(200, "text/css", (const uint8_t *)appCssStart,
                               reinterpret_cast<int32_t>(&appCssEnd[0]) -
                                   reinterpret_cast<int32_t>(&appCssStart[0]));
    response->addHeader("Content-Encoding", "gzip");
    request->send(response);
  }
  void webReturnFavicon(AsyncWebServerRequest *request) const {
    Log.notice(F("WEB : webServer callback for /favicon.ico (Memory)." CR));
    AsyncWebServerResponse *response = request->beginResponse(
        200, "image/x-icon", (const uint8_t *)faviconIcoStart,
        reinterpret_cast<uint32_t>(&faviconIcoEnd[0]) -
            reinterpret_cast<uint32_t>(&faviconIcoStart[0]));
    response->addHeader("Content-Encoding", "gzip");
    request->send(response);
  }
#endif

  void webHandleAuth(AsyncWebServerRequest *request) const;
  void webHandleWifiScan(AsyncWebServerRequest *request);
  void webHandleWifiScanStatus(AsyncWebServerRequest *request);
  void webHandleWifiClear(AsyncWebServerRequest *request);
  void webHandlePageNotFound(AsyncWebServerRequest *request) const;
  void webHandleUploadFirmware(AsyncWebServerRequest *request, String filename,
                               size_t index, uint8_t *data, size_t len,
                               bool final);
  void webHandleUploadFile(AsyncWebServerRequest *request, String filename,
                           size_t index, uint8_t *data, size_t len, bool final);
  void webHandleRestart(AsyncWebServerRequest *request);
  void webHandlePing(AsyncWebServerRequest *request) const;
  void webHandleFileSystem(AsyncWebServerRequest *request, JsonVariant &json);

  virtual void setupWebHandlers();

 public:
  explicit BaseWebServer(WebConfigInterface *config);

  virtual bool setupWebServer();
  virtual AsyncWebServer *getWebServer() const { return _server.get(); }
  virtual void setWifiSetup(bool wifiSetup) { _wifiSetup = wifiSetup; }
  virtual void loop();
};

#endif  // SRC_BASEWEBSERVER_HPP_

// EOF
