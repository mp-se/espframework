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
#include <led.hpp>

SerialDebug mySerial(115200L);

void setup() {
  delay(1000);
  Log.notice(F("Main: Started setup." CR));

  Log.notice(F("Blue LED." CR));
  ledOn(LedColor::BLUE);
  delay(2000);

  Log.notice(F("Green LED." CR));
  ledOn(LedColor::GREEN);
  delay(2000);

  Log.notice(F("Red LED." CR));
  ledOn(LedColor::RED);
  delay(2000);

  Log.notice(F("Black LED." CR));
  ledOn(LedColor::BLACK);
  delay(2000);

  Log.notice(F("Cyan LED." CR));
  ledOn(LedColor::CYAN);
  delay(2000);

  Log.notice(F("Purple LED." CR));
  ledOn(LedColor::PURPLE);
  delay(2000);

  Log.notice(F("Yellow LED." CR));
  ledOn(LedColor::YELLOW);
  delay(2000);

  Log.notice(F("White LED." CR));
  ledOn(LedColor::WHITE);
  delay(2000);

  Log.notice(F("Off LED." CR));
  ledOn(LedColor::OFF);
  delay(2000);
}

void loop() {
}

// EOF
