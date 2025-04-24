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
#ifndef SRC_BASECONFIG_HPP_
#define SRC_BASECONFIG_HPP_

#include <espframework.hpp>
#include <interface.hpp>

class BaseConfig : public WifiConfigInterface,
                   public OtaConfigInterface,
                   public WebConfigInterface,
                   public PushConfigInterface {
 private:
  // WifiConfig
  String _mDNS;
  String _wifiSSID[2] = {"", ""};
  String _wifiPASS[2] = {"", ""};
  String _wifiDirectSSID = "";
  String _wifiDirectPASS = "";
  int _wifiConnectionTimeout = 30;
  int _wifiPortalTimeout = 120;
  bool _wifiScanAP = false;

  // OtaConfig
  String _otaURL;

  // PushConfig
  String _targetHttpPost;
  String _header1HttpPost = "Content-Type: application/json";
  String _header2HttpPost;
  String _targetHttpPost2;
  String _header1HttpPost2 = "Content-Type: application/json";
  String _header2HttpPost2;
  String _targetHttpGet;
  String _header1HttpGet;
  String _header2HttpGet;
  String _targetInfluxDb2;
  String _orgInfluxDb2;
  String _bucketInfluxDb2;
  String _tokenInfluxDb2;
  String _targetMqtt;
  int _portMqtt = 1883;
  String _userMqtt;
  String _passMqtt;

  int _pushTimeout = 10;

  // BaseConfig
  String _id;
  char _tempUnit = 'C';
  String _fileName;
  bool _darkMode = false;

  // WebServer
#if defined(ENABLE_REMOTE_UI_DEVELOPMENT)
  bool _corsAllowed = true;
#else
  bool _corsAllowed = false;
#endif

  void formatFileSystem();
  void getWifiPreference();

  // Internal
 protected:
  bool _saveNeeded;

  void createJsonBase(JsonObject& doc) const;
  void createJsonOta(JsonObject& doc) const;
  void createJsonWifi(JsonObject& doc) const;
  void createJsonPush(JsonObject& doc) const;

  void parseJsonBase(JsonObject& doc);
  void parseJsonOta(JsonObject& doc);
  void parseJsonWifi(JsonObject& doc);
  void parseJsonPush(JsonObject& doc);

 public:
  BaseConfig(String baseMDNS, String fileName);

  // WebServer
  bool getCorsAllowed() const { return _corsAllowed; }
  void setCorsAllowed(bool b) {
    _corsAllowed = b;
    _saveNeeded = true;
  }
  bool isCorsAllowed()  const { return getCorsAllowed(); }

  // WifiConfig
  const char* getMDNS() const { return _mDNS.c_str(); }
  void setMDNS(String s) {
    _mDNS = s;
    _saveNeeded = true;
  }
  const char* getWifiSSID(int idx) const { return _wifiSSID[idx].c_str(); }
  void setWifiSSID(String s, int idx) {
    _wifiSSID[idx] = s;
    _saveNeeded = true;
  }
  const char* getWifiPass(int idx) const { return _wifiPASS[idx].c_str(); }
  void setWifiPass(String s, int idx) {
    _wifiPASS[idx] = s;
    _saveNeeded = true;
  }
  const char* getWifiDirectSSID() const { return _wifiDirectSSID.c_str(); }
  void setWifiDirectSSID(String s) {
    _wifiDirectSSID = s;
    _saveNeeded = true;
  }
  const char* getWifiDirectPass() const { return _wifiDirectPASS.c_str(); }
  void setWifiDirectPass(String s) {
    _wifiDirectPASS = s;
    _saveNeeded = true;
  }
  bool dualWifiConfigured() const {
    return _wifiSSID[0].length() > 0 && _wifiSSID[1].length() > 0 ? true
                                                                  : false;
  }
  void swapPrimaryWifi() {
    String s = _wifiSSID[0];
    _wifiSSID[0] = _wifiSSID[1];
    _wifiSSID[1] = s;

    String p = _wifiPASS[0];
    _wifiPASS[0] = _wifiPASS[1];
    _wifiPASS[1] = p;

    _saveNeeded = true;
  }
  int getWifiConnectionTimeout() const { return _wifiConnectionTimeout; }
  void setWifiConnectionTimeout(int t) {
    _wifiConnectionTimeout = t;
    _saveNeeded = true;
  }
  int getWifiPortalTimeout() const { return _wifiPortalTimeout; }
  void setWifiPortalTimeout(int t) {
    _wifiPortalTimeout = t;
    _saveNeeded = true;
  }
  bool getWifiScanAP() const { return _wifiScanAP; }
  void setWifiScanAP(bool t) {
    _wifiScanAP = t;
    _saveNeeded = true;
  }

  int getPushTimeout() const { return _pushTimeout; }
  void setPushTimeout(int t) { _pushTimeout = t; }

  // OtaConfig
  const char* getOtaURL() const { return _otaURL.c_str(); }
  void setOtaURL(String s) {
    _otaURL = s;
    _saveNeeded = true;
  }
  bool isOtaActive() const { return _otaURL.length() > 0 ? true : false; }
  bool isOtaSSL() const { return _otaURL.startsWith("https://"); }

  // PushConfig
  bool hasTargetHttpPost() const { return _targetHttpPost.length() ? true : false; }
  bool hasTargetHttpPost2() const { return _targetHttpPost2.length() ? true : false; }
  bool hasTargetHttpGet() const { return _targetHttpGet.length() ? true : false; }
  bool hasTargetInfluxDb2() const { return _targetInfluxDb2.length() ? true : false; }
  bool hasTargetMqtt() const { return _targetMqtt.length() ? true : false; }

  bool isHttpPostSSL() const { return _targetHttpPost.startsWith("https://"); }
  bool isHttpPost2SSL() const { return _targetHttpPost2.startsWith("https://"); }
  bool isHttpGetSSL() const { return _targetHttpGet.startsWith("https://"); }
  bool isHttpInfluxDb2SSL() const { return _targetInfluxDb2.startsWith("https://"); }
  bool isMqttSSL() const { return _portMqtt > 8000 ? true : false; }

  const char* getTargetHttpPost() const { return _targetHttpPost.c_str(); }
  void setTargetHttpPost(String target) {
    _targetHttpPost = target;
    _saveNeeded = true;
  }
  const char* getHeader1HttpPost() const { return _header1HttpPost.c_str(); }
  void setHeader1HttpPost(String header) {
    _header1HttpPost = header;
    _saveNeeded = true;
  }
  const char* getHeader2HttpPost() const { return _header2HttpPost.c_str(); }
  void setHeader2HttpPost(String header) {
    _header2HttpPost = header;
    _saveNeeded = true;
  }

  const char* getTargetHttpPost2() const { return _targetHttpPost2.c_str(); }
  void setTargetHttpPost2(String target) {
    _targetHttpPost2 = target;
    _saveNeeded = true;
  }
  const char* getHeader1HttpPost2() const { return _header1HttpPost2.c_str(); }
  void setHeader1HttpPost2(String header) {
    _header1HttpPost2 = header;
    _saveNeeded = true;
  }
  const char* getHeader2HttpPost2() const { return _header2HttpPost2.c_str(); }
  void setHeader2HttpPost2(String header) {
    _header2HttpPost2 = header;
    _saveNeeded = true;
  }

  const char* getTargetHttpGet() const { return _targetHttpGet.c_str(); }
  void setTargetHttpGet(String target) {
    _targetHttpGet = target;
    _saveNeeded = true;
  }
  const char* getHeader1HttpGet() const { return _header1HttpGet.c_str(); }
  void setHeader1HttpGet(String header) {
    _header1HttpGet = header;
    _saveNeeded = true;
  }
  const char* getHeader2HttpGet() const { return _header2HttpGet.c_str(); }
  void setHeader2HttpGet(String header) {
    _header2HttpGet = header;
    _saveNeeded = true;
  }

  const char* getTargetInfluxDB2() const { return _targetInfluxDb2.c_str(); }
  void setTargetInfluxDB2(String target) {
    _targetInfluxDb2 = target;
    _saveNeeded = true;
  }
  const char* getOrgInfluxDB2() const { return _orgInfluxDb2.c_str(); }
  void setOrgInfluxDB2(String org) {
    _orgInfluxDb2 = org;
    _saveNeeded = true;
  }
  const char* getBucketInfluxDB2() const { return _bucketInfluxDb2.c_str(); }
  void setBucketInfluxDB2(String bucket) {
    _bucketInfluxDb2 = bucket;
    _saveNeeded = true;
  }
  const char* getTokenInfluxDB2() const { return _tokenInfluxDb2.c_str(); }
  void setTokenInfluxDB2(String token) {
    _tokenInfluxDb2 = token;
    _saveNeeded = true;
  }

  const char* getTargetMqtt() const { return _targetMqtt.c_str(); }
  void setTargetMqtt(String target) {
    _targetMqtt = target;
    _saveNeeded = true;
  }
  int getPortMqtt() const { return _portMqtt; }
  void setPortMqtt(int port) {
    _portMqtt = port;
    _saveNeeded = true;
  }
  const char* getUserMqtt() const { return _userMqtt.c_str(); }
  void setUserMqtt(String user) {
    _userMqtt = user;
    _saveNeeded = true;
  }
  const char* getPassMqtt() const { return _passMqtt.c_str(); }
  void setPassMqtt(String pass) {
    _passMqtt = pass;
    _saveNeeded = true;
  }

  // Base
  const char* getID() const { return _id.c_str(); }

  char getTempFormat() const{
    return getTempUnit();
  }  // @deprecated, use setTempUnit()
  void setTempFormat(char c) {
    setTempUnit(c);
  }  // @deprecated, use setTempUnit()
  bool isTempFormatC() const {
    return isTempUnitC();
  }  // @deprecated, use isTempUnitC()
  bool isTempFormatF() const {
    return isTempUnitF();
  }  // @deprecated, use isTempUnitF()

  char getTempUnit() const { return _tempUnit; }
  void setTempUnit(char c) {
    if (c == 'C' || c == 'F') {
      _tempUnit = c;
      _saveNeeded = true;
    }
  }
  bool isTempUnitC() const { return _tempUnit == 'C' ? true : false; }
  bool isTempUnitF() const { return _tempUnit == 'F' ? true : false; }

  bool getDarkMode() const { return _darkMode; }
  void setDarkMode(bool b) {
    _darkMode = b;
    _saveNeeded = true;
  }

  // EEPROM function (Only ESP32)
#if !defined(ESP8266)
  void setPreference(const char* key, const char* value, const char* nameSpace);
  String getPreference(const char* key, const char* nameSpace);
#endif

  // Functions
  virtual void createJson(JsonObject& doc) const {}
  virtual void parseJson(JsonObject& doc) {}

  bool saveFile();
  bool loadFile();
  bool saveFileWifiOnly();
  void checkFileSystem() const;
  bool isSaveNeeded() const { return _saveNeeded; }
  void setSaveNeeded() { _saveNeeded = true; }
};

#endif  // SRC_BASECONFIG_HPP_

// EOF
