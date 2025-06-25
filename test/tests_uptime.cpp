/*
MIT License

Copyright (c) 2025 Magnus

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
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
