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
#ifndef ESPFWK_DISABLE_WEBSERVER

#ifdef ESPFWK_PSYCHIC_HTTP

#include <log.hpp>
#include <serialws2.hpp>

void SerialWebSocket::begin(PsychicHttpServer *server, Print *secondary) {
  Log.notice(F("WS  : Starting serial websocket" CR));
  _server = server;
  _secondayLog = secondary;
  _webSocket = new PsychicWebSocketHandler();  
  _server->on("/serialws", _webSocket);
 
}

size_t SerialWebSocket::write(uint8_t c) {
  _buf[_bufSize++] = c;

  if (_bufSize >= (sizeof(_buf) - 1) || c == '\n') {
    flush();
  }

  return sizeof(c);
}

void SerialWebSocket::flush() {
  if (_secondayLog) _secondayLog->write(_buf, _bufSize);

  if (_webSocket && _webSocket->count()) {
    _webSocket->sendAll(HTTPD_WS_TYPE_TEXT, _buf, _bufSize);
  }

  memset(_buf, 0, sizeof(_buf));
  _bufSize = 0;
}

#endif  // ESPFWK_PSYCHIC_HTTP

#endif  // ESPFWK_DISABLE_WEBSERVER

// EOF
