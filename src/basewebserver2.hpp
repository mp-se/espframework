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
#ifndef SRC_BASEWEBSERVER2_HPP_
#define SRC_BASEWEBSERVER2_HPP_

#ifndef ESPFWK_DISABLE_WEBSERVER

#ifdef ESPFWK_PSYCHIC_HTTP

#include <ArduinoJson.h>
#include <ESPmDNS.h>
#include <PsychicHttp.h>
#include <PsychicHttpsServer.h>
#include <WiFi.h>
#include <freertos/FreeRTOS.h>

#include <espframework.hpp>
#include <interface.hpp>
#include <log.hpp>
#include <memory>

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
#if defined(ESPFWK_PSYCHIC_SSL)
  std::unique_ptr<PsychicHttpsServer> _server;
  std::unique_ptr<PsychicHttpServer> _redirectServer;
  String _sslCert;
  String _sslKey;
#else
  std::unique_ptr<PsychicHttpServer> _server;
#endif
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

  esp_err_t webHandleAuth(PsychicRequest *request, PsychicResponse *response);
  esp_err_t webHandleWifiScan(PsychicRequest *request, PsychicResponse *response);
  esp_err_t webHandleWifiScanStatus(PsychicRequest *request, PsychicResponse *response);
  esp_err_t webHandleWifiClear(PsychicRequest *request);
  esp_err_t webHandlePageNotFound(PsychicRequest *request);
  esp_err_t webHandleUploadFirmware(PsychicRequest *request, String filename,
                                    uint64_t index, uint8_t *data, size_t len,
                                    bool final);
  esp_err_t webHandleUploadFile(PsychicRequest *request, String filename,
                                size_t index, uint8_t *data, size_t len,
                                bool final);
  esp_err_t webHandleRestart(PsychicRequest *request, PsychicResponse *response);
  esp_err_t webHandlePing(PsychicRequest *request, PsychicResponse *response);
  esp_err_t webHandleFileSystem(PsychicRequest *request, PsychicResponse *response, JsonVariant &json);

  virtual void setupWebHandlers();

 public:
  explicit BaseWebServer(WebConfigInterface *config);

  virtual bool setupWebServer(bool skipSSL = false);
  virtual PsychicHttpServer *getWebServer() { return _server.get(); }
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
