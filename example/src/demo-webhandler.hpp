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
#ifndef SRC_DEMO_WEBHANDLER_HPP_
#define SRC_DEMO_WEBHANDLER_HPP_

#include <basewebhandler.hpp>
#include <demo-push.hpp>

#if defined(ESP8266)
#include <incbin.h>
INCBIN_EXTERN(TestHtm);
#else
extern const uint8_t testHtmStart[] asm("_binary_html_test_min_htm_start");
extern const uint8_t testHtmEnd[] asm("_binary_html_test_min_htm_end");
#endif

class DemoWebHandler : public BaseWebHandler {
 private:
  DemoPush* _push;

#if defined(ESP8266)
  void webReturnTestHtm() {
    _server->send_P(200, "text/html", (const char*)gTestHtmData, gTestHtmSize);
  }
#else
  void webReturnTestHtm() {
    _server->send_P(200, "text/html", (const char*)testHtmStart,
                    strlen(reinterpret_cast<const char*>(&testHtmStart[0])));
  }
#endif

  void setupWebHandlers();

  void webHandleStatus();
  void webHandlePushHttpPost();
  void webHandlePushHttpGet();
  void webHandlePushHttpMqtt();
  void webHandlePushHttpInfluxDb2();

 public:
  explicit DemoWebHandler(WebConfig* config, DemoPush* push);
};

#endif  // SRC_DEMO_WEBHANDLER_HPP_

// EOF
