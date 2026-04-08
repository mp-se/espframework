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
