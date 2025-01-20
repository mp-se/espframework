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
#include <baseconfig.hpp>
#include <espframework.hpp>
#include <log.hpp>
#include <ota.hpp>
#include <wificonnection.hpp>

SerialDebug mySerial(115200L);
BaseConfig myConfig("mdnsbase", "/esplib.cfg");
WifiConnection myWifi(&myConfig, "espSSID", "password", "esplib", "", "");

// Here the OTA class is intialzed with the current version (format is Major.Minor.Patch)
OtaUpdate myOta(&myConfig, "1.1.0");

void setup() {
  delay(4000);  // Allow some time for computer to connect

  Log.notice(F("Main: Started setup." CR));

  myConfig.checkFileSystem();
  myConfig.loadFile();
  
  myConfig.setWifiSSID("myssid", 0); // Set your wifi SSID
  myConfig.setWifiPass("password", 0); // Set your wifi password

  // Set the URL for the directory that contains the version.json file, this file has 
  // the new version number that will be compered towards the current version
  myConfig.setOtaURL("http://server/firmware/"); 
  
  myConfig.saveFile();

  myWifi.init();  
  myWifi.connect(WIFI_AP_STA);

  if (!myWifi.isConnected() || myOta.checkFirmwareVersion()) {
    Log.notice(F("Main: New firmware available via OTA, doing update." CR));
    myOta.updateFirmware();
  }
  Log.notice(F("Main: Setup is completed." CR));
}

void loop() {
  myWifi.loop();
}

// EOF
