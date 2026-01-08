/*
MIT License

Copyright (c) 2021-2025 Magnus

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
#ifndef SRC_INTERFACE_HPP_
#define SRC_INTERFACE_HPP_

#include <ArduinoJson.h>

class OtaConfigInterface {
 public:
  OtaConfigInterface() = default;
  virtual ~OtaConfigInterface() = default;

  virtual const char* getOtaURL() const = 0;
  virtual void setOtaURL(String s) = 0;
  virtual bool isOtaActive() const = 0;
  virtual bool isOtaSSL() const = 0;
};

class WebConfigInterface {
 public:
  WebConfigInterface() = default;
  virtual ~WebConfigInterface() {}

  virtual void createJson(JsonObject& doc) const = 0;
  virtual void parseJson(JsonObject& doc) = 0;

  virtual bool saveFile() = 0;

  virtual const char* getID() const = 0;
  virtual const char* getMDNS() const = 0;
  virtual int getWifiPortalTimeout() const = 0;

  virtual const char* getAdminUser() const = 0;
  virtual const char* getAdminPass() const = 0;

  virtual bool isCorsAllowed() const = 0;
};

class WifiConfigInterface {
 public:
  WifiConfigInterface() = default;
  virtual ~WifiConfigInterface() {}

  virtual int getWifiConnectionTimeout() const = 0;
  virtual void setWifiConnectionTimeout(int t) = 0;
  virtual int getWifiPortalTimeout() const = 0;
  virtual void setWifiPortalTimeout(int t) = 0;

  virtual bool getWifiScanAP() const = 0;
  virtual void setWifiScanAP(bool b) = 0;

  virtual const char* getMDNS() const = 0;
  virtual void setMDNS(String s) = 0;
  virtual const char* getWifiSSID(int idx) const = 0;
  virtual void setWifiSSID(String s, int idx) = 0;
  virtual const char* getWifiPass(int idx) const = 0;
  virtual void setWifiPass(String s, int idx) = 0;

  virtual const char* getWifiDirectSSID() const = 0;
  virtual void setWifiDirectSSID(String s) = 0;
  virtual const char* getWifiDirectPass() const = 0;
  virtual void setWifiDirectPass(String s) = 0;

  virtual bool saveFile() = 0;
};

class PushConfigInterface {
 public:
  PushConfigInterface() = default;
  virtual ~PushConfigInterface() {}

  virtual const char* getID() const = 0;
  virtual const char* getMDNS() const = 0;

  virtual int getPushTimeout() const = 0;
  virtual void setPushTimeout(int t) = 0;

  virtual const char* getTargetHttpPost() const = 0;
  virtual void setTargetHttpPost(String target) = 0;
  virtual const char* getHeader1HttpPost() const = 0;
  virtual void setHeader1HttpPost(String header) = 0;
  virtual const char* getHeader2HttpPost() const = 0;
  virtual void setHeader2HttpPost(String header) = 0;
  virtual bool hasTargetHttpPost() const = 0;
  virtual void setTcpHttpPost(bool tcp) = 0;
  virtual bool getTcpHttpPost() const = 0;

  virtual const char* getTargetHttpPost2() const = 0;
  virtual void setTargetHttpPost2(String target) = 0;
  virtual const char* getHeader1HttpPost2() const = 0;
  virtual void setHeader1HttpPost2(String header) = 0;
  virtual const char* getHeader2HttpPost2() const = 0;
  virtual void setHeader2HttpPost2(String header) = 0;
  virtual bool hasTargetHttpPost2() const = 0;

  virtual const char* getTargetHttpGet() const = 0;
  virtual void setTargetHttpGet(String target) = 0;
  virtual const char* getHeader1HttpGet() const = 0;
  virtual void setHeader1HttpGet(String header) = 0;
  virtual const char* getHeader2HttpGet() const = 0;
  virtual void setHeader2HttpGet(String header) = 0;
  virtual bool hasTargetHttpGet() const = 0;

  virtual const char* getTargetInfluxDB2() const = 0;
  virtual void setTargetInfluxDB2(String target) = 0;
  virtual const char* getOrgInfluxDB2() const = 0;
  virtual void setOrgInfluxDB2(String org) = 0;
  virtual const char* getBucketInfluxDB2() const = 0;
  virtual void setBucketInfluxDB2(String bucket) = 0;
  virtual const char* getTokenInfluxDB2() const = 0;
  virtual void setTokenInfluxDB2(String token) = 0;
  virtual bool hasTargetInfluxDb2() const = 0;

  virtual const char* getTargetMqtt() const = 0;
  virtual void setTargetMqtt(String target) = 0;
  virtual int getPortMqtt() const = 0;
  virtual void setPortMqtt(int port) = 0;
  virtual const char* getUserMqtt() const = 0;
  virtual void setUserMqtt(String user) = 0;
  virtual const char* getPassMqtt() const = 0;
  virtual void setPassMqtt(String pass) = 0;
  virtual bool hasTargetMqtt() const = 0;
};

#endif  // SRC_INTERFACE_HPP_

// EOF
