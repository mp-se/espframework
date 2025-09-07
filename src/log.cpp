/*
MIT License

Copyright (c) 2021-2025 Magnus

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
#include <HardwareSerial.h>

#include <cstdio>
#include <ctime>
#include <sys/time.h>
#include <log.hpp>

void writeErrorLog(const char *format, ...) {
  File f = LittleFS.open(ERR_FILENAME, "a");

  if (f && f.size() > ERR_FILEMAXSIZE) {
    f.close();
    LittleFS.remove(ERR_FILENAME2);
    LittleFS.rename(ERR_FILENAME, ERR_FILENAME2);
    f = LittleFS.open(ERR_FILENAME, "a");
  }

  if (f) {
    va_list arg;
    va_start(arg, format);
    char buf[80];
    vsnprintf(&buf[0], sizeof(buf), format, arg);
    f.write(reinterpret_cast<unsigned char *>(&buf[0]), strlen(&buf[0]));
    va_end(arg);
    f.println();
    f.close();
  }
}

void dumpErrorLog(const char *fname) {
  File f = LittleFS.open(fname, "r");

  if (f) {
    String s;
    do {
      s = f.readString();
      EspSerial.print(s.c_str());
    } while (s.length());
    f.close();
  }
  LittleFS.remove(fname);
}

void dumpErrorLog1() { dumpErrorLog(ERR_FILENAME); }

void dumpErrorLog2() { dumpErrorLog(ERR_FILENAME2); }

SerialDebug::SerialDebug(const uint32_t serialSpeed, bool autoBegin, uint8_t tx,
                         uint8_t rx) {
  setup(serialSpeed, tx, rx);

  EspSerial.println("Serial console activated.");

  if (autoBegin) {
    begin(&EspSerial);
  }
}

void SerialDebug::setup(const uint32_t serialSpeed, uint8_t tx, uint8_t rx) {
  _serialSpeed = serialSpeed;

#if defined(ESPFWK_USE_SERIAL_PINS) && !defined(ESP8266)
  if (tx == -1) tx = TX;

  if (rx == -1) rx = RX;

  EspSerial.begin(serialSpeed, SERIAL_8N1, rx, tx, false);
#elif defined(ESPFWK_USE_SERIAL_PINS) && defined(ESP8266)
  if (tx == -1) tx = TX;

  EspSerial.begin(serialSpeed, SERIAL_8N1, tx);
#else
  EspSerial.begin(serialSpeed);
#endif
}

void SerialDebug::begin(Print *p) {
  getLog()->begin(LOG_LEVEL, p, true);
  getLog()->setPrefix(printTimestamp);
  getLog()->notice(F("SDBG: Serial logging started at %d." CR), _serialSpeed);
}

void printTimestamp(Print *_logOutput, int _logLevel) {
#if !defined(ESPFWK_DISABLE_LOG_TIMESTAMP)
  // Prefer human-readable local date/time once RTC/NTP has set the clock.
  // Simple rule: if calendar year >= 2000, assume valid time.
  struct timeval tv;
  gettimeofday(&tv, nullptr);
  struct tm t;
  if (localtime_r(&tv.tv_sec, &t) && (t.tm_year + 1900) >= 2000) {
    char ts[24];  // "YYYY-MM-DD HH:MM:SS:mmm"
    if (strftime(ts, sizeof(ts), "%Y-%m-%d %H:%M:%S", &t)) {
      _logOutput->print(ts);
      _logOutput->print('.');

      unsigned int msec = static_cast<unsigned int>(tv.tv_usec / 1000UL);
      char ms[4];
      ms[0] = '0' + (msec / 100);
      ms[1] = '0' + ((msec / 10) % 10);
      ms[2] = '0' + (msec % 10);
      ms[3] = '\0';
      _logOutput->print(ms);
      _logOutput->print(' ');
      return;
    }
  }
#endif // !ESPFWK_DISABLE_LOG_TIMESTAMP

  // Fallback: print uptime millis when real time is not available.
  char c[16];
  snprintf(c, sizeof(c), "%10lu ", millis());
  _logOutput->print(c);
}

// EOF
