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
#ifndef SRC_WIFICONNECTION_HPP_
#define SRC_WIFICONNECTION_HPP_

#if defined(ESP8266)
#include <ESP8266HTTPClient.h>
#else
#include <HTTPClient.h>
#endif
#include <WiFiUdp.h>

#include <interface.hpp>

class WifiConnection {
 private:
  String _apSSID;
  String _apPWD;
  String _apMDNS;
  String _userSSID;
  String _userPWD;
  WifiConfig* _wifiConfig;

  void connectAsync(int wifiIndex);
  bool waitForConnection(int maxTime);

 public:
  WifiConnection(WifiConfig* cfg, String apSSID, String apPWD, String apMDNS,
                 String userSSID = "", String userPWD = "");
  void init();
  void timeSync();

  bool connect();
  bool disconnect();
  bool isConnected();
  bool isDoubleResetDetected();
  void stopDoubleReset();
  bool hasConfig();
  String getIPAddress();
  void startPortal();

  void loop();
};

#endif  // SRC_WIFICONNECTION_HPP_

// EOF
