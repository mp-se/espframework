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
#include <uptime.hpp>

constexpr auto SECONDS = 1000;
constexpr auto MINUTES = 60 * 1000;
constexpr auto HOURS = 60 * 60 * 1000;
constexpr auto DAYS = 24 * 60 * 60 * 1000;

Uptime myUptime;

void Uptime::calculate() {
  uint64_t t = millis() - _startTime;

  _seconds = static_cast<int>(floor((t % MINUTES) / SECONDS));
  _minutes = static_cast<int>(floor((t % HOURS) / MINUTES));
  _hours = static_cast<int>(floor((t % DAYS) / HOURS));
  _days = static_cast<int>(floor(t / DAYS));
}

// EOF
