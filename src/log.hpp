/*
 * ESPFramework
 * Copyright (c) 2021-2026 Magnus
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
#ifndef SRC_LOG_HPP_
#define SRC_LOG_HPP_

#include <ArduinoLog.h>

#include <espframework.hpp>

#define ERR_FILENAME "/error.log"
#define ERR_FILENAME2 "/error2.log"
#define ERR_FILEMAXSIZE 2048

class SerialDebug {
 public:
  explicit SerialDebug(const uint32_t serialSpeed = 115200L);
  void begin(Print* p) { getLog()->begin(LOG_LEVEL, p, true); }
  static Logging* getLog() { return &Log; }
};

void printTimestamp(Print* _logOutput, int _logLevel);
void printNewline(Print* _logOutput);

void writeErrorLog(const char* format, ...);
void dumpErrorLog1();
void dumpErrorLog2();

#if defined(USE_SERIAL_PINS) && defined(ESP32) && \
    !defined(ARDUINO_USB_CDC_ON_BOOT)
#define EspSerial Serial0
#else
#define EspSerial Serial
#if defined(USE_SERIAL_PINS)
#undef USE_SERIAL_PINS
#warning "Cannot use serial pins with ARDUINO_USB_CDC_ON_BOOT"
#endif
#endif

#endif  // SRC_LOG_HPP_

// EOF
