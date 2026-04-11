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
#ifndef SRC_OTA_HPP_
#define SRC_OTA_HPP_

#if defined(ESP8266)
#include <ESP8266HTTPClient.h>
#else
#include <HTTPClient.h>
#endif

#include <interface.hpp>

class OtaUpdate {
 private:
  OtaConfig* _otaConfig;
  String _curVer;
  bool _newFirmware = false;

  bool parseFirmwareVersionString(int (&num)[3], const char* version);
  void downloadFile(HTTPClient& http, String& fname);

 public:
  OtaUpdate(OtaConfig* cfg, String ver);

  bool updateFirmware();
  bool checkFirmwareVersion();
};

#endif  // SRC_OTA_HPP_

// EOF
