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
#ifndef SRC_LED_HPP_
#define SRC_LED_HPP_

#include <pins_arduino.h>

enum LedColor {
#if defined(RGB_BUILTIN) || defined(ESPFWK_ENABLE_RGB_LED)
  OFF = 0x000000,
  BLACK = 0x000000,
  RED = 0xff0000,
  GREEN = 0x00ff00,
  BLUE = 0x0000ff,
  CYAN = 0x00ffff,
  PURPLE = 0xff00ff,
  YELLOW = 0xffff00,
  WHITE = 0xffffff
#else
  OFF = HIGH,
  BLACK = HIGH,
  RED = 3,  // TIcker at fast pace
  GREEN = LOW,
  BLUE = 2,  // Ticker at slow pace
  PURPLE = LOW,
  CYAN = LOW,
  YELLOW = LOW,
  WHITE = LOW
#endif
};

void ledOn(LedColor l = LedColor::WHITE);
void ledOff();

#endif  // SRC_LED_HPP_

// EOF
