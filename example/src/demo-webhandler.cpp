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
#include <demo-webhandler.hpp>
#include <demo-push.hpp>
#include <espframework.hpp>

DemoWebHandler::DemoWebHandler(WebConfig* config, DemoPush* push) : BaseWebHandler(config) {
  _push = push;
}

void DemoWebHandler::setupWebHandlers() {
  Log.notice(F("WEB : Setting up web handlers." CR));
  BaseWebHandler::setupWebHandlers();

  _server->on("/test.htm", std::bind(&DemoWebHandler::webReturnTestHtm, this));

  _server->on("/api/push/http-post", HTTP_POST, std::bind(&DemoWebHandler::webHandlePushHttpPost, this));
  _server->on("/api/push/http-get", HTTP_POST, std::bind(&DemoWebHandler::webHandlePushHttpGet, this));
  _server->on("/api/push/influxdb2", HTTP_POST, std::bind(&DemoWebHandler::webHandlePushHttpInfluxDb2, this));
  _server->on("/api/push/mqtt", HTTP_POST, std::bind(&DemoWebHandler::webHandlePushHttpMqtt, this));
}

void DemoWebHandler::webHandlePushHttpPost() {
  Log.notice(F("WEB : BaseWebHandler callback for /api/push/http-post." CR));

}

void DemoWebHandler::webHandlePushHttpGet() {
  Log.notice(F("WEB : BaseWebHandler callback for /api/push/http-get." CR));
  
}

void DemoWebHandler::webHandlePushHttpMqtt() {
  Log.notice(F("WEB : BaseWebHandler callback for /api/push/mqtt." CR));

}

void DemoWebHandler::webHandlePushHttpInfluxDb2() {
  Log.notice(F("WEB : BaseWebHandler callback for /api/push/influxdb2." CR));

}

// EOF
