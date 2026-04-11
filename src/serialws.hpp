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
#ifndef SRC_SERIALWS_HPP_
#define SRC_SERIALWS_HPP_

#include <Print.h>

#include <espframework.hpp>

#if defined(ESP32)
#include <freertos/FreeRTOS.h>
#endif

#if defined(ESP8266)
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#else
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#endif

class SerialWebSocket : public Print {
 protected:
  AsyncWebServer *_server = 0;
  AsyncWebSocket *_webSocket = 0;
  Print *_secondayLog = 0;
  uint8_t _buf[40] = {0};
  uint32_t _bufSize = 0;

 public:
  SerialWebSocket() {}
  void begin(AsyncWebServer *_server, Print *secondary = 0);
  size_t write(uint8_t c);
  using Print::write;
  void flush();

  void loop() {
    if (_webSocket) _webSocket->cleanupClients();
  }
};

#endif  // SRC_SERIALWS_HPP_

// EOF
