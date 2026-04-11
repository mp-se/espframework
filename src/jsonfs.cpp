/*
 * ESPFramework
 * Copyright (c) 2025-2026 Magnus
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
#include <LittleFS.h>

#include <jsonfs.hpp>
#include <log.hpp>

JsonFileSystemHelper::JsonFileSystemHelper(const String& fileName) {
  _fileName = fileName;
}

bool JsonFileSystemHelper::saveJson(JsonDocument& doc) {
  File jsonFile = LittleFS.open(_fileName.c_str(), "w");

  if (!jsonFile) {
    Log.error(F("JSFS: Failed to open file %s for save." CR),
              _fileName.c_str());
    return false;
  }

  // #if LOG_LEVEL == 6
  //   serializeJson(doc, EspSerial);
  //   EspSerial.print(CR);
  // #endif
  serializeJson(doc, jsonFile);

  if (serializeJson(doc, jsonFile) == 0) {
    Log.error(F("Failed to write to file" CR));
    jsonFile.close();
    return false;
  }

  jsonFile.flush();
  jsonFile.close();
  Log.notice(F("CFG : Data saved to file." CR));
  return true;
}

bool JsonFileSystemHelper::loadJson(JsonDocument& doc) {
  Log.verbose(F("CFG : Loading data from file %s." CR), _fileName.c_str());

  if (!LittleFS.exists(_fileName.c_str())) {
    Log.error(F("CFG : Configuration file %s does not exist." CR),
              _fileName.c_str());
    return false;
  }

  File jsonFile = LittleFS.open(_fileName.c_str(), "r");

  if (!jsonFile) {
    Log.error(F("CFG : Failed to open %s." CR), _fileName.c_str());
    return false;
  }

  DeserializationError err = deserializeJson(doc, jsonFile);
  // #if LOG_LEVEL == 6
  //   serializeJson(doc, EspSerial);
  //   EspSerial.print(CR);
  // #endif
  jsonFile.close();

  if (err) {
    Log.error(F("CFG : Failed to parse file, Err: %s." CR), err.c_str());
    return false;
  }

  Log.notice(F("CFG : Json file %s loaded." CR), _fileName.c_str());
  return true;
}

// EOF
