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
#ifndef SRC_PERF_HPP_
#define SRC_PERF_HPP_

#if defined(PERF_ENABLE)

#include <baseconfig.hpp>

class PerfLogging {
 private:
  PushConfig* _config = 0;

  struct PerfEntry {
    uint32_t start;   // millis()
    uint32_t end;     // millis()
    uint32_t max;     // max time in ms
    const char* key;  // measurement

    PerfEntry* next;

    float mA;  // used when a power meeter is attached
    float V;
  };

  PerfEntry* first = 0;
  bool measurePower = false;

  PerfEntry* find(const char* k) {
    if (first == 0) return 0;

    PerfEntry* pe = first;

    while (strcmp(k, pe->key) != 0) {
      if (pe->next == 0) return 0;
      pe = pe->next;
    }
    return pe;
  }

  PerfEntry* add(const char* k) {
    if (first == 0) {
      first = new PerfEntry();
      first->key = k;
      first->next = 0;
      first->max = 0;
      return first;
    }

    PerfEntry* pe = first;

    while (strcmp(k, pe->key) != 0) {
      if (pe->next == 0) {
        pe->next = new PerfEntry();
        pe->next->key = k;
        pe->next->max = 0;
        pe->next->next = 0;
        return pe->next;
      }

      pe = pe->next;
    }

    return pe;
  }

  PerfLogging(PerfLogging const&) = delete;
  void operator=(PerfLogging const&) = delete;

 public:
  PerfLogging() {}

  static PerfLogging& getInstance() {
    static PerfLogging _instance;
    return _instance;
  }

  void setBaseConfig(PushConfig* config) { _config = config; }

  void clear();
  void start(const char* key);
  void stop(const char* key);
  void print();
  void pushInflux();
};

extern PerfLogging gblPerfLogging;

// Use these to collect performance data from various parts of the code
#define PERF_BEGIN(s) gblPerfLogging.getInstance().start(s)
#define PERF_END(s) gblPerfLogging.getInstance().stop(s)
#define PERF_CLEAR() gblPerfLogging.getInstance().clear()
#define PERF_PUSH() gblPerfLogging.getInstance().pushInflux()

#else

// These will disable the performance collection function
#define PERF_BEGIN(s)
#define PERF_END(s)
#define PERF_CLEAR()
#define PERF_PUSH()

#endif  // PERF_ENABLE

#endif  // SRC_PERF_HPP_

// EOF
