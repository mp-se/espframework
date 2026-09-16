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
#ifndef SRC_LOOPTIMER_HPP_
#define SRC_LOOPTIMER_HPP_

#include <Arduino.h>

class LoopTimer {
 private:
  uint32_t _startMillis = 0;
  uint32_t _interval = 0;
  uint64_t _loopCounter = 0;

 public:
  explicit LoopTimer(uint32_t interval) {
    _interval = interval;
    reset();
  }

  static uint32_t elapsedMillis(uint32_t start, uint32_t now) {
    return now - start;
  }

  static uint32_t elapsedMillis(uint32_t start) {
    return elapsedMillis(start, millis());
  }

  static bool hasElapsed(uint32_t start, uint32_t interval, uint32_t now) {
    return elapsedMillis(start, now) >= interval;
  }

  static bool hasElapsed(uint32_t start, uint32_t interval) {
    return hasElapsed(start, interval, millis());
  }

  bool hasExpired() {
    if (hasElapsed(_startMillis, _interval)) {
      _loopCounter++;
      return true;
    }
    return false;
  }

  void reset() { _startMillis = millis(); }
  uint64_t getLoopCounter() const { return _loopCounter; }
  uint32_t getTimePassed() const {
    return elapsedMillis(_startMillis);
  }
  void setInterval(uint32_t interval) { _interval = interval; }
};

#endif  // SRC_LOOPTIMER_HPP_

// EOF
