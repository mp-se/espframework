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
#include <Arduino.h>
#include <AUnit.h>

using aunit::Printer;
using aunit::TestRunner;
using aunit::Verbosity;

void setup() {
  Serial.begin(115200);
  Serial.println("ESP framework - Unit Test Build");

  delay(2000);
  Printer::setPrinter(&Serial);
  // TestRunner::setVerbosity(Verbosity::kAll);

  // TestRunner::exclude("WifiConnection_*");
  // TestRunner::exclude("BaseConfig_*");
  // TestRunner::exclude("TemplatingEngine_*");
  // TestRunner::exclude("Uptime_*");
  // TestRunner::exclude("Utils_*");
  // TestRunner::exclude("LoopTimer_*");
  // TestRunner::exclude("JsonFS_*");
  // TestRunner::exclude("Led_*");

  // TestRunner::exclude("wifi_*");
}

void loop() {
  TestRunner::run();
  delay(10);
}

// EOF
