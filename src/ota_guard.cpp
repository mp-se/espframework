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
#if defined(ESP32)

#include <esp_ota_ops.h>
#include <esp_partition.h>
#include <esp_system.h>
#include <esp_timer.h>

#include <esp_flash.h>
#include <esp_flash_partitions.h>

#if __has_include(<esp_bootloader_desc.h>)
#include <esp_bootloader_desc.h>
#define OTAG_HAS_BOOTLOADER_DESC 1
#endif

#include <Preferences.h>
#include <log.hpp>
#include <ota_guard.hpp>

#define OTAG_NVS_NAMESPACE "otaguard"
#define OTAG_KEY_SHA "sha"      // sha256 of last known good image
#define OTAG_KEY_SLOT "slot"    // partition label it runs from
#define OTAG_KEY_BOOTS "boots"  // failed boots of pending image
#define OTAG_SHA_LEN 32

static esp_timer_handle_t otagHangTimer = NULL;

static void otagHangCallback(void*) {
  Log.error(F("OTAG: Image did not validate in time, restarting." CR));
  esp_restart();
}

static void otagStopHangTimer() {
  if (!otagHangTimer) return;
  esp_timer_stop(otagHangTimer);
  esp_timer_delete(otagHangTimer);
  otagHangTimer = NULL;
}

static void otagStartHangTimer() {
  esp_timer_create_args_t args = {};
  args.callback = otagHangCallback;
  args.name = "otaguard";

  if (esp_timer_create(&args, &otagHangTimer) != ESP_OK) {
    otagHangTimer = NULL;
    return;
  }
  esp_timer_start_once(otagHangTimer, OTA_GUARD_HANG_TIMEOUT_MS * 1000ULL);
}

static bool otagSameSha(const uint8_t* a, const uint8_t* b) {
  return memcmp(a, b, OTAG_SHA_LEN) == 0;
}

// The bootloader image ends with a SHA-256 digest (hash_appended flag in the
// image header). Walk the segments to find it, no hashing needed.
static bool otagReadBootloaderDigest(String& hex) {
  uint8_t hdr[24];
  uint32_t pos = ESP_BOOTLOADER_OFFSET;

  if (esp_flash_read(NULL, hdr, pos, sizeof(hdr)) != ESP_OK ||
      hdr[0] != 0xE9 || hdr[1] > 16 || hdr[23] != 1)
    return false;

  pos += sizeof(hdr);

  for (int i = 0; i < hdr[1]; i++) {
    uint32_t seg[2];  // load address, data length

    if (esp_flash_read(NULL, seg, pos, sizeof(seg)) != ESP_OK ||
        seg[1] > 0x10000)
      return false;

    pos += sizeof(seg) + seg[1];
  }

  // Checksum byte is the last byte of the 16 byte aligned block, digest follows
  pos = (pos - ESP_BOOTLOADER_OFFSET + 1 + 15) & ~15u;
  pos += ESP_BOOTLOADER_OFFSET;

  uint8_t digest[OTAG_SHA_LEN];
  if (esp_flash_read(NULL, digest, pos, sizeof(digest)) != ESP_OK) return false;

  hex = "";
  for (size_t i = 0; i < sizeof(digest); i++) {
    char buf[3];
    snprintf(buf, sizeof(buf), "%02x", digest[i]);
    hex += buf;
  }
  return true;
}

// Switch to the recorded known good image. Returns false if there is none or
// if it has been overwritten since (checked by comparing the image hash).
static bool otagRollback(Preferences& prefs, const esp_partition_t* running,
                         const uint8_t* goodSha) {
  String slot = prefs.getString(OTAG_KEY_SLOT, "");
  if (slot.isEmpty()) return false;

  const esp_partition_t* target = esp_partition_find_first(
      ESP_PARTITION_TYPE_APP, ESP_PARTITION_SUBTYPE_ANY, slot.c_str());
  esp_app_desc_t desc;

  if (!target || target == running ||
      esp_ota_get_partition_description(target, &desc) != ESP_OK ||
      !otagSameSha(desc.app_elf_sha256, goodSha)) {
    return false;
  }

  return esp_ota_set_boot_partition(target) == ESP_OK;
}

const OtaBootloaderInfo& OtaGuard::checkBootloader() {
  _bootloader = OtaBootloaderInfo();

#if defined(OTAG_HAS_BOOTLOADER_DESC)
  const esp_bootloader_desc_t* desc = esp_bootloader_get_description();

  if (desc && desc->magic_byte == ESP_BOOTLOADER_DESC_MAGIC_BYTE) {
    _bootloader.hasDescriptor = true;
    _bootloader.version = desc->version;
    // Fields are fixed size, make sure they are terminated.
    _bootloader.idfVersion = String(desc->idf_ver).substring(0, 31);
    _bootloader.buildDate = String(desc->date_time).substring(0, 23);
  }
#endif

  _bootloader.hasDigest = otagReadBootloaderDigest(_bootloader.digest);

  const esp_partition_t* running = esp_ota_get_running_partition();
  esp_ota_img_states_t state;

  if (running && esp_ota_get_state_partition(running, &state) == ESP_OK) {
    _bootloader.otaState = state;
    _bootloader.rollbackSeen = state == ESP_OTA_IMG_VALID ||
                               state == ESP_OTA_IMG_PENDING_VERIFY;
  }

  if (_bootloader.hasDescriptor) {
    Log.notice(F("OTAG: Bootloader version %u, idf %s, built %s." CR),
               _bootloader.version, _bootloader.idfVersion.c_str(),
               _bootloader.buildDate.c_str());
  } else {
    Log.notice(F("OTAG: Bootloader has no descriptor (old IDF or bootloader)." CR));
  }

  if (_bootloader.hasDigest)
    Log.notice(F("OTAG: Bootloader digest %s." CR), _bootloader.digest.c_str());

  Log.notice(F("OTAG: Image OTA state %d, rollback capable bootloader %s." CR),
             _bootloader.otaState,
             _bootloader.rollbackSeen ? "confirmed" : "not confirmed");
  return _bootloader;
}

void OtaGuard::begin() {
  const esp_partition_t* running = esp_ota_get_running_partition();
  esp_app_desc_t desc;

  if (!running || esp_ota_get_partition_description(running, &desc) != ESP_OK)
    return;

  Preferences prefs;
  if (!prefs.begin(OTAG_NVS_NAMESPACE, false)) return;

  uint8_t good[OTAG_SHA_LEN] = {0};
  bool hasGood =
      prefs.getBytes(OTAG_KEY_SHA, good, OTAG_SHA_LEN) == OTAG_SHA_LEN;

  if (hasGood && otagSameSha(good, desc.app_elf_sha256)) {
    prefs.end();
    return;  // Known good, nothing to do
  }

  _pending = true;
  uint8_t boots = prefs.getUChar(OTAG_KEY_BOOTS, 0) + 1;

  if (boots > OTA_GUARD_MAX_FAILED_BOOTS && hasGood) {
    Log.error(F("OTAG: Image failed to validate in %d boots, rolling back." CR),
              OTA_GUARD_MAX_FAILED_BOOTS);

    if (otagRollback(prefs, running, good)) {
      prefs.putUChar(OTAG_KEY_BOOTS, 0);
      prefs.end();
      delay(100);
      ESP.restart();
      return;
    }
    Log.error(F("OTAG: No valid previous image to roll back to." CR));
  }

  // Stop counting once past the limit (no rollback target), saves flash wear.
  if (boots <= OTA_GUARD_MAX_FAILED_BOOTS + 1)
    prefs.putUChar(OTAG_KEY_BOOTS, boots);

  Log.notice(F("OTAG: New image pending validation (boot %d)." CR), boots);
  prefs.end();

  // Without a fallback a restart would not help, so only arm with one.
  if (hasGood) otagStartHangTimer();
}

void OtaGuard::markValid() {
  if (!_pending) return;

  const esp_partition_t* running = esp_ota_get_running_partition();
  esp_app_desc_t desc;

  if (!running || esp_ota_get_partition_description(running, &desc) != ESP_OK)
    return;

  Preferences prefs;
  if (!prefs.begin(OTAG_NVS_NAMESPACE, false)) return;

  prefs.putBytes(OTAG_KEY_SHA, desc.app_elf_sha256, OTAG_SHA_LEN);
  prefs.putString(OTAG_KEY_SLOT, running->label);
  prefs.putUChar(OTAG_KEY_BOOTS, 0);
  prefs.end();
  _pending = false;
  otagStopHangTimer();

  // Only has an effect if the bootloader was built with rollback support.
  esp_ota_mark_app_valid_cancel_rollback();
  Log.notice(F("OTAG: Image validated." CR));
}

void OtaGuard::loop() {
  if (_pending && millis() > OTA_GUARD_VALIDATE_AFTER_MS) markValid();
}

#endif  // ESP32
