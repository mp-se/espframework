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
#define PARAM_TARGET_HTTP_POST "http-post-target"
#define PARAM_HEADER1_HTTP_POST "http-post-header1"
#define PARAM_HEADER2_HTTP_POST "http-post-header2"
#define PARAM_TARGET_HTTP_GET "http-get-target"
#define PARAM_HEADER1_HTTP_GET "http-get-header1"
#define PARAM_HEADER2_HTTP_GET "http-get-header2"
#define PARAM_TARGET_INFLUXDB2 "influxdb2-target"
#define PARAM_ORG_INFLUXDB2 "influxdb2-org"
#define PARAM_BUCKET_INFLUXDB2 "influxdb2-bucket"
#define PARAM_TOKEN_INFLUXDB2 "influxdb2-token"
#define PARAM_TARGET_MQTT "mqtt-target"
#define PARAM_PORT_MQTT "mqtt-port"
#define PARAM_USER_MQTT "mqtt-user"
#define PARAM_PASS_MQTT "mqtt-pass"

class BaseConfig : public WifiConfig, public OtaConfig, public WebConfig, public PushConfig {
 private:
  // WifiConfig
  String _mDNS;
  String _wifiSSID[2] = {"", ""};
  String _wifiPASS[2] = {"", ""};

  // OtaConfig
  String _otaURL;

  // PushConfig
  String _targetHttpPost;
  String _header1HttpPost = "Content-Type: application/json";
  String _header2HttpPost;
  String _targetHttpGet;
  String _header1HttpGet;
  String _header2HttpGet;
  String _targetInfluxDb2;
  String _orgInfluxDb2;
  String _bucketInfluxDb2;
  String _tokenInfluxDb2;
  String _targetMqtt;
  int _portMqtt;
  String _userMqtt;
  String _passMqtt;

  // BaseConfig
  String _id;
  char _tempFormat = 'C';

  String _fileName;

  void formatFileSystem();

  // Internal
 protected:
  bool _saveNeeded;

  void createJsonBase(DynamicJsonDocument& doc, bool skipSecrets);
  void createJsonOta(DynamicJsonDocument& doc, bool skipSecrets);
  void createJsonWifi(DynamicJsonDocument& doc, bool skipSecrets);
  void createJsonPush(DynamicJsonDocument& doc, bool skipSecrets);

  void parseJsonBase(DynamicJsonDocument& doc);
  void parseJsonOta(DynamicJsonDocument& doc);
  void parseJsonWifi(DynamicJsonDocument& doc);
  void parseJsonPush(DynamicJsonDocument& doc);

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

  // PushConfig
  const char* getTargetHttpPost() { return _targetHttpPost.c_str(); }
  void setTargetHttpPost(String target) { 
    _targetHttpPost = target; 
    _saveNeeded = true; 
  }
  const char* getHeader1HttpPost() { return _header1HttpPost.c_str(); }
  void setHeader1HttpPost(String header) { 
    _header1HttpPost = header; 
    _saveNeeded = true; 
  }
  const char* getHeader2HttpPost() { return _header2HttpPost.c_str(); }
  void setHeader2HttpPost(String header) { 
    _header2HttpPost = header; 
    _saveNeeded = true; 
  }

  const char* getTargetHttpGet() { return _targetHttpGet.c_str(); }
  void setTargetHttpGet(String target)  { 
    _targetHttpGet = target; 
    _saveNeeded = true; 
  }
  const char* getHeader1HttpGet() { return _header1HttpGet.c_str(); }
  void setHeader1HttpGet(String header) { 
    _header1HttpGet = header; 
    _saveNeeded = true; 
  }
  const char* getHeader2HttpGet() { return _header2HttpGet.c_str(); }
  void setHeader2HttpGet(String header) { 
    _header2HttpGet = header; 
    _saveNeeded = true; 
  }

  const char* getTargetInfluxDB2() { return _targetInfluxDb2.c_str(); }
  void setTargetInfluxDB2(String target)  { 
    _targetInfluxDb2 = target; 
    _saveNeeded = true; 
  }
  const char* getOrgInfluxDB2() { return _orgInfluxDb2.c_str(); }
  void setOrgInfluxDB2(String org) { 
    _orgInfluxDb2 = org; 
    _saveNeeded = true; 
  }
  const char* getBucketInfluxDB2() { return _bucketInfluxDb2.c_str(); }
  void setBucketInfluxDB2(String bucket) { 
    _bucketInfluxDb2 = bucket; 
    _saveNeeded = true; 
  }
  const char* getTokenInfluxDB2() { return _tokenInfluxDb2.c_str(); }
  void setTokenInfluxDB2(String token) { 
    _tokenInfluxDb2 = token; 
    _saveNeeded = true; 
  }

  const char* getTargetMqtt() { return _targetMqtt.c_str(); }
  void setTargetMqtt(String target) { 
    _targetMqtt = target; 
    _saveNeeded = true; 
  }
  int getPortMqtt() { return _portMqtt; }
  void setPortMqtt(int port) { 
    _portMqtt = port; 
    _saveNeeded = true; 
  }
  const char* getUserMqtt() { return _userMqtt.c_str(); }
  void setUserMqtt(String user) { 
    _userMqtt = user; 
    _saveNeeded = true; 
  }
  const char* getPassMqtt() { return _passMqtt.c_str(); }
  void setPassMqtt(String pass) { 
    _passMqtt = pass; 
    _saveNeeded = true; 
  }

  // Base
  const char* getID() { return _id.c_str(); }

  char getTempFormat() { return _tempFormat; }
  void setTempFormat(char c) {
    if (c == 'C' || c == 'F') {
      _tempFormat = c;
      _saveNeeded = true;
    }
  }
  bool isTempFormatC() { return _tempFormat == 'C' ? true : false; }
  bool isTempFormatF() { return _tempFormat == 'F' ? true : false; }

  // Functions
  virtual void createJson(DynamicJsonDocument& doc, bool skipSecrets = true) {}
  virtual void parseJson(DynamicJsonDocument& doc) {}

  bool saveFile();
  bool loadFile();
  void checkFileSystem();
  bool isSaveNeeded() { return _saveNeeded; }
};

#endif  // SRC_BASECONFIG_HPP_

// EOF
