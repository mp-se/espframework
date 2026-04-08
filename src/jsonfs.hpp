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
#ifndef SRC_JSONFS_HPP_
#define SRC_JSONFS_HPP_

#include <Arduino.h>
#include <ArduinoJson.h>

class JsonFileSystemHelper {
 private:
  String _fileName;

 public:
  explicit JsonFileSystemHelper(const String& fileName);

  bool saveJson(JsonDocument& doc);
  bool loadJson(JsonDocument& doc);
};

#endif  // SRC_JSONFS_HPP_

// EOF
