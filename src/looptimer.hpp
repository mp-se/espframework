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
#ifndef SRC_LOOPTIMER_HPP_
#define SRC_LOOPTIMER_HPP_

#include <Arduino.h>

class LoopTimer {
 private:
  uint64_t _startMillis = 0;
  uint64_t _interval = 0;
  uint64_t _loopCounter = 0;

 public:
  explicit LoopTimer(uint64_t interval) {
    _interval = interval;
    reset();
  }

  bool hasExpired() {
    if (abs(static_cast<int32_t>((millis() - _startMillis))) > _interval) {
      _loopCounter++;
      return true;
    }
    return false;
  }

  void reset() { _startMillis = millis(); }
  uint64_t getLoopCounter() const { return _loopCounter; }
  int32_t getTimePassed() const {
    return abs(static_cast<int32_t>(millis() - _startMillis));
  }
  void setInterval(uint64_t interval) { _interval = interval; }
};

#endif  // SRC_LOOPTIMER_HPP_

// EOF
