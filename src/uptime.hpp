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
#ifndef SRC_UPTIME_HPP_
#define SRC_UPTIME_HPP_

#include <Arduino.h>

class Uptime {
 private:
  uint64_t _startTime = 0;
  int _seconds;
  int _minutes;
  int _hours;
  int _days;

 public:
  Uptime() { _startTime = millis(); }

  void calculate();

  int getSeconds() const { return _seconds; }
  int getMinutes() const { return _minutes; }
  int getHours() const { return _hours; }
  int getDays() const { return _days; }
};

extern Uptime myUptime;

#endif  // SRC_UPTIME_HPP_

// EOF
