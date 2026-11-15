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
/*
 * Validation of a firmware image header before anything is written to flash.
 *
 * Image header layout (esp-idf / esptool):
 *   byte 0      magic, always 0xE9
 *   byte 1      segment count
 *   byte 12-13  chip id (ESP32 family only, little endian)
 */
#ifndef SRC_FIRMWARE_CHECK_HPP_
#define SRC_FIRMWARE_CHECK_HPP_

#include <Arduino.h>

#define FW_IMAGE_MAGIC 0xE9
#define FW_IMAGE_MIN_HEADER 16

#if defined(ESP8266)
#define FW_EXPECTED_CHIP_ID -1  // Header has no chip id
#elif defined(CONFIG_IDF_TARGET_ESP32C3)
#define FW_EXPECTED_CHIP_ID 0x0005
#elif defined(CONFIG_IDF_TARGET_ESP32S2)
#define FW_EXPECTED_CHIP_ID 0x0002
#elif defined(CONFIG_IDF_TARGET_ESP32S3)
#define FW_EXPECTED_CHIP_ID 0x0009
#elif defined(CONFIG_IDF_TARGET_ESP32)
#define FW_EXPECTED_CHIP_ID 0x0000
#else
#define FW_EXPECTED_CHIP_ID -1  // Unknown target, only check the magic byte
#endif

// Checks the first bytes of an uploaded image. Must be called with the first
// chunk of data. Returns true if the image looks like firmware for this chip,
// otherwise false and 'error' describes why.
inline bool validateFirmwareImage(const uint8_t *data, size_t len,
                                  String &error) {
  if (len < FW_IMAGE_MIN_HEADER) {
    error = "Firmware image is too short";
    return false;
  }

  if (data[0] != FW_IMAGE_MAGIC) {
    error = "Not a firmware image (bad magic byte)";
    return false;
  }

#if FW_EXPECTED_CHIP_ID >= 0
  uint16_t chipId = data[12] | (data[13] << 8);

  if (chipId != FW_EXPECTED_CHIP_ID) {
    error = "Firmware is built for another chip (id " + String(chipId) +
            ", expected " + String(FW_EXPECTED_CHIP_ID) + ")";
    return false;
  }
#endif

  return true;
}

#endif  // SRC_FIRMWARE_CHECK_HPP_
