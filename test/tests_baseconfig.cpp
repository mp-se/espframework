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
#include <LittleFS.h>
#include <ArduinoJson.h>

// Mock JsonObject for testing (minimal stub)
class MockJsonObject {
public:
  void set(const char*, const char*) {}
  void set(const char*, int) {}
  void set(const char*, bool) {}
};

// Helper subclass to expose protected and private methods for testing
class TestableBaseConfig : public BaseConfig {
public:
  using BaseConfig::BaseConfig;
  using BaseConfig::createJsonBase;
  using BaseConfig::parseJsonBase;
  using BaseConfig::createJsonWifi;
  using BaseConfig::parseJsonWifi;
  using BaseConfig::createJsonOta;
  using BaseConfig::parseJsonOta;
  using BaseConfig::createJsonPush;
  using BaseConfig::parseJsonPush;
  // void callFormatFileSystem() { this->formatFileSystem(); }
  void callCheckFileSystem() { this->checkFileSystem(); }
  // Add wrappers for base JSON methods for test compatibility
  void testCreateJsonBase(JsonObject& doc) const { this->createJsonBase(doc); }
  void testParseJsonBase(JsonObject& doc) { this->parseJsonBase(doc); }
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

test(BaseConfig_SaveAndLoadFile) {
  LittleFS.begin();
  const char* filename = "/test_baseconfig.json";
  LittleFS.remove(filename);

  BaseConfig cfg("mdns", filename);
  cfg.setWifiSSID("ssid1", 0);
  cfg.setWifiPass("pass1", 0);
  cfg.setTempUnit('F');
  cfg.setDarkMode(true);
  cfg.setSaveNeeded();

  // Save to file
  assertTrue(cfg.saveFile());
  assertTrue(LittleFS.exists(filename));

  LittleFS.remove(filename);
  LittleFS.end();
}

test(BaseConfig_SaveFileNotNeeded) {
  BaseConfig cfg("mdns", "/test_baseconfig.json");
  // _saveNeeded is false by default
  assertTrue(cfg.saveFile()); // Should skip save and return true
}

test(BaseConfig_JsonRoundTrip) {
  TestableBaseConfig cfg("mdns", "file");
  JsonDocument doc;
  JsonObject obj = doc.to<JsonObject>();
  cfg.setTempUnit('F');
  cfg.setDarkMode(true);
  cfg.testCreateJsonBase(obj);

  // Change values, then parse back
  cfg.setTempUnit('C');
  cfg.setDarkMode(false);
  cfg.testParseJsonBase(obj);
  assertEqual(cfg.getTempUnit(), 'F');
  assertTrue(cfg.getDarkMode());
}

test(BaseConfig_JsonWifiOtaPush) {
  TestableBaseConfig cfg("mdns", "file");
  JsonDocument doc;
  JsonObject obj = doc.to<JsonObject>();
  // Wifi
  cfg.setWifiSSID("ssid1", 0);
  cfg.setWifiSSID("ssid2", 1);
  cfg.setWifiPass("pass1", 0);
  cfg.setWifiPass("pass2", 1);
  cfg.setWifiDirectSSID("directssid");
  cfg.setWifiDirectPass("directpass");
  cfg.setWifiConnectionTimeout(42);
  cfg.setWifiPortalTimeout(99);
  cfg.setWifiScanAP(true);
  cfg.createJsonWifi(obj);
  cfg.setWifiSSID("", 0);
  cfg.setWifiPass("", 0);
  cfg.setWifiSSID("", 1);
  cfg.setWifiPass("", 1);
  cfg.setWifiDirectSSID("");
  cfg.setWifiDirectPass("");
  cfg.setWifiConnectionTimeout(0);
  cfg.setWifiPortalTimeout(0);
  cfg.setWifiScanAP(false);
  cfg.parseJsonWifi(obj);
  assertEqual(String(cfg.getWifiSSID(0)), String("ssid1"));
  assertEqual(String(cfg.getWifiSSID(1)), String("ssid2"));
  assertEqual(String(cfg.getWifiPass(0)), String("pass1"));
  assertEqual(String(cfg.getWifiPass(1)), String("pass2"));
  assertEqual(String(cfg.getWifiDirectSSID()), String("directssid"));
  assertEqual(String(cfg.getWifiDirectPass()), String("directpass"));
  assertEqual(cfg.getWifiConnectionTimeout(), 42);
  assertEqual(cfg.getWifiPortalTimeout(), 99);
  assertTrue(cfg.getWifiScanAP());
  // Ota
  cfg.setOtaURL("http://ota.url");
  cfg.createJsonOta(obj);
  cfg.setOtaURL("");
  cfg.parseJsonOta(obj);
  assertEqual(String(cfg.getOtaURL()), String("http://ota.url"));
  // Push
  cfg.setTargetHttpPost("http://host");
  cfg.setHeader1HttpPost("Header1");
  cfg.setHeader2HttpPost("Header2");
  cfg.setTcpHttpPost(true);
  cfg.setTargetHttpPost2("http://host2");
  cfg.setHeader1HttpPost2("Header1-2");
  cfg.setHeader2HttpPost2("Header2-2");
  cfg.setTargetHttpGet("http://get");
  cfg.setHeader1HttpGet("Header1G");
  cfg.setHeader2HttpGet("Header2G");
  cfg.setTargetInfluxDB2("http://influx");
  cfg.setOrgInfluxDB2("org");
  cfg.setBucketInfluxDB2("bucket");
  cfg.setTokenInfluxDB2("token");
  cfg.setTargetMqtt("mqtt");
  cfg.setPortMqtt(1234);
  cfg.setUserMqtt("user");
  cfg.setPassMqtt("pass");
  cfg.setPushTimeout(55);
  cfg.createJsonPush(obj);
  // Clear and parse back
  cfg.setTargetHttpPost("");
  cfg.setHeader1HttpPost("");
  cfg.setHeader2HttpPost("");
  cfg.setTcpHttpPost(false);
  cfg.setTargetHttpPost2("");
  cfg.setHeader1HttpPost2("");
  cfg.setHeader2HttpPost2("");
  cfg.setTargetHttpGet("");
  cfg.setHeader1HttpGet("");
  cfg.setHeader2HttpGet("");
  cfg.setTargetInfluxDB2("");
  cfg.setOrgInfluxDB2("");
  cfg.setBucketInfluxDB2("");
  cfg.setTokenInfluxDB2("");
  cfg.setTargetMqtt("");
  cfg.setPortMqtt(0);
  cfg.setUserMqtt("");
  cfg.setPassMqtt("");
  cfg.setPushTimeout(0);
  cfg.parseJsonPush(obj);
  assertEqual(String(cfg.getTargetHttpPost()), String("http://host"));
  assertEqual(String(cfg.getHeader1HttpPost()), String("Header1"));
  assertEqual(String(cfg.getHeader2HttpPost()), String("Header2"));
  assertTrue(cfg.getTcpHttpPost());
  assertEqual(String(cfg.getTargetHttpPost2()), String("http://host2"));
  assertEqual(String(cfg.getHeader1HttpPost2()), String("Header1-2"));
  assertEqual(String(cfg.getHeader2HttpPost2()), String("Header2-2"));
  assertEqual(String(cfg.getTargetHttpGet()), String("http://get"));
  assertEqual(String(cfg.getHeader1HttpGet()), String("Header1G"));
  assertEqual(String(cfg.getHeader2HttpGet()), String("Header2G"));
  assertEqual(String(cfg.getTargetInfluxDB2()), String("http://influx"));
  assertEqual(String(cfg.getOrgInfluxDB2()), String("org"));
  assertEqual(String(cfg.getBucketInfluxDB2()), String("bucket"));
  assertEqual(String(cfg.getTokenInfluxDB2()), String("token"));
  assertEqual(String(cfg.getTargetMqtt()), String("mqtt"));
  assertEqual(cfg.getPortMqtt(), 1234);
  assertEqual(String(cfg.getUserMqtt()), String("user"));
  assertEqual(String(cfg.getPassMqtt()), String("pass"));
  assertEqual(cfg.getPushTimeout(), 55);
}

test(BaseConfig_ExtraSettersGetters) {
  BaseConfig cfg("mdns", "file");
  cfg.setCorsAllowed(true);
  assertTrue(cfg.getCorsAllowed());
  cfg.setCorsAllowed(false);
  assertFalse(cfg.getCorsAllowed());
  cfg.setPushTimeout(77);
  assertEqual(cfg.getPushTimeout(), 77);
  cfg.setHeader1HttpPost("H1");
  assertEqual(String(cfg.getHeader1HttpPost()), String("H1"));
  cfg.setHeader2HttpPost("H2");
  assertEqual(String(cfg.getHeader2HttpPost()), String("H2"));
  cfg.setHeader1HttpPost2("H1-2");
  assertEqual(String(cfg.getHeader1HttpPost2()), String("H1-2"));
  cfg.setHeader2HttpPost2("H2-2");
  assertEqual(String(cfg.getHeader2HttpPost2()), String("H2-2"));
  cfg.setHeader1HttpGet("H1G");
  assertEqual(String(cfg.getHeader1HttpGet()), String("H1G"));
  cfg.setHeader2HttpGet("H2G");
  assertEqual(String(cfg.getHeader2HttpGet()), String("H2G"));
  cfg.setTempFormat('C'); 
  cfg.setTempFormat('X'); 
  assertEqual(cfg.getTempFormat(), 'C');
}

test(BaseConfig_Callability) {
  TestableBaseConfig cfg("mdns", "file");
  // cfg.callFormatFileSystem();
  cfg.callCheckFileSystem();
#if !defined(ESP8266)
  cfg.getWifiPreference();
  cfg.setPreference("k", "v", "n");
  cfg.getPreference("k", "n");
#endif
  assertTrue(true); // If no crash, test passes
}

// EOF
