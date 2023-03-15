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
#ifndef SRC_BASEASYNCWEBHANDLER_HPP_
#define SRC_BASEASYNCWEBHANDLER_HPP_

#if defined(ESP8266)
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#else
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <ESPmDNS.h>
#include <FS.h>
#include <FreeRTOS.h>
#include <WiFi.h>
#define ESP8266WebServer WebServer
#endif

#include <interface.hpp>

#if defined(ESP8266)
#include <incbin.h>
INCBIN_EXTERN(IndexHtm);
INCBIN_EXTERN(AboutHtm);
INCBIN_EXTERN(ConfigHtm);
INCBIN_EXTERN(UploadHtm);
#else
extern const uint8_t indexHtmStart[] asm("_binary_html_index_min_htm_start");
extern const uint8_t indexHtmEnd[] asm("_binary_html_index_min_htm_end");
extern const uint8_t configHtmStart[] asm("_binary_html_config_min_htm_start");
extern const uint8_t configHtmEnd[] asm("_binary_html_config_min_htm_end");
extern const uint8_t aboutHtmStart[] asm("_binary_html_about_min_htm_start");
extern const uint8_t aboutHtmEnd[] asm("_binary_html_about_min_htm_end");
extern const uint8_t uploadHtmStart[] asm("_binary_html_upload_min_htm_start");
extern const uint8_t uploadHtmEnd[] asm("_binary_html_upload_min_htm_end");
#endif

class BaseAsyncWebHandler {
 protected:
  AsyncWebServer *_server = 0;
  File _uploadFile;
  int _uploadedSize = 0;
  WebConfig *_webConfig;
  int _uploadReturn = 200;
  int _dynamicJsonSize = 2000;
  uint32_t _rebootTimer = 0;
  bool _reboot = false;

  void webReturnOK(AsyncWebServerRequest *request) {
    request->send(_uploadReturn);
  }
#if defined(ESP8266)
  void webReturnIndexHtm(AsyncWebServerRequest *request) {
    request->send_P(200, "text/html", (const uint8_t *)gIndexHtmData,
                    gIndexHtmSize);
  }
  void webReturnConfigHtm(AsyncWebServerRequest *request) {
    request->send_P(200, "text/html", (const uint8_t *)gConfigHtmData,
                    gConfigHtmSize);
  }
  void webReturnAboutHtm(AsyncWebServerRequest *request) {
    request->send_P(200, "text/html", (const uint8_t *)gAboutHtmData,
                    gAboutHtmSize);
  }
  void webReturnUploadHtm(AsyncWebServerRequest *request) {
    request->send_P(200, "text/html", (const uint8_t *)gUploadHtmData,
                    gUploadHtmSize);
  }
#else
  void webReturnIndexHtm(AsyncWebServerRequest *request) {
    request->send_P(200, "text/html", (const uint8_t *)indexHtmStart,
                    strlen(reinterpret_cast<const char *>(&indexHtmStart[0])));
  }
  void webReturnConfigHtm(AsyncWebServerRequest *request) {
    request->send_P(200, "text/html", (const uint8_t *)configHtmStart,
                    strlen(reinterpret_cast<const char *>(&configHtmStart[0])));
  }
  void webReturnAboutHtm(AsyncWebServerRequest *request) {
    request->send_P(200, "text/html", (const uint8_t *)aboutHtmStart,
                    strlen(reinterpret_cast<const char *>(&aboutHtmStart[0])));
  }
  void webReturnUploadHtm(AsyncWebServerRequest *request) {
    request->send_P(200, "text/html", (const uint8_t *)uploadHtmStart,
                    strlen(reinterpret_cast<const char *>(&uploadHtmStart[0])));
  }
#endif
  void webHandlePageNotFound(AsyncWebServerRequest *request);
  void webHandleUploadFile(AsyncWebServerRequest *request, String filename,
                           size_t index, uint8_t *data, size_t len, bool final);
  void webHandleConfigRead(AsyncWebServerRequest *request);
  void webHandleConfigWrite(AsyncWebServerRequest *request);

  virtual void setupAsyncWebHandlers();

 public:
  explicit BaseAsyncWebHandler(WebConfig *config, int dynamicJsonSize = 2000);
  virtual bool setupAsyncWebServer();
  virtual AsyncWebServer *getWebServer() { return _server; }
  virtual void loop();
};

#endif  // SRC_BASEASYNCWEBHANDLER_HPP_

// EOF
