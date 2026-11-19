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
#ifndef SRC_OTA_GUARD_HPP_
#define SRC_OTA_GUARD_HPP_

/*
 * Application level rollback for OTA updates.
 *
 * Works without any bootloader support, so it protects devices that cannot be
 * reflashed. A running image that has not yet proven itself is "pending".
 * Every boot that does not reach markValid() counts as a failed attempt. After
 * OTA_GUARD_MAX_FAILED_BOOTS failed boots the previous known good image is
 * selected as boot partition and the device restarts.
 *
 * Usage (ESP32 only, the class does not exist on other platforms so guard the
 * instance and every call with #if defined(ESP32)):
 *   OtaGuard myOtaGuard;
 *   setup():  myOtaGuard.begin();       // as early as possible
 *   ...       myOtaGuard.markValid();   // when the firmware has proven itself
 *   loop():   myOtaGuard.loop();        // optional, validates after a delay
 *
 * Limits: only protects updates *to* an image that contains this code, and can
 * not catch images that never reach begin().
 */

#if defined(ESP32)

#define OTA_GUARD_MAX_FAILED_BOOTS 3
#define OTA_GUARD_VALIDATE_AFTER_MS 15000  // Uptime needed for loop() to validate

#include <Arduino.h>

// What could be found out about the bootloader that started this image.
struct OtaBootloaderInfo {
  // Descriptor is only present in bootloaders built with a recent IDF. When
  // false, the bootloader is older (or unknown) and the fields below are empty.
  bool hasDescriptor = false;
  uint32_t version = 0;
  String idfVersion;
  String buildDate;

  // SHA-256 digest stored at the end of the bootloader image (64 hex chars).
  // Works with any bootloader and uniquely identifies the build, compare it to
  // the digest at the end of the bootloader.bin of your releases.
  bool hasDigest = false;
  String digest;

  // OTA state of the running image, as recorded in otadata. VALID or
  // PENDING_VERIFY means a rollback capable bootloader handled the last OTA.
  // UNDEFINED/unknown tells nothing (serial flashed image or old bootloader).
  bool rollbackSeen = false;
  int otaState = -1;  // esp_ota_img_states_t, -1 = not available
};

class OtaGuard {
 private:
  bool _pending = false;  // Running image is not yet known to be good
  OtaBootloaderInfo _bootloader;

 public:
  // Call during startup to find out and log which bootloader is in use. Safe to
  // call several times. The result is also available via getBootloaderInfo().
  const OtaBootloaderInfo& checkBootloader();

  const OtaBootloaderInfo& getBootloaderInfo() const { return _bootloader; }

  // Call as early as possible in setup(). May restart into the previous image.
  void begin();

  // Call when the running image has proven itself. Cheap and safe to call
  // repeatedly, does nothing once the image is validated.
  void markValid();

  // Call from loop() in modes that have no natural validation point. Validates
  // once the uptime exceeds OTA_GUARD_VALIDATE_AFTER_MS.
  void loop();

  bool isPending() const { return _pending; }
};

#endif  // ESP32

#endif  // SRC_OTA_GUARD_HPP_
