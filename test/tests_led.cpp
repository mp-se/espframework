#include <AUnit.h>
#include <led.hpp>

test(Led_OnOff) {
  // These functions are hardware-dependent and do not return a value.
  // The test will pass if they do not crash or hang.
  ledOn(LedColor::WHITE);
  ledOn(LedColor::RED);
  ledOn(LedColor::GREEN);
  ledOn(LedColor::BLUE);
  ledOn(LedColor::OFF);
  ledOff();
  assertTrue(true); // If no crash, test passes
}

// EOF
