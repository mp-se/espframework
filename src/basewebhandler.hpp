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
#ifndef SRC_BASEWEBHANDLER_HPP_
#define SRC_BASEWEBHANDLER_HPP_

#if defined(ESP8266)
#include <ESP8266WebServer.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#else
#include <ESPmDNS.h>
#include <WebServer.h>
#include <WiFi.h>
#define ESP8266WebServer WebServer
#include <FS.h>
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

class BaseWebHandler {
 protected:
  ESP8266WebServer* _server = 0;
  File _uploadFile;
  WebConfig* _webConfig;
  int _uploadReturn = 200;
  int _dynamicJsonSize = 2000;

  void webReturnOK() { _server->send(_uploadReturn); }
#if defined(ESP8266)
  void webReturnIndexHtm() {
    _server->send_P(200, "text/html", (const char*)gIndexHtmData,
                    gIndexHtmSize);
  }
  void webReturnConfigHtm() {
    _server->send_P(200, "text/html", (const char*)gConfigHtmData,
                    gConfigHtmSize);
  }
  void webReturnAboutHtm() {
    _server->send_P(200, "text/html", (const char*)gAboutHtmData,
                    gAboutHtmSize);
  }
  void webReturnUploadHtm() {
    _server->send_P(200, "text/html", (const char*)gUploadHtmData,
                    gUploadHtmSize);
  }
#else
  void webReturnIndexHtm() {
    _server->send_P(200, "text/html", (const char*)indexHtmStart, indexHtmEnd-indexHtmStart);
  }
  void webReturnConfigHtm() {
    _server->send_P(200, "text/html", (const char*)configHtmStart, configHtmEnd-configHtmStart);
  }
  void webReturnAboutHtm() {
    _server->send_P(200, "text/html", (const char*)aboutHtmStart, aboutHtmEnd-aboutHtmStart);
  }
  void webReturnUploadHtm() {
    _server->send_P(200, "text/html", (const char*)uploadHtmStart, uploadHtmEnd-uploadHtmStart);
  }
#endif
  void webHandlePageNotFound();
  void webHandleUploadFile();
  void webHandleConfigRead();
  void webHandleConfigWrite();

  virtual void setupWebHandlers();

 public:
  explicit BaseWebHandler(WebConfig* config, int dynamicJsonSize = 2000);
  virtual bool setupWebServer();
  virtual void loop();
};

#endif  // SRC_BASEWEBHANDLER_HPP_

// EOF
