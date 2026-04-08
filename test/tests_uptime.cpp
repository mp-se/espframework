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
#include <uptime.hpp>

test(Uptime_Initial) {
  Uptime up;
  up.calculate();
  // Immediately after creation, uptime should be very small (0 days, 0 hours, 0 minutes)
  assertEqual(up.getDays(), 0);
  assertEqual(up.getHours(), 0);
  assertEqual(up.getMinutes(), 0);
  // Seconds may be 0 or 1 depending on timing, so just check range
  assertTrue(up.getSeconds() >= 0 && up.getSeconds() < 60);
}

// Simulate passage of time by manipulating millis() is not possible in a real MCU test,
// but you can check that after a delay, the seconds increase.
test(Uptime_SecondsIncrease) {
  Uptime up;
  delay(1100); // Wait a bit more than 1 second
  up.calculate();
  assertTrue(up.getSeconds() >= 1 && up.getSeconds() < 60);
}

// EOF
