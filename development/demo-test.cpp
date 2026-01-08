/*
MIT License

Copyright (c) 2021-2024 Magnus

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
