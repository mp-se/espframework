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
#include <AUnit.h>
#include <firmware_check.hpp>

static void makeHeader(uint8_t *h, uint16_t chipId) {
  memset(h, 0, FW_IMAGE_MIN_HEADER);
  h[0] = FW_IMAGE_MAGIC;
  h[12] = chipId & 0xff;
  h[13] = chipId >> 8;
}

test(FirmwareCheck_ValidImage) {
  uint8_t h[FW_IMAGE_MIN_HEADER];
#if FW_EXPECTED_CHIP_ID >= 0
  makeHeader(h, FW_EXPECTED_CHIP_ID);
#else
  makeHeader(h, 0);
#endif
  String err;
  assertTrue(validateFirmwareImage(h, sizeof(h), err));
}

test(FirmwareCheck_BadMagic) {
  uint8_t h[FW_IMAGE_MIN_HEADER];
  makeHeader(h, 0);
  h[0] = 0x00;
  String err;
  assertFalse(validateFirmwareImage(h, sizeof(h), err));
  assertTrue(err.length() > 0);
}

test(FirmwareCheck_TooShort) {
  uint8_t h[FW_IMAGE_MIN_HEADER];
  makeHeader(h, 0);
  String err;
  assertFalse(validateFirmwareImage(h, FW_IMAGE_MIN_HEADER - 1, err));
}

#if FW_EXPECTED_CHIP_ID >= 0
test(FirmwareCheck_WrongChip) {
  uint8_t h[FW_IMAGE_MIN_HEADER];
  makeHeader(h, FW_EXPECTED_CHIP_ID + 1);
  String err;
  assertFalse(validateFirmwareImage(h, sizeof(h), err));
}
#endif
