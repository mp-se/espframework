/*
MIT License

Copyright (c) 2021-2024 Magnus

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
#ifndef SRC_ESPFRAMEWORK_HPP_
#define SRC_ESPFRAMEWORK_HPP_

#define ESPFWK_VER "1.0.0-alfa1"

#if defined(ESP8266)
#define ESP_RESET ESP.reset
#define PIN_LED 2
#elif defined(ESP32)
#define ESP_RESET ESP.restart
#if defined(LED_BUILTIN)
#define PIN_LED LED_BUILTIN
#else
#define PIN_LED 2
#endif
#endif

constexpr auto JSON_BUFFER_SIZE_S = 500;
constexpr auto JSON_BUFFER_SIZE_M = 1000;
constexpr auto JSON_BUFFER_SIZE_L = 3000;
constexpr auto JSON_BUFFER_SIZE_XL = 5000;

#endif  // SRC_ESPFRAMEWORK_HPP_

// EOF
