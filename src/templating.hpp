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
#ifndef SRC_TEMPLATING_HPP_
#define SRC_TEMPLATING_HPP_

#include <algorithm>
#include <espframework.hpp>
#include <log.hpp>
#include <memory>
#include <utils.hpp>

constexpr auto MAX_KEY_VAL = 30;

class TemplatingEngine {
 private:
  struct KeyVal {
    String key;
    String val;
  };

  KeyVal _items[MAX_KEY_VAL];

  char _buffer[20] = "";
  std::unique_ptr<char[]> _output;

  void transform(const char *format) {
    int len = strlen(format);
    int size = len;

    // Lets check how much memory will be needed to transform the template
    for (int j = 0; j < len - 2; j++) {
      if (*(format + j) == '$' && *(format + j + 1) == '{') {
        // Start of format tag found
        int max = sizeof(_items) / sizeof(KeyVal);
        for (int i = 0; i < max; i++) {
          if (strncmp(format + j, _items[i].key.c_str(),
                      _items[i].key.length()) == 0) {
            // Found key
            size = size - _items[i].key.length() + _items[i].val.length();
          }
        }
      }
    }

    Log.notice(F("TPL : Buffer needed %d." CR), size);

    _output.reset(new char[size + 10]);
    memset(_output.get(), 0, size + 10);

    if (!_output) {
      Log.error(F("TPL : Unable to allocate memory for transforming template, "
                  "needed %d." CR),
                size);
      return;
    }

    // Lets do the transformation
    int k = 0;
    for (int j = 0; j < len - 2; j++) {
      if (*(format + j) == '$' && *(format + j + 1) == '{') {
        // Start of format tag found
        int max = sizeof(_items) / sizeof(KeyVal);
        for (int i = 0; i < max; i++) {
          if (!_items[i].key.equals("")) {
            if (strncmp(format + j, _items[i].key.c_str(),
                        _items[i].key.length()) == 0) {
              // Found key
              strncat(_output.get(), format + k, j - k);
              strncat(_output.get(), _items[i].val.c_str(),
                      _items[i].val.length());
              k = j + _items[i].key.length();
            }
          }
        }
      }
    }
    // strncat(_output, format + k, size - k);
    strncat(_output.get(), format + k, strlen(format + k));
    Log.notice(F("TPL : Transformed template %d chars to %d chars" CR),
               strlen(format), strlen(_output.get()));
  }

 public:
  TemplatingEngine() {}

  void setVal(String key, float val, int dec = 2) {
    String s = convertFloatToString(val, &_buffer[0], dec);
    s.trim();
    setVal(key, s);
  }
  void setVal(String key, int val) { setVal(key, String(val)); }
  void setVal(String key, char val) { setVal(key, String(val)); }
  void setVal(String key, String val) {
    int max = sizeof(_items) / sizeof(KeyVal);
    for (int i = 0; i < max; i++) {
      if (_items[i].key.equals("")) {
        _items[i].key = key;
        _items[i].val = val;
        return;
      } else if (_items[i].key.equals(key)) {
        _items[i].val = val;
        return;
      }
    }

    Log.warning(F("TPL : Too many keys, max is %d." CR), MAX_KEY_VAL);
  }

  void dumpAll() const {
    int max = sizeof(_items) / sizeof(KeyVal);
    for (int i = 0; i < max; i++) {
      if (!_items[i].key.equals("")) {
        EspSerial.print("Key=\'");
        EspSerial.print(_items[i].key.c_str());
        EspSerial.print("\', Val=\'");
        EspSerial.print(_items[i].val.c_str());
        EspSerial.println("\'");
      }
    }
  }

  void freeMemory() { _output.reset(); }
  const char *create(const char *base);
};

#endif  // SRC_TEMPLATING_HPP_

// EOF
