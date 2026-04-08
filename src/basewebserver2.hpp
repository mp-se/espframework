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
#ifndef SRC_BASEWEBSERVER2_HPP_
#define SRC_BASEWEBSERVER2_HPP_

#ifndef ESPFWK_DISABLE_WEBSERVER

#ifdef ESPFWK_PSYCHIC_HTTP

#include <ArduinoJson.h>
#include <ESPmDNS.h>
#include <LittleFS.h>
#include <PsychicHttp.h>
#include <PsychicHttpsServer.h>
#include <WiFi.h>
#include <freertos/FreeRTOS.h>

#include <espframework.hpp>
#include <interface.hpp>
#include <log.hpp>
#include <memory>
#include <serialws2.hpp>

extern const uint8_t indexHtmlStart[] asm("_binary_html_index_html_start");
extern const uint8_t indexHtmlEnd[] asm("_binary_html_index_html_end");
extern const uint8_t appJsStart[] asm("_binary_html_app_js_gz_start");
extern const uint8_t appJsEnd[] asm("_binary_html_app_js_gz_end");
extern const uint8_t appCssStart[] asm("_binary_html_app_css_gz_start");
extern const uint8_t appCssEnd[] asm("_binary_html_app_css_gz_end");
extern const uint8_t faviconIcoStart[] asm("_binary_html_favicon_ico_gz_start");
extern const uint8_t faviconIcoEnd[] asm("_binary_html_favicon_ico_gz_end");

class BaseWebServer {
 protected:
  std::unique_ptr<PsychicHttpServer> _server;
  std::unique_ptr<PsychicHttpServer> _redirectServer;
  String _sslCert;
  String _sslKey;
  File _uploadFile;
  File _tempFile;
  WebConfigInterface *_webConfig;
  String _wifiScanData;

  int _uploadReturn = 200;
  uint32_t _uploadedSize = 0;
  uint32_t _rebootTimer = 0;
  uint32_t _wifiPortalTimer = 0;

  bool _wifiSetup = false;
  volatile bool _wifiScanTask = false;
  volatile bool _rebootTask = false;
  String _authToken;

  void resetWifiPortalTimer() { _wifiPortalTimer = millis(); }
  bool isAuthenticated(PsychicRequest *request);

  esp_err_t webReturnIndexHtml(PsychicRequest *request) {
    Log.notice(F("WEB : webServer callback for /index.html (Memory)." CR));
    PsychicResponse response(request);

    response.setContentType("text/html");
    response.setCode(200);
    response.setContent(reinterpret_cast<const uint8_t *>(indexHtmlStart),
                        reinterpret_cast<size_t>(&indexHtmlEnd[0]) -
                            reinterpret_cast<size_t>(&indexHtmlStart[0]));
    response.send();
    return ESP_OK;
  }
  esp_err_t webReturnAppJs(PsychicRequest *request) {
    PsychicResponse response(request);

    Log.notice(F("WEB : webServer callback for /app.js (Memory)." CR));
    response.setContentType("application/javascript");
    response.addHeader("content-encoding", "gzip");
    response.setCode(200);
    response.setContent(reinterpret_cast<const uint8_t *>(appJsStart),
                        reinterpret_cast<size_t>(&appJsEnd[0]) -
                            reinterpret_cast<size_t>(&appJsStart[0]));
    response.send();
    return ESP_OK;
  }
  esp_err_t webReturnAppCss(PsychicRequest *request) {
    PsychicResponse response(request);

    Log.notice(F("WEB : webServer callback for /app.css (Memory)." CR));
    response.setContentType("text/css");
    response.addHeader("content-encoding", "gzip");
    response.setCode(200);
    response.setContent(reinterpret_cast<const uint8_t *>(appCssStart),
                        reinterpret_cast<size_t>(&appCssEnd[0]) -
                            reinterpret_cast<size_t>(&appCssStart[0]));
    response.send();
    return ESP_OK;
  }
  esp_err_t webReturnFavicon(PsychicRequest *request) {
    PsychicResponse response(request);

    Log.notice(F("WEB : webServer callback for /favicon.ico (Memory)." CR));
    response.setContentType("image/x-icon");
    response.addHeader("content-encoding", "gzip");
    response.setCode(200);
    response.setContent(reinterpret_cast<const uint8_t *>(faviconIcoStart),
                        reinterpret_cast<size_t>(&faviconIcoEnd[0]) -
                            reinterpret_cast<size_t>(&faviconIcoStart[0]));
    response.send();
    return ESP_OK;
  }

  esp_err_t webHandleAuth(PsychicRequest *request);
  esp_err_t webHandleWifiScan(PsychicRequest *request);
  esp_err_t webHandleWifiScanStatus(PsychicRequest *request);
  esp_err_t webHandleWifiClear(PsychicRequest *request);
  esp_err_t webHandlePageNotFound(PsychicRequest *request);
  esp_err_t webHandleUploadFirmware(PsychicRequest *request, String filename,
                                    uint64_t index, uint8_t *data, size_t len,
                                    bool final);
  esp_err_t webHandleUploadFile(PsychicRequest *request, String filename,
                                size_t index, uint8_t *data, size_t len,
                                bool final);
  esp_err_t webHandleRestart(PsychicRequest *request);
  esp_err_t webHandlePing(PsychicRequest *request);
  esp_err_t webHandleFileSystem(PsychicRequest *request,
                                PsychicResponse *response, JsonVariant &json);

  virtual void setupWebHandlers();
  virtual PsychicHttpServer *getWebServer() { return _server.get(); }

 public:
  explicit BaseWebServer(WebConfigInterface *config);

  virtual bool setupWebServer(bool skipSSL = false,
                              SerialWebSocket *serialWs = nullptr,
                              Print *secondary = nullptr);
  virtual void setWifiSetup(bool wifiSetup) { _wifiSetup = wifiSetup; }
  virtual void loop();

  bool isSslEnabled() {
#if defined(ESPFWK_PSYCHIC_SSL)
    return _sslCert.length() && _sslKey.length();
#else
    return false;
#endif
  }
};

#endif  // ESPFWK_PSYCHIC_HTTP

#endif  // !ESPFWK_DISABLE_WEBSERVER

#endif  // SRC_BASEWEBSERVER2_HPP_

// EOF
