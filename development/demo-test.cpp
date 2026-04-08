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
#include <espframework.hpp>
#include <log.hpp>
#include <wificonnection.hpp>
#include <baseconfig.hpp>
#include <basepush.hpp>
#include <templating.hpp>

SerialDebug mySerial(115200L);
// BaseConfig myConfig("mdnsbase", "/esplib.cfg", JSON_BUFFER_SIZE_XL);
BaseConfig myConfig("mdnsbase", "/esplib.cfg");
WifiConnection myWifi(&myConfig, "", "", "esplib", "", "");
// DemoPush myPush(&myConfig);

void setup() {
  delay(1000);
  Log.notice(F("Main: Started setup." CR));

  myConfig.checkFileSystem();
  myConfig.loadFile();

  myConfig.setWifiSSID("", 0);
  myConfig.setWifiPass("", 0);

  myWifi.init();
  myWifi.connect(WIFI_AP_STA);

  Log.notice(F("Main: Setup is completed." CR));

  // Testing Templating before refactor
  printHeap("BEFORE");
  TemplatingEngine engine;
  engine.setVal("${Key1}", "Value1 - This is a longer string to test the heap.");
  engine.setVal("${Key2}", "Value2 - This is a longer string to test the heap.");
  engine.setVal("${Key3}", "Value3 - This is a longer string to test the heap.");
  engine.setVal("${Key4}", "Value4 - This is a longer string to test the heap.");
  engine.setVal("${Key5}", "Value5 - This is a longer string to test the heap.");
  engine.setVal("${Key6}", "Value6 - This is a longer string to test the heap.");
  engine.setVal("${Key7}", "Value7 - This is a longer string to test the heap.");
  engine.setVal("${Key8}", "Value8 - This is a longer string to test the heap.");
  engine.setVal("${Key9}", "Value9 - This is a longer string to test the heap.");
  printHeap("MID");
  // Log.notice(F("Main: %s." CR), engine.create("This is a test: ${Key1} ${Key2} ${Key3} ${Key4} ${Key5}"));
  Serial.println(engine.create("This is a test: ${Key1} ${Key2} ${Key3} ${Key4} ${Key5}"));
  printHeap("AFTER");

//   BasePush myPush(&myConfig);
//   String payload;

//   printHeap("MQTT BEFORE");
//   myConfig.setTargetMqtt("192.168.1.134");
//   myConfig.setPortMqtt(1883);
//   myConfig.setUserMqtt("");
//   myConfig.setPassMqtt("");
//   payload = "ispindel/testing/tilt:45|"
//             "ispindel/testing/temperature:22.5|";
//   myPush.sendMqtt(payload);
//   printHeap("MQTT AFTER");

//   printHeap("MQTT SSL BEFORE");
//   myConfig.setTargetMqtt("192.168.1.134");
//   myConfig.setPortMqtt(8883);
//   myConfig.setUserMqtt("");
//   myConfig.setPassMqtt("");
//   payload = "ispindel/testing/tilt:45|"
//             "ispindel/testing/temperature:22.5|";
//   myPush.sendMqtt(payload);
//   printHeap("MQTT SSL AFTER");

//   printHeap("POST BEFORE");
//   myConfig.setTargetHttpPost("http://192.168.1.134/gravmon/");
//   payload = "{\"name\": \"test\", \"ID\": \"e17d44\", \"token\": \"\", \"interval\": 300, \"temperature\": 22.3, \"temp_units\": \"C\", "
//             "\"gravity\": 1.001, \"angle\": 24.45, \"gravity_units\": \"SG\", \"battery\": 3.85, \"RSSI\": -76.2 }";
//   myPush.sendHttpPost(payload);
//   printHeap("POST AFTER");

//  printHeap("POST SSL BEFORE");
//   myConfig.setTargetHttpPost("https://192.168.1.134/gravmon/");
//   payload = "{\"name\": \"test\", \"ID\": \"e17d44\", \"token\": \"\", \"interval\": 300, \"temperature\": 22.3, \"temp_units\": \"C\", "
//             "\"gravity\": 1.001, \"angle\": 24.45, \"gravity_units\": \"SG\", \"battery\": 3.85, \"RSSI\": -76.2 }";
//   myPush.sendHttpPost(payload);
//   printHeap("POST SSL AFTER");

  Log.notice(F("Main: Exiting setup method." CR));
}

void loop() {
  // myWifi.loop();

  delay(2000);
}

// EOF
