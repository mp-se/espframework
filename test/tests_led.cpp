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
#include <led.hpp>

test(Led_OnOff) {
  // These functions are hardware-dependent and do not return a value.
  // The test will pass if they do not crash or hang.
  ledOn(LedColor::WHITE);
  ledOn(LedColor::RED);
  ledOn(LedColor::GREEN);
  ledOn(LedColor::BLUE);
  ledOn(LedColor::OFF);
  ledOff();
  assertTrue(true); // If no crash, test passes
}

// EOF
