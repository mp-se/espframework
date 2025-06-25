#include <AUnit.h>
#include <looptimer.hpp>

test(LoopTimer_ExpireAndReset) {
  LoopTimer timer(100); // 100 ms interval
  delay(120); // Wait for timer to expire
  assertTrue(timer.hasExpired());
  uint64_t v = 1;
  assertEqual(timer.getLoopCounter(), v);
  timer.reset();
  assertFalse(timer.hasExpired());
}

test(LoopTimer_IntervalChange) {
  LoopTimer timer(200);
  timer.setInterval(50);
  delay(60);
  assertTrue(timer.hasExpired());
}

test(LoopTimer_TimePassed) {
  LoopTimer timer(1000);
  delay(30);
  int32_t passed = timer.getTimePassed();
  assertTrue(passed >= 25 && passed <= 50); // Allow for some timing jitter
}
// EOF
