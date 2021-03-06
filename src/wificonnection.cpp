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
#include <ESP8266WiFi.h>

#include <espframework.hpp>
#include <wificonnection.hpp>

// Settings for DRD
#define ESP_DRD_USE_LITTLEFS true
#define ESP_DRD_USE_SPIFFS false
#define ESP_DRD_USE_EEPROM false
#include <ESP_DoubleResetDetector.h>
#define DRD_TIMEOUT 3
#define DRD_ADDRESS 0

// Settings for WIFI Manager
#define USE_ESP_WIFIMANAGER_NTP false
#define USE_CLOUDFLARE_NTP false
#define USING_CORS_FEATURE false
#define NUM_WIFI_CREDENTIALS 1
#define USE_STATIC_IP_CONFIG_IN_CP false
#define _WIFIMGR_LOGLEVEL_ 3
#include <ESP_WiFiManager.h>
ESP_WiFiManager *myWifiManager;
DoubleResetDetector *myDRD;

WifiConnection::WifiConnection(WifiConfig *cfg, String apSSID, String apPWD,
                               String apMDNS, String userSSID, String userPWD) {
  _wifiConfig = cfg;
  _apSSID = apSSID;
  _apPWD = apPWD;
  _apMDNS = apMDNS;
  _userSSID = userSSID;
  _userPWD = userPWD;
}

void WifiConnection::init() {
  myDRD = new DoubleResetDetector(DRD_TIMEOUT, DRD_ADDRESS);
}

bool WifiConnection::hasConfig() {
  if (strlen(_wifiConfig->getWifiSSID(0))) return true;
  if (_userSSID.length()) return true;

  // Check if there are stored WIFI Settings we can use (only works for
  // esp8266).
  String ssid = WiFi.SSID();
  if (ssid.length()) {
    Log.notice(F("WIFI: Found credentials in EEPORM." CR));
    _wifiConfig->setWifiSSID(ssid, 0);

    if (WiFi.psk().length()) _wifiConfig->setWifiPass(WiFi.psk(), 0);

    _wifiConfig->saveFile();
    return true;
  }
  return false;
}

bool WifiConnection::isConnected() { return WiFi.status() == WL_CONNECTED; }

String WifiConnection::getIPAddress() { return WiFi.localIP().toString(); }

bool WifiConnection::isDoubleResetDetected() {
  if (_userSSID.length())
    return false;  // Ignore this if we have hardcoded settings.
  return myDRD->detectDoubleReset();
}

void WifiConnection::stopDoubleReset() { myDRD->stop(); }

void WifiConnection::startPortal() {
  Log.notice(F("WIFI: Starting Wifi config portal." CR));

  pinMode(PIN_LED, OUTPUT);
  digitalWrite(PIN_LED, LOW);

  myWifiManager = new ESP_WiFiManager(_apMDNS.c_str());
  myWifiManager->setMinimumSignalQuality(-1);
  myWifiManager->setConfigPortalChannel(0);
  myWifiManager->setConfigPortalTimeout(_wifiConfig->getWifiPortalTimeout());

  String mdns("<p>Default mDNS name is: http://");
  mdns += _wifiConfig->getMDNS();
  mdns += ".local<p>";
  ESP_WMParameter deviceName(mdns.c_str());
  myWifiManager->addParameter(&deviceName);

  myWifiManager->startConfigPortal(_apSSID.c_str(), _apPWD.c_str());

  if (myWifiManager->getSSID(0).length()) {
#if LOG_LEVEL == 6
    Log.verbose(
        F("WIFI: Saving=%s:%s, %s:%s." CR), myWifiManager->getSSID(0).c_str(),
        myWifiManager->getPW(0).c_str(), myWifiManager->getSSID(1).c_str(),
        myWifiManager->getPW(1).c_str());
#endif
    _wifiConfig->setWifiSSID(myWifiManager->getSSID(0), 0);
    _wifiConfig->setWifiPass(myWifiManager->getPW(0), 0);
    _wifiConfig->setWifiSSID(myWifiManager->getSSID(1), 1);
    _wifiConfig->setWifiPass(myWifiManager->getPW(1), 1);

    // If the same SSID has been used for both entire, lets delete the second
    if (!strcmp(_wifiConfig->getWifiSSID(0), _wifiConfig->getWifiSSID(1))) {
#if LOG_LEVEL == 6
      Log.verbose(
          F("WIFI: Both SSID are the same, setting second SSID to blank." CR));
#endif
      _wifiConfig->setWifiSSID("", 1);
      _wifiConfig->setWifiPass("", 1);
    }

    Log.notice(F("WIFI: Stored SSID1:'%s' SSID2:'%s'" CR),
               _wifiConfig->getWifiSSID(0), _wifiConfig->getWifiSSID(1));
    _wifiConfig->saveFile();
  } else {
    Log.notice(
        F("WIFI: Could not find first SSID so assuming we got a timeout." CR));
  }

  Log.notice(F("WIFI: Exited wifi config portal. Rebooting..." CR));
  stopDoubleReset();
  delay(500);
  ESP_RESET();
}

void WifiConnection::loop() { myDRD->loop(); }

void WifiConnection::connectAsync(int wifiIndex) {
  WiFi.persistent(true);
  WiFi.mode(WIFI_STA);
  if (_userSSID.length()) {
    Log.notice(F("WIFI: Connecting to wifi using hardcoded settings %s." CR),
               _userSSID.c_str());
    WiFi.begin(_userSSID, _userPWD);
  } else {
    Log.notice(F("WIFI: Connecting to wifi (%d) using stored settings %s." CR),
               wifiIndex, _wifiConfig->getWifiSSID(wifiIndex));
    WiFi.begin(_wifiConfig->getWifiSSID(wifiIndex),
               _wifiConfig->getWifiPass(wifiIndex));
  }
}

bool WifiConnection::waitForConnection(int maxTime) {
#if LOG_LEVEL == 6
  WiFi.printDiag(Serial);
#endif
  int i = 0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(100);

    if (i % 10) Serial.print(".");

    if (i++ > (maxTime * 10)) {
      Log.error(F("WIFI: Failed to connect to wifi %d" CR), WiFi.status());
      WiFi.disconnect();
      Serial.print(CR);
      return false;  // Return to main that we have failed to connect.
    }
  }
  Serial.print(CR);
  Log.notice(F("WIFI: Connected to wifi %s ip=%s." CR), WiFi.SSID().c_str(),
             getIPAddress().c_str());
  Log.notice(F("WIFI: Using mDNS name %s." CR), _wifiConfig->getMDNS());
  return true;
}

bool WifiConnection::connect() {
  int timeout = _wifiConfig->getWifiConnectionTimeout();

  connectAsync(0);
  if (!waitForConnection(timeout)) {
    Log.warning(F("WIFI: Failed to connect to first SSID %s." CR),
                _wifiConfig->getWifiSSID(0));

    if (strlen(_wifiConfig->getWifiSSID(1))) {
      connectAsync(1);

      if (waitForConnection(timeout)) {
        Log.notice(F("WIFI: Connected to second SSID %s." CR),
                   _wifiConfig->getWifiSSID(1));
        return true;
      }
    }

    Log.warning(F("WIFI: Failed to connect to any SSID." CR));
    return false;
  }

  return true;
}

bool WifiConnection::disconnect() {
  Log.notice(F("WIFI: Erasing stored WIFI credentials." CR));
  return WiFi.disconnect(true);  // Erase WIFI credentials
}

// EOF
