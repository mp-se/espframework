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
#ifndef SRC_ESPFRAMEWORK_HPP_
#define SRC_ESPFRAMEWORK_HPP_

#include <Arduino.h>
#include <ArduinoJson.h>

#define ESPFWK_VER "1.1"

void forcedReset();
void espReset();

#if defined(ESP8266)
#include <LittleFS.h>
#define ESP_RESET espReset
#elif defined(ESP32C3)
#include <FS.h>
#include <LittleFS.h>

#include "esp32c3/rom/rtc.h"
#define ESP_RESET forcedReset
#elif defined(ESP32S2)
#include <FS.h>
#include <LittleFS.h>

#include "esp32s2/rom/rtc.h"
#define ESP_RESET forcedReset
#elif defined(ESP32S3)
#include <FS.h>
#include <LittleFS.h>

#include "esp32s3/rom/rtc.h"
#define ESP_RESET forcedReset
#elif defined(ESP32)
#include <FS.h>
#include <LittleFS.h>

#include "esp32/rom/rtc.h"
#define ESP_RESET forcedReset
#else
#error \
    "You must define what platform is used, valid are: ESP8266, ESP32, ESP32S2, ESP32S3 or ESP32C3"
#endif

// Config
constexpr auto PARAM_ID = "id";
constexpr auto PARAM_MDNS = "mdns";
constexpr auto PARAM_CORS_ALLOWED = "cors_allowed";
constexpr auto PARAM_SSID = "wifi_ssid";
constexpr auto PARAM_PASS = "wifi_pass";
constexpr auto PARAM_SSID2 = "wifi_ssid2";
constexpr auto PARAM_PASS2 = "wifi_pass2";
constexpr auto PARAM_DIRECT_SSID = "wifi_direct_ssid";
constexpr auto PARAM_DIRECT_PASS = "wifi_direct_pass";
constexpr auto PARAM_SCAN_AP = "wifi_scan_ap";
constexpr auto PARAM_OTA_URL = "ota_url";
constexpr auto PARAM_TEMP_FORMAT = "temp_format";  // Alias is temp_unit
constexpr auto PARAM_TEMP_UNIT = "temp_unit";
constexpr auto PARAM_HTTP_POST_TARGET = "http_post_target";
constexpr auto PARAM_HTTP_POST_TCP = "http_post_tcp";
constexpr auto PARAM_HTTP_POST_HEADER1 = "http_post_header1";
constexpr auto PARAM_HTTP_POST_HEADER2 = "http_post_header2";
constexpr auto PARAM_HTTP_POST2_TARGET = "http_post2_target";
constexpr auto PARAM_HTTP_POST2_HEADER1 = "http_post2_header1";
constexpr auto PARAM_HTTP_POST2_HEADER2 = "http_post2_header2";
constexpr auto PARAM_HTTP_GET_TARGET = "http_get_target";
constexpr auto PARAM_HTTP_GET_HEADER1 = "http_get_header1";
constexpr auto PARAM_HTTP_GET_HEADER2 = "http_get_header2";
constexpr auto PARAM_INFLUXDB2_TARGET = "influxdb2_target";
constexpr auto PARAM_INFLUXDB2_ORG = "influxdb2_org";
constexpr auto PARAM_INFLUXDB2_BUCKET = "influxdb2_bucket";
constexpr auto PARAM_INFLUXDB2_TOKEN = "influxdb2_token";
constexpr auto PARAM_MQTT_TARGET = "mqtt_target";
constexpr auto PARAM_MQTT_PORT = "mqtt_port";
constexpr auto PARAM_MQTT_USER = "mqtt_user";
constexpr auto PARAM_MQTT_PASS = "mqtt_pass";
constexpr auto PARAM_WIFI_PORTAL_TIMEOUT = "wifi_portal_timeout";
constexpr auto PARAM_WIFI_CONNECT_TIMEOUT = "wifi_connect_timeout";
constexpr auto PARAM_PUSH_TIMEOUT = "push_timeout";
constexpr auto PARAM_DARK_MODE = "dark_mode";

// Webserver
constexpr auto PARAM_SUCCESS = "success";
constexpr auto PARAM_STATUS = "status";
constexpr auto PARAM_MESSAGE = "message";
constexpr auto PARAM_TOKEN = "token";
constexpr auto PARAM_COMMAND = "command";

// File system
constexpr auto PARAM_FILE = "file";
constexpr auto PARAM_SIZE = "size";
constexpr auto PARAM_TOTAL = "total";
constexpr auto PARAM_FREE = "free";
constexpr auto PARAM_USED = "used";
constexpr auto PARAM_FILES = "files";

// Wifi
constexpr auto PARAM_NETWORKS = "networks";
constexpr auto PARAM_RSSI = "rssi";
constexpr auto PARAM_CHANNEL = "channel";
constexpr auto PARAM_ENCRYPTION = "encryption";

#endif  // SRC_ESPFRAMEWORK_HPP_

// EOF
