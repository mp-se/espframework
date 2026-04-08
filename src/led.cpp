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
#include <Ticker.h>

#include <espframework.hpp>
#include <led.hpp>
#include <log.hpp>

#ifdef ESPFWK_DISABLE_LED

void ledOn(LedColor l) {}
void ledOff() {}

#else

#ifdef RGB_BUILTIN
void ledOn(LedColor l) {
  uint8_t r, g, b, pin;

  r = (l & 0xff0000) >> 16;
  g = (l & 0x00ff00) >> 8;
  b = (l & 0x0000ff);
  pin = LED_BUILTIN;

  Log.info(F("LED : Setting led %d to RGB %d-%d-%d" CR), pin, r, g, b);
#if ESP_ARDUINO_VERSION_MAJOR >= 3
  rgbLedWrite(pin, r, g, b);
#elif ESP_ARDUINO_VERSION_MAJOR >= 2 && ESP_ARDUINO_VERSION_MAJOR < 3
  neopixelWrite(pin, g, r, b);
#endif
}
#elif defined(ESPFWK_ENABLE_RGB_LED)
void ledOn(LedColor l) {
  uint8_t r, g, b, pin;

  r = (l & 0xff0000) >> 16;
  g = (l & 0x00ff00) >> 8;
  b = (l & 0x0000ff);
  pin = LED_BUILTIN;

  Log.info(F("LED : Setting led %d to RGB %d-%d-%d (2)" CR), pin, r, g, b);
#if ESP_ARDUINO_VERSION_MAJOR >= 3
  rgbLedWrite(pin, r, g, b);
#elif ESP_ARDUINO_VERSION_MAJOR >= 2 && ESP_ARDUINO_VERSION_MAJOR < 3
  neopixelWrite(pin, r, g, b);  // C3 mini has a different API for rgb led
#endif
}
#else
bool ledInit = false;
Ticker ledTicker;

void ledToggle() { digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN)); }

void ledOn(LedColor l) {
  if (!ledInit) {
    pinMode(LED_BUILTIN, OUTPUT);
    ledInit = true;
  }

  if (l == LedColor::BLUE) {
    ledTicker.attach(1, ledToggle);
  } else if (l == LedColor::RED) {
    ledTicker.attach(0.2, ledToggle);
  } else {
    ledTicker.detach();
    digitalWrite(LED_BUILTIN, l);
  }
}
#endif

void ledOff() { ledOn(LedColor::OFF); }

#endif  // ESPFWK_DISABLE_LED

// EOF
