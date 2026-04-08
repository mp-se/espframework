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
#include <looptimer.hpp>

test(LoopTimer_ExpireAndReset) {
  LoopTimer timer(100); // 100 ms interval
  delay(120); // Wait for timer to expire
  assertTrue(timer.hasExpired());
  uint64_t v = 1;
  assertEqual(timer.getLoopCounter(), v);
  timer.reset();
  assertFalse(timer.hasExpired());
}

test(LoopTimer_IntervalChange) {
  LoopTimer timer(200);
  timer.setInterval(50);
  delay(60);
  assertTrue(timer.hasExpired());
}

test(LoopTimer_TimePassed) {
  LoopTimer timer(1000);
  delay(30);
  int32_t passed = timer.getTimePassed();
  assertTrue(passed >= 25 && passed <= 50); // Allow for some timing jitter
}
// EOF
