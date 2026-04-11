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
#include <log.hpp>
#include <serialws.hpp>

void SerialWebSocket::begin(AsyncWebServer *server, Print *secondary) {
  Log.notice(F("WS  : Starting serial websocket" CR));
  _server = server;
  _secondayLog = secondary;
  _webSocket = new AsyncWebSocket("/serialws");
  _server->addHandler(_webSocket);
}

size_t SerialWebSocket::write(uint8_t c) {
  _buf[_bufSize++] = c;

  if (_bufSize >= sizeof(_buf) || c == '\n') {
    flush();
  }

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

// EOF
