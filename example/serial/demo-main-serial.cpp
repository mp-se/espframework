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
#include <looptimer.hpp>

SerialDebug mySerial(115200L);
LoopTimer myTimer(1000);  

void setup() {
#if defined(ESPFWK_USE_SERIAL_PINS)
  // Setup serial with new pins and speed or just initialize it again if needed
  mySerial.setup(115200L, TX, RX);
  Log.notice(F("Main: Using serial pins as output." CR));
#else
  Log.notice(F("Main: Using usb connector as output." CR));
#endif
}

void loop() {
  if(myTimer.hasExipred()) {
    myTimer.reset();  
    Log.notice(F("Loop: Timer triggered." CR));
  }
}

// EOF
