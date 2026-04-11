/*
 * ESPFramework
 * Copyright (c) 2021-2026 Magnus
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 */
#ifndef SRC_INTERFACE_HPP_
#define SRC_INTERFACE_HPP_

#include <ArduinoJson.h>

class OtaConfig {
 public:
  virtual const char* getOtaURL() = 0;
  virtual void setOtaURL(String s);
  virtual bool isOtaActive();
  virtual bool isOtaSSL();
};

class WebConfig {
 public:
  virtual void createJson(JsonObject& doc);
  virtual void parseJson(JsonObject& doc);

  virtual bool saveFile();

  virtual const char* getID();
  virtual const char* getMDNS();
  virtual int getWifiPortalTimeout();

  virtual bool isCorsAllowed();
};

class WifiConfig {
 public:
  virtual int getWifiConnectionTimeout() = 0;
  virtual void setWifiConnectionTimeout(int t);
  virtual int getWifiPortalTimeout();
  virtual void setWifiPortalTimeout(int t);

  virtual bool getWifiScanAP();
  virtual void setWifiScanAP(bool b);

  virtual const char* getMDNS();
  virtual void setMDNS(String s);
  virtual const char* getWifiSSID(int idx);
  virtual void setWifiSSID(String s, int idx);
  virtual const char* getWifiPass(int idx);
  virtual void setWifiPass(String s, int idx);

  virtual const char* getWifiDirectSSID();
  virtual void setWifiDirectSSID(String s);
  virtual const char* getWifiDirectPass();
  virtual void setWifiDirectPass(String s);

  virtual bool saveFile();
};

class PushConfig {
 public:
  virtual const char* getID();
  virtual const char* getMDNS();

  virtual int getPushTimeout();
  virtual void setPushTimeout(int t);

  virtual const char* getTargetHttpPost() = 0;
  virtual void setTargetHttpPost(String target);
  virtual const char* getHeader1HttpPost();
  virtual void setHeader1HttpPost(String header);
  virtual const char* getHeader2HttpPost();
  virtual void setHeader2HttpPost(String header);
  virtual bool hasTargetHttpPost();

  virtual const char* getTargetHttpPost2() = 0;
  virtual void setTargetHttpPost2(String target);
  virtual const char* getHeader1HttpPost2();
  virtual void setHeader1HttpPost2(String header);
  virtual const char* getHeader2HttpPost2();
  virtual void setHeader2HttpPost2(String header);
  virtual bool hasTargetHttpPost2();

  virtual const char* getTargetHttpGet();
  virtual void setTargetHttpGet(String target);
  virtual const char* getHeader1HttpGet();
  virtual void setHeader1HttpGet(String header);
  virtual const char* getHeader2HttpGet();
  virtual void setHeader2HttpGet(String header);
  virtual bool hasTargetHttpGet();

  virtual const char* getTargetInfluxDB2();
  virtual void setTargetInfluxDB2(String target);
  virtual const char* getOrgInfluxDB2();
  virtual void setOrgInfluxDB2(String org);
  virtual const char* getBucketInfluxDB2();
  virtual void setBucketInfluxDB2(String bucket);
  virtual const char* getTokenInfluxDB2();
  virtual void setTokenInfluxDB2(String token);
  virtual bool hasTargetInfluxDb2();

  virtual const char* getTargetMqtt();
  virtual void setTargetMqtt(String target);
  virtual int getPortMqtt();
  virtual void setPortMqtt(int port);
  virtual const char* getUserMqtt();
  virtual void setUserMqtt(String user);
  virtual const char* getPassMqtt();
  virtual void setPassMqtt(String pass);
  virtual bool hasTargetMqtt();
};

#endif  // SRC_INTERFACE_HPP_

// EOF
