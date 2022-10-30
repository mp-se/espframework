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
#define USE_ASYNC_WEB 1

#include <demo-config.hpp>
#include <demo-main.hpp>
#include <demo-push.hpp>
#include <espframework.hpp>
#include <ota.hpp>
#include <perf.hpp>
#include <wificonnection.hpp>
#include <log.hpp>

SerialDebug mySerial(115200L);
DemoConfig myConfig("mdnsbase", "/esplib.cfg");
WifiConnection myWifi(&myConfig, "espSSID", "password", "esplib", "", "");
OtaUpdate myOta(&myConfig, "0.0.0");
DemoPush myPush(&myConfig);

#if defined(USE_ASYNC_WEB)
#include <demo-asyncwebhandler.hpp>
DemoAsyncWebHandler myAsyncWebHandler(&myConfig, &myPush);
#else
#include <demo-webhandler.hpp>
DemoWebHandler myWebHandler(&myConfig, &myPush);
#endif

void setup() {
  delay(2000);
  Log.notice(F("Main: Started setup." CR));

#if defined(PERF_ENABLE)
  PerfLogging perf;
  perf.getInstance().setBaseConfig(&myConfig);
#endif
  myConfig.checkFileSystem();
  myConfig.loadFile();

  myWifi.init();
  if (!myWifi.hasConfig() || myWifi.isDoubleResetDetected()) {
    Log.notice(
        F("Main: Missing wifi config or double reset detected, entering wifi "
          "setup." CR));
    myWifi.startPortal();
  }

  PERF_BEGIN("wifi-connect");
  myWifi.connect();
  PERF_END("wifi-connect");
  PERF_PUSH();
  myWifi.timeSync();

  if (!myWifi.isConnected() || myOta.checkFirmwareVersion()) {
    Log.notice(F("Main: New firmware available via OTA, doing update." CR));
    myOta.updateFirmware();
  }

  if (myWifi.isConnected()) {
#if defined(USE_ASYNC_WEB)
    myAsyncWebHandler.setupAsyncWebServer();
#else
    myWebHandler.setupWebServer();
#endif
  }

  Serial.println("Setup() complete");
  Log.notice(F("Main: Setup is completed." CR));
}

void loop() {
  myWifi.loop();
#if defined(USE_ASYNC_WEB)
  myAsyncWebHandler.loop();
#else
  myWebHandler.loop();
#endif
}

// EOF
