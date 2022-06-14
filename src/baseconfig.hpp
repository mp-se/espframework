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
#ifndef SRC_BASECONFIG_HPP_
#define SRC_BASECONFIG_HPP_

#include <interface.hpp>

#define PARAM_ID "id"
#define PARAM_MDNS "mdns"
#define PARAM_SSID "wifi-ssid"
#define PARAM_PASS "wifi-pass"
#define PARAM_SSID2 "wifi-ssid2"
#define PARAM_PASS2 "wifi-pass2"
#define PARAM_OTA_URL "ota-url"
#define PARAM_TEMP_FORMAT "temp-format"

class BaseConfig : public WifiConfig, public OtaConfig, public WebConfig {
 private:
  // WifiConfig
  String _mDNS;
  String _wifiSSID[2] = {"", ""};
  String _wifiPASS[2] = {"", ""};

  // OtaConfig
  String _otaURL;

  // BaseConfig
  String _id;
  char _tempFormat = 'C';

  String _fileName;

  void formatFileSystem();

  // Internal
 protected:
  bool _saveNeeded;

  void createJsonBase(DynamicJsonDocument& doc);
  void createJsonOta(DynamicJsonDocument& doc);
  void createJsonWifi(DynamicJsonDocument& doc);

  void parseJsonBase(DynamicJsonDocument& doc);
  void parseJsonOta(DynamicJsonDocument& doc);
  void parseJsonWifi(DynamicJsonDocument& doc);

 public:
  BaseConfig(String baseMDNS, String fileName);

  // WifiConfig
  const char* getMDNS() { return _mDNS.c_str(); }
  void setMDNS(String s) {
    _mDNS = s;
    _saveNeeded = true;
  }
  const char* getWifiSSID(int idx) { return _wifiSSID[idx].c_str(); }
  void setWifiSSID(String s, int idx) {
    _wifiSSID[idx] = s;
    _saveNeeded = true;
  }
  const char* getWifiPass(int idx) { return _wifiPASS[idx].c_str(); }
  void setWifiPass(String s, int idx) {
    _wifiPASS[idx] = s;
    _saveNeeded = true;
  }
  int getWifiConnectionTimeout() { return 30; }
  int getWifiPortalTimeout() { return 120; }

  // OtaConfig
  const char* getOtaURL() { return _otaURL.c_str(); }
  void setOtaURL(String s) {
    _otaURL = s;
    _saveNeeded = true;
  }
  bool isOtaActive() { return _otaURL.length() > 0 ? true : false; }
  bool isOtaSSL() { return _otaURL.startsWith("https://"); }

  // Base
  const char* getID() { return _id.c_str(); }

  char getTempFormat() { return _tempFormat; }
  void setTempFormat(char c) {
    if (c == 'C' || c == 'F') {
      _tempFormat = c;
      _saveNeeded = true;
    }
  }
  bool isTempC() { return _tempFormat == 'C' ? true : false; }
  bool isTempF() { return _tempFormat == 'F' ? true : false; }

  // Functions
  virtual void createJson(DynamicJsonDocument& doc) {}
  virtual void parseJson(DynamicJsonDocument& doc) {}

  bool saveFile();
  bool loadFile();
  void checkFileSystem();
  bool isSaveNeeded() { return _saveNeeded; }
};

#endif  // SRC_BASECONFIG_HPP_

// EOF
