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
#if defined(PERF_ENABLE)

#include <espframework.hpp>
#include <log.hpp>
#include <perf.hpp>
#include <utils.hpp>

#if defined(ESP8266)
#include <ESP8266HTTPClient.h>
#else
#include <HTTPClient.h>
#endif

PerfLogging gblPerfLogging;

void PerfLogging::clear() {
  if (first == 0) return;

  PerfEntry* pe = first;

  do {
    pe->max = 0;
    pe->start = 0;
    pe->end = 0;
    pe->mA = 0;
    pe->V = 0;
    pe = pe->next;
  } while (pe != 0);
}

void PerfLogging::start(const char* key) {
  PerfEntry* pe = add(key);
  pe->start = millis();
}

void PerfLogging::stop(const char* key) {
  PerfEntry* pe = find(key);

  if (pe != 0) {
    pe->end = millis();

    uint32_t t = pe->end - pe->start;

    if (t > pe->max) pe->max = t;
  }
}

void PerfLogging::print() {
  PerfEntry* pe = first;

  while (pe != 0) {
    Log.notice(F("PERF: %s %ums" CR), pe->key, pe->max);
    pe = pe->next;
  }
}

void PerfLogging::pushInflux() {
  if (strlen(PERF_INFLUX_TARGET) == 0) return;

  if (_config == 0) return;

  WiFiClient wifi;
  HTTPClient http;
  String serverPath = String(PERF_INFLUX_TARGET) +
                      "/api/v2/write?org=" + String(PERF_INFLUX_ORG) +
                      "&bucket=" + String(PERF_INFLUX_BUCKET);

  http.begin(wifi, serverPath);

  String body;
  body.reserve(500);

  PerfEntry* pe = first;
  char buf[150];
  snprintf(&buf[0], sizeof(buf), "perf,host=%s,device=%s ", _config->getMDNS(),
           _config->getID());
  body += &buf[0];

  while (pe != 0) {
    if (pe->max) {
      if (pe->next)
        snprintf(&buf[0], sizeof(buf), "%s=%u,", pe->key, pe->max);
      else
        snprintf(&buf[0], sizeof(buf), "%s=%u", pe->key, pe->max);

      body += &buf[0];
    }
    pe = pe->next;
  }

  snprintf(&buf[0], sizeof(buf), "\ndebug,host=%s,device=%s ",
           _config->getMDNS(), _config->getID());
  body += &buf[0];
#if defined(ESP8266)
  snprintf(&buf[0], sizeof(buf), "heap=%d,heap-frag=%d,heap-max=%d,stack=%d",
           ESP.getFreeHeap(), ESP.getHeapFragmentation(),
           ESP.getMaxFreeBlockSize(), ESP.getFreeContStack());
#else  // defined (ESP32)
  snprintf(&buf[0], sizeof(buf), "heap=%d,heap-frag=%d,heap-max=%d",
           ESP.getFreeHeap(), 0, ESP.getMaxAllocHeap());
#endif

  body += &buf[0];

#if LOG_LEVEL == 6
  Log.verbose(F("PERF: url %s." CR), serverPath.c_str());
  Log.verbose(F("PERF: data %s." CR), body.c_str());
#endif

  String auth = "Token " + String(PERF_INFLUX_TOKEN);
  http.addHeader(F("Authorization"), auth.c_str());
  http.setTimeout(_config->getPushTimeout() * 1000);
  int httpResponseCode = http.POST(body);

  if (httpResponseCode == 204) {
    // No need to report success.
  } else {
    Log.error(F("PERF: InfluxDB2 push performance data failed, response=%d" CR),
              httpResponseCode);
  }

  http.end();
  wifi.stop();
  tcp_cleanup();
}

#endif  // PERF_ENABLE

// EOF
