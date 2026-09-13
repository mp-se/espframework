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
#ifndef ESPFWK_DISABLE_WEBSERVER

#ifndef ESPFWK_PSYCHIC_HTTP

#include <log.hpp>
#include <serialws.hpp>

void SerialWebSocket::begin(AsyncWebServer *server, Print *secondary) {
#if !defined(ESP8266)
  if (!_mutex) _mutex = xSemaphoreCreateMutex();
#endif

  Log.notice(F("WS  : Starting serial websocket" CR));
  _server = server;
  _secondayLog = secondary;
  _webSocket = new AsyncWebSocket("/serialws");
  _server->addHandler(_webSocket);
}

size_t SerialWebSocket::write(uint8_t c) {
  // _buf/_bufSize are shared state: write() can be called concurrently from
  // any task that logs (e.g. the Arduino loop task and application background
  // tasks), so guard every access with a mutex.
#if !defined(ESP8266)
  if (_mutex) xSemaphoreTake(_mutex, portMAX_DELAY);
#endif

  // Flush before writing if the buffer is already full, so we never index
  // past the end of _buf.
  if (_bufSize >= sizeof(_buf)) flush();

  _buf[_bufSize++] = c;

  if (_bufSize >= sizeof(_buf) || c == '\n') {
    flush();
  }

#if !defined(ESP8266)
  if (_mutex) xSemaphoreGive(_mutex);
#endif

  return sizeof(c);
}

void SerialWebSocket::flush() {
  if (_secondayLog) _secondayLog->write(_buf, _bufSize);

  if (_webSocket->count()) {
    // Only send data to socket if there are connected clients
    _webSocket->textAll(reinterpret_cast<const char *>(_buf), _bufSize);
  }

  memset(_buf, 0, sizeof(_buf));
  _bufSize = 0;
}

#endif  // !ESPFWK_PSYCHIC_HTTP

#endif  // ESPFWK_DISABLE_WEBSERVER

// EOF
