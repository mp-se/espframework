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
  virtual void createJson(DynamicJsonDocument& doc) = 0;
  virtual void parseJson(DynamicJsonDocument& doc);

  virtual bool saveFile();

  virtual const char* getID();
  virtual const char* getMDNS();
};

class WifiConfig {
 public:
  virtual int getWifiConnectionTimeout() = 0;
  virtual int getWifiPortalTimeout();

  virtual const char* getMDNS();
  virtual void setMDNS(String s);
  virtual const char* getWifiSSID(int idx);
  virtual void setWifiSSID(String s, int idx);
  virtual const char* getWifiPass(int idx);
  virtual void setWifiPass(String s, int idx);

  virtual bool saveFile();
};

#endif  // SRC_INTERFACE_HPP_

// EOF
