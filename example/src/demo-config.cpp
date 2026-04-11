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
#include <ArduinoJson.h>

#include <demo-config.hpp>
#include <espframework.hpp>

DemoConfig::DemoConfig(String baseMDNS, String fileName)
    : BaseConfig(baseMDNS, fileName, JSON_BUFFER_SIZE_L) {}

void DemoConfig::createJson(JsonObject& doc) {
  // Call base class functions
  createJsonBase(doc);
  createJsonWifi(doc);
  createJsonOta(doc);
  createJsonPush(doc);

  // Handle project specific config
}

void DemoConfig::parseJson(JsonObject& doc) {
  // Call base class functions
  parseJsonBase(doc);
  parseJsonWifi(doc);
  parseJsonOta(doc);
  parseJsonPush(doc);

  // Handle project specific config
}

// EOF
