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
#ifndef SRC_SERIALWS2_HPP_
#define SRC_SERIALWS2_HPP_

#ifndef ESPFWK_DISABLE_WEBSERVER

#ifdef ESPFWK_PSYCHIC_HTTP

#include <Print.h>
#include <PsychicHttp.h>
#include <freertos/FreeRTOS.h>

#include <espframework.hpp>

class SerialWebSocket : public Print {
 protected:
  PsychicHttpServer *_server = 0;
  PsychicWebSocketHandler *_webSocket = 0;
  Print *_secondayLog = 0;
  uint8_t _buf[40] = {0};
  uint32_t _bufSize = 0;

 public:
  SerialWebSocket() {}
  void begin(PsychicHttpServer *_server, Print *secondary = 0);
  size_t write(uint8_t c);
  using Print::write;
  void flush();
  void loop() {}
};

#endif  // ESPFWK_PSYCHIC_HTTP

#endif  // ESPFWK_DISABLE_WEBSERVER

#endif  // SRC_SERIALWS2_HPP_

// EOF
