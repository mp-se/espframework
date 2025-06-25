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
#include <AUnit.h>
#include <baseconfig.hpp>

// Mock JsonObject for testing (minimal stub)
class MockJsonObject {
public:
  void set(const char*, const char*) {}
  void set(const char*, int) {}
  void set(const char*, bool) {}
};

// Test BaseConfig constructor and getters/setters

test(BaseConfig_MDNS) {
  BaseConfig cfg("testmdns", "testfile");
  assertEqual(strncmp(cfg.getMDNS(), "testmdns", strlen("testmdns")), 0);
  cfg.setMDNS("newmdns");
  assertEqual(strcmp(cfg.getMDNS(), "newmdns"), 0);
}

test(BaseConfig_WifiSSIDPass) {
  BaseConfig cfg("mdns", "file");
  cfg.setWifiSSID("ssid1", 0);
  cfg.setWifiSSID("ssid2", 1);
  assertEqual(String(cfg.getWifiSSID(0)), String("ssid1"));
  assertEqual(String(cfg.getWifiSSID(1)), String("ssid2"));
  cfg.setWifiPass("pass1", 0);
  cfg.setWifiPass("pass2", 1);
  assertEqual(String(cfg.getWifiPass(0)), String("pass1"));
  assertEqual(String(cfg.getWifiPass(1)), String("pass2"));
}

test(BaseConfig_WifiDirect) {
  BaseConfig cfg("mdns", "file");
  cfg.setWifiDirectSSID("directssid");
  cfg.setWifiDirectPass("directpass");
  assertEqual(String(cfg.getWifiDirectSSID()), String("directssid"));
  assertEqual(String(cfg.getWifiDirectPass()), String("directpass"));
}

test(BaseConfig_WifiTimeouts) {
  BaseConfig cfg("mdns", "file");
  cfg.setWifiConnectionTimeout(42);
  cfg.setWifiPortalTimeout(99);
  assertEqual(cfg.getWifiConnectionTimeout(), 42);
  assertEqual(cfg.getWifiPortalTimeout(), 99);
}

test(BaseConfig_OtaConfig) {
  BaseConfig cfg("mdns", "file");
  cfg.setOtaURL("http://ota.url");
  assertTrue(cfg.isOtaActive());
  assertFalse(cfg.isOtaSSL());
  cfg.setOtaURL("https://ota.url");
  assertTrue(cfg.isOtaSSL());
}

test(BaseConfig_PushConfig) {
  BaseConfig cfg("mdns", "file");
  cfg.setTargetHttpPost("http://host");
  assertTrue(cfg.hasTargetHttpPost());
  cfg.setTargetHttpPost2("http://host2");
  assertTrue(cfg.hasTargetHttpPost2());
  cfg.setTargetHttpGet("http://get");
  assertTrue(cfg.hasTargetHttpGet());
  cfg.setTargetInfluxDB2("http://influx");
  assertTrue(cfg.hasTargetInfluxDb2());
  cfg.setTargetMqtt("mqtt");
  assertTrue(cfg.hasTargetMqtt());
}

test(BaseConfig_TempUnit) {
  BaseConfig cfg("mdns", "file");
  cfg.setTempUnit('F');
  assertEqual(cfg.getTempUnit(), 'F');
  assertTrue(cfg.isTempUnitF());
  assertFalse(cfg.isTempUnitC());
  cfg.setTempUnit('C');
  assertEqual(cfg.getTempUnit(), 'C');
  assertTrue(cfg.isTempUnitC());
  assertFalse(cfg.isTempUnitF());
}

test(BaseConfig_DarkMode) {
  BaseConfig cfg("mdns", "file");
  cfg.setDarkMode(true);
  assertTrue(cfg.getDarkMode());
  cfg.setDarkMode(false);
  assertFalse(cfg.getDarkMode());
}

// EOF
