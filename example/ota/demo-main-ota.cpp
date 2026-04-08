/*
  Copyright (c) 2021-2026 Magnus
  https://github.com/mp-se/espframework

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program. If not, see <https://www.gnu.org/licenses/>.
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
