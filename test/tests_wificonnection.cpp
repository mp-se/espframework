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
#include <AUnit.h>
#include <wificonnection.hpp>

// Mock WifiConfigInterface for constructor
class MockWifiConfig : public WifiConfigInterface {
public:
  const char* getMDNS() const override { return "testmdns"; }
  const char* getWifiSSID(int idx) const override { return idx == 0 ? "ssid" : "ssid2"; }
  const char* getWifiPass(int idx) const override { return idx == 0 ? "pass" : "pass2"; }
  int getWifiConnectionTimeout() const override { return 10; }
  int getWifiPortalTimeout() const override { return 10; }
  bool getWifiScanAP() const override { return false; }
  void setMDNS(String) override {}
  void setWifiSSID(String, int) override {}
  void setWifiPass(String, int) override {}
  void setWifiConnectionTimeout(int) override {}
  void setWifiPortalTimeout(int) override {}
  void setWifiScanAP(bool) override {}
  void setWifiDirectSSID(String) override {}
  void setWifiDirectPass(String) override {}
  const char* getWifiDirectSSID() const override { return "directssid"; }
  const char* getWifiDirectPass() const override { return "directpass"; }
  bool saveFile() override { return true; }
};

test(WifiConnection_Construct) {
  MockWifiConfig cfg;
  WifiConnection wifi(&cfg, "apSSID", "apPWD", "apMDNS", "userSSID", "userPWD");
  // Just check that object is created and AP setters work
  wifi.setAP("newAP", "newPWD");
  // No assert, just ensure no crash
}

test(WifiConnection_EnableImprov) {
  MockWifiConfig cfg;
  WifiConnection wifi(&cfg, "apSSID", "apPWD", "apMDNS");
  wifi.enableImprov(true);
  // No assert, just ensure no crash
}

test(WifiConnection_HasConfig) {
  MockWifiConfig cfg;
  WifiConnection wifi(&cfg, "apSSID", "apPWD", "apMDNS");
  // hasConfig() may depend on internal state, just call to ensure no crash
  wifi.hasConfig();
}

test(WifiConnection_ConnectDisconnect) {
  // Note! This will only work if there is an SSID that can be connected to., skipping this test

  /*MockWifiConfig cfg;
  WifiConnection wifi(&cfg, "apSSID", "apPWD", "apMDNS");
  // Try to connect (STA mode)
  bool connected = wifi.connect();
  assertTrue(connected == wifi.isConnected());
  // Disconnect
  bool disconnected = wifi.disconnect();
  assertTrue(disconnected);*/
}

test(WifiConnection_APMode) {
  MockWifiConfig cfg;
  WifiConnection wifi(&cfg, "apSSID", "apPWD", "apMDNS");
  wifi.startAP();
  // After starting AP, should not crash, and AP config should be set
  assertTrue(true); // If no crash, test passes
}

test(WifiConnection_IPAddress) {
  // Note! This will only work if there is an SSID that can be connected to., skipping this test

  /*MockWifiConfig cfg;
  WifiConnection wifi(&cfg, "apSSID", "apPWD", "apMDNS");
  wifi.connect();
  String ip = wifi.getIPAddress();
  // Should return a non-empty string if connected
  assertTrue(ip.length() > 0);*/
}

test(WifiConnection_DoubleReset) {
  MockWifiConfig cfg;
  WifiConnection wifi(&cfg, "apSSID", "apPWD", "apMDNS");
  // Simulate double reset detection logic
  wifi.isDoubleResetDetected();
  wifi.stopDoubleReset();
  // Just ensure calls do not crash
  assertTrue(true);
}

test(WifiConnection_Loop) {
  MockWifiConfig cfg;
  WifiConnection wifi(&cfg, "apSSID", "apPWD", "apMDNS");
  wifi.loop();
  // Just ensure no crash
  assertTrue(true);
}

test(WifiConnection_DoubleResetFileNotWritten) {
  LittleFS.begin();
  LittleFS.remove("/reset.dat");
  MockWifiConfig cfg;
  WifiConnection wifi(&cfg, "apSSID", "apPWD", "apMDNS");
  wifi.init();
  wifi.stopDoubleReset(); 
  LittleFS.remove("/reset.dat"); 
  wifi.stopDoubleReset(); 
  bool exists = LittleFS.exists("/reset.dat");
  assertFalse(exists);
  LittleFS.end();
}

// EOF
