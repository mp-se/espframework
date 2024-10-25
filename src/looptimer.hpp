/*
MIT License

Copyright (c) 2024 Magnus

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
#ifndef SRC_LOOPTIMER_HPP_
#define SRC_LOOPTIMER_HPP_

#include <Arduino.h>

class LoopTimer {
 private:
  uint64_t _startMillis = 0;
  uint64_t _interval = 0;
  uint64_t _loopCounter = 0;

 public:
  explicit LoopTimer(uint64_t interval) {
    _interval = interval;
    reset();
  }

  bool hasExipred() {
    if (abs((int32_t)(millis() - _startMillis)) > _interval) {
      _loopCounter++;
      return true;
    }
    return false;
  }

  void reset() { _startMillis = millis(); }
  uint64_t getLoopCounter() { return _loopCounter; }
};

#endif  // SRC_LOOPTIMER_HPP_

// EOF