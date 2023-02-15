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
#if !defined(USE_ASYNC_WEB)

#include <LittleFS.h>

#include <demo-push.hpp>
#include <demo-webhandler.hpp>
#include <espframework.hpp>
#include <log.hpp>

DemoWebHandler::DemoWebHandler(WebConfig* config, DemoPush* push)
    : BaseWebHandler(config) {
  _push = push;
}

void DemoWebHandler::setupWebHandlers() {
  Log.notice(F("WEB : Setting up web handlers." CR));
  BaseWebHandler::setupWebHandlers();

  _server->on("/test.htm", std::bind(&DemoWebHandler::webReturnTestHtm, this));

  _server->serveStatic("/log", LittleFS, ERR_FILENAME);
  _server->serveStatic("/log2", LittleFS, ERR_FILENAME2);

  _server->on("/api/status", HTTP_GET,
              std::bind(&DemoWebHandler::webHandleStatus, this));
  _server->on("/api/push/http-post", HTTP_POST,
              std::bind(&DemoWebHandler::webHandlePushHttpPost, this));
  _server->on("/api/push/http-get", HTTP_POST,
              std::bind(&DemoWebHandler::webHandlePushHttpGet, this));
  _server->on("/api/push/influxdb2", HTTP_POST,
              std::bind(&DemoWebHandler::webHandlePushHttpInfluxDb2, this));
  _server->on("/api/push/mqtt", HTTP_POST,
              std::bind(&DemoWebHandler::webHandlePushHttpMqtt, this));
}

void DemoWebHandler::webHandleStatus() {
  Log.notice(F("WEB : DemoWebHandler callback for /api/status)." CR));

  DynamicJsonDocument doc(512);

  doc[PARAM_ID] = _webConfig->getID();
  doc[PARAM_MDNS] = _webConfig->getMDNS();
  doc[PARAM_SSID] = WiFi.SSID();

#if LOG_LEVEL == 6
  serializeJson(doc, Serial);
  Serial.print(CR);
#endif

  String out;
  out.reserve(2048);
  serializeJson(doc, out);
  _server->send(200, "application/json", out.c_str());
}

void DemoWebHandler::webHandlePushHttpPost() {
  Log.notice(F("WEB : DemoWebHandler callback for /api/push/http-post." CR));
}

void DemoWebHandler::webHandlePushHttpGet() {
  Log.notice(F("WEB : DemoWebHandler callback for /api/push/http-get." CR));
}

void DemoWebHandler::webHandlePushHttpMqtt() {
  Log.notice(F("WEB : DemoWebHandler callback for /api/push/mqtt." CR));
}

void DemoWebHandler::webHandlePushHttpInfluxDb2() {
  Log.notice(F("WEB : DemoWebHandler callback for /api/push/influxdb2." CR));
}

#endif // USE_ASYNC_WEB

// EOF
