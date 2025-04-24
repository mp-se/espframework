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
#ifndef SRC_INTERFACE_HPP_
#define SRC_INTERFACE_HPP_

#include <ArduinoJson.h>

class OtaConfigInterface {
 public:
  virtual const char* getOtaURL() const = 0;
  virtual void setOtaURL(String s);
  virtual bool isOtaActive() const;
  virtual bool isOtaSSL() const;
};

class WebConfigInterface {
 public:
  virtual void createJson(JsonObject& doc) const;
  virtual void parseJson(JsonObject& doc);

  virtual bool saveFile();

  virtual const char* getID() const;
  virtual const char* getMDNS() const;
  virtual int getWifiPortalTimeout() const;

  virtual bool isCorsAllowed() const;
};

class WifiConfigInterface {
 public:
  virtual int getWifiConnectionTimeout() const = 0;
  virtual void setWifiConnectionTimeout(int t);
  virtual int getWifiPortalTimeout() const;
  virtual void setWifiPortalTimeout(int t);

  virtual bool getWifiScanAP() const;
  virtual void setWifiScanAP(bool b);

  virtual const char* getMDNS() const;
  virtual void setMDNS(String s);
  virtual const char* getWifiSSID(int idx) const;
  virtual void setWifiSSID(String s, int idx);
  virtual const char* getWifiPass(int idx) const;
  virtual void setWifiPass(String s, int idx);

  virtual const char* getWifiDirectSSID() const;
  virtual void setWifiDirectSSID(String s);
  virtual const char* getWifiDirectPass() const;
  virtual void setWifiDirectPass(String s);

  virtual bool saveFile();
};

class PushConfigInterface {
 public:
  virtual const char* getID() const;
  virtual const char* getMDNS() const;

  virtual int getPushTimeout() const;
  virtual void setPushTimeout(int t);

  virtual const char* getTargetHttpPost()  const= 0;
  virtual void setTargetHttpPost(String target);
  virtual const char* getHeader1HttpPost() const;
  virtual void setHeader1HttpPost(String header);
  virtual const char* getHeader2HttpPost() const;
  virtual void setHeader2HttpPost(String header);
  virtual bool hasTargetHttpPost() const;

  virtual const char* getTargetHttpPost2()  const= 0;
  virtual void setTargetHttpPost2(String target);
  virtual const char* getHeader1HttpPost2() const;
  virtual void setHeader1HttpPost2(String header);
  virtual const char* getHeader2HttpPost2() const;
  virtual void setHeader2HttpPost2(String header);
  virtual bool hasTargetHttpPost2() const;

  virtual const char* getTargetHttpGet() const;
  virtual void setTargetHttpGet(String target);
  virtual const char* getHeader1HttpGet() const;
  virtual void setHeader1HttpGet(String header);
  virtual const char* getHeader2HttpGet() const;
  virtual void setHeader2HttpGet(String header);
  virtual bool hasTargetHttpGet() const;

  virtual const char* getTargetInfluxDB2() const;
  virtual void setTargetInfluxDB2(String target);
  virtual const char* getOrgInfluxDB2() const;
  virtual void setOrgInfluxDB2(String org);
  virtual const char* getBucketInfluxDB2() const;
  virtual void setBucketInfluxDB2(String bucket);
  virtual const char* getTokenInfluxDB2() const;
  virtual void setTokenInfluxDB2(String token);
  virtual bool hasTargetInfluxDb2() const;

  virtual const char* getTargetMqtt() const;
  virtual void setTargetMqtt(String target);
  virtual int getPortMqtt() const;
  virtual void setPortMqtt(int port);
  virtual const char* getUserMqtt() const;
  virtual void setUserMqtt(String user);
  virtual const char* getPassMqtt() const;
  virtual void setPassMqtt(String pass);
  virtual bool hasTargetMqtt() const;
};

#endif  // SRC_INTERFACE_HPP_

// EOF
