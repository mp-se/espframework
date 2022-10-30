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
#include <LittleFS.h>

#include <demo-push.hpp>
#include <demo-asyncwebhandler.hpp>
#include <espframework.hpp>
#include <log.hpp>

DemoAsyncWebHandler::DemoAsyncWebHandler(WebConfig* config, DemoPush* push)
    : BaseAsyncWebHandler(config) {
  _push = push;
}

void DemoAsyncWebHandler::setupAsyncWebHandlers() {
  Log.notice(F("WEB : Setting up web handlers." CR));
  BaseAsyncWebHandler::setupAsyncWebHandlers();

  _server->on("/test.htm", std::bind(&DemoAsyncWebHandler::webReturnTestHtm, this, std::placeholders::_1));

  _server->serveStatic("/log", LittleFS, ERR_FILENAME);
  _server->serveStatic("/log2", LittleFS, ERR_FILENAME2);

  _server->on("/api/status", HTTP_GET,
              std::bind(&DemoAsyncWebHandler::webHandleStatus, this, std::placeholders::_1));
  _server->on("/api/push/http-post", HTTP_POST,
              std::bind(&DemoAsyncWebHandler::webHandlePushHttpPost, this, std::placeholders::_1));
  _server->on("/api/push/http-get", HTTP_POST,
              std::bind(&DemoAsyncWebHandler::webHandlePushHttpGet, this, std::placeholders::_1));
  _server->on("/api/push/influxdb2", HTTP_POST,
              std::bind(&DemoAsyncWebHandler::webHandlePushHttpInfluxDb2, this, std::placeholders::_1));
  _server->on("/api/push/mqtt", HTTP_POST,
              std::bind(&DemoAsyncWebHandler::webHandlePushHttpMqtt, this, std::placeholders::_1));
}

void DemoAsyncWebHandler::webHandleStatus(AsyncWebServerRequest *request) {
  DynamicJsonDocument doc(512);

  doc[PARAM_ID] = _webConfig->getID();
  doc[PARAM_MDNS] = _webConfig->getMDNS();
  doc[PARAM_SSID] = WiFi.SSID();

  String out;
  out.reserve(2048);
  serializeJson(doc, out);
  request->send(200, "application/json", out.c_str());
}

void DemoAsyncWebHandler::webHandlePushHttpPost(AsyncWebServerRequest *request) {
}

void DemoAsyncWebHandler::webHandlePushHttpGet(AsyncWebServerRequest *request) {
}

void DemoAsyncWebHandler::webHandlePushHttpMqtt(AsyncWebServerRequest *request) {
}

void DemoAsyncWebHandler::webHandlePushHttpInfluxDb2(AsyncWebServerRequest *request) {
}

// EOF
