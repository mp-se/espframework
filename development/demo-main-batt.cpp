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
