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
#include <demo-config.hpp>
#include <demo-main.hpp>
#include <demo-push.hpp>
#include <demo-webhandler.hpp>
#include <espframework.hpp>
#include <ota.hpp>
#include <wificonnection.hpp>

SerialDebug mySerial(115200L);
DemoConfig myConfig("mdnsbase", "/esplib.cfg");
WifiConnection myWifi(&myConfig, "espSSID", "password", "esplib", "", "");
OtaUpdate myOta(&myConfig, "0.0.0");
DemoPush myPush(&myConfig);
DemoWebHandler myWebHandler(&myConfig, &myPush);

void setup() {
  Log.notice(F("Main: Started setup for %s." CR),
             String(ESP.getChipId(), HEX).c_str());

  myConfig.checkFileSystem();
  myConfig.loadFile();
  myWifi.init();

  if (!myWifi.hasConfig() || myWifi.isDoubleResetDetected()) {
    Log.notice(
        F("Main: Missing wifi config or double reset detected, entering wifi "
          "setup." CR));
    myWifi.startPortal();
  }

  myWifi.connect();

  if (!myWifi.isConnected() || myOta.checkFirmwareVersion()) {
    Log.notice(F("Main: New firmware available via OTA, doing update." CR));
    myOta.updateFirmware();
  }

  if (myWifi.isConnected()) {
    myWebHandler.setupWebServer();
  }

  Log.notice(F("Main: Setup is completed." CR));
}

void loop() {
  myWifi.loop();
  myWebHandler.loop();
}

// EOF
