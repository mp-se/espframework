/*
 * ESPFramework
 * Copyright (c) 2021-2026 Magnus
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 */
#include <demo-config.hpp>
#include <demo-main.hpp>
#include <demo-push.hpp>
#include <espframework.hpp>
#include <log.hpp>
#include <ota.hpp>
#include <perf.hpp>
#include <serialws.hpp>
#include <wificonnection.hpp>
#include <uptime.hpp>

SerialDebug mySerial(115200L);
DemoConfig myConfig("mdnsbase", "/esplib.cfg");
WifiConnection myWifi(&myConfig, "espSSID", "password", "esplib", "", "");
OtaUpdate myOta(&myConfig, "1.0.0");
DemoPush myPush(&myConfig);

#include <demo-webserver.hpp>
DemoWebServer myDemoWebServer(&myConfig, &myPush);
SerialWebSocket mySerialWebSocket;

void setup() {
  delay(4000);
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
    myDemoWebServer.setWifiSetup(true);
    myWifi.startAP();
  } else {
    PERF_BEGIN("wifi-connect");
    myWifi.connect(WIFI_AP_STA);
    myWifi.setAP("extra", "password");  // Will create an AP as well as
                                        // connecting to the defined wifi
    myWifi.startAP(WIFI_AP_STA);
    PERF_END("wifi-connect");
    PERF_PUSH();
    myWifi.timeSync();

    if (!myWifi.isConnected() || myOta.checkFirmwareVersion()) {
      Log.notice(F("Main: New firmware available via OTA, doing update." CR));
      myOta.updateFirmware();
    }
  }

  myDemoWebServer.setupWebServer();
  mySerialWebSocket.begin(myDemoWebServer.getWebServer(), &Serial);
  mySerial.begin(&mySerialWebSocket);

  Serial.println("Setup() complete");
  Log.notice(F("Main: Setup is completed." CR));
}

void loop() {
  myWifi.loop();
  myDemoWebServer.loop();
  mySerialWebSocket.loop();
  // Log.notice(F("Loop:" CR));
  delay(2000);

  myUptime.calculate();
  Log.notice(F("Loop: Uptime %d days, %d hours, %d minutes, %d seconds." CR), myUptime.getDays(), myUptime.getHours(), myUptime.getMinutes(), myUptime.getSeconds());
}

// EOF
