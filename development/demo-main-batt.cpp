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
#include <espframework.hpp>
#include <log.hpp>

SerialDebug mySerial(115200L);

void setup() {
  delay(4000);  // Allow some time for computer to connect

  Log.notice(F("Main: Started setup." CR));
 
  Log.notice(F("Main: Setup is completed." CR));

// #define PIN_VOLT A7 // Pin 35 on D32 PRO is connected to voltage divider
#define PIN_VOLT ADC_2_5db // IO3 on S3 PRO is connected to voltage divider

  pinMode(PIN_VOLT, INPUT); 
  analogReadResolution(SOC_ADC_MAX_BITWIDTH);
  analogSetAttenuation(ADC_11db); 
}

void loop() {
  int v = analogRead(PIN_VOLT);

  Log.notice(F("ADC: %d." CR), v);

  delay(1000);
}

// EOF
