/*
MIT License

Copyright (c) 2021-23 Magnus

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
#include <log.hpp>
#include <utils.hpp>

float convertCtoF(float c) { return (c * 1.8) + 32.0; }

float convertFtoC(float f) { return (f - 32.0) / 1.8; }

float convertKGtoLBS(float kg) { return kg * 2.20462262185; }
float convertLBStoKG(float lbs) {
  return lbs == 0.0 ? 0.0 : lbs / 2.20462262185;
}

float convertCLtoUSOZ(float cl) { return cl * 0.34; }
float convertCLtoUKOZ(float cl) { return cl == 0.0 ? 0.0 : cl / 2.84; }
float convertUSOZtoCL(float usoz) { return usoz == 0.0 ? 0.0 : usoz / 0.34; }
float convertUKOZtoCL(float ukoz) { return ukoz * 2.84; }

float reduceFloatPrecision(float f, int dec) {
  char buffer[10];
  dtostrf(f, 6, dec, &buffer[0]);
  return atof(&buffer[0]);
}

char* convertFloatToString(float f, char* buffer, int dec) {
  dtostrf(f, 6, dec, buffer);
  return buffer;
}

struct tcp_pcb;
extern struct tcp_pcb* tcp_tw_pcbs;
extern "C" void tcp_abort(struct tcp_pcb* pcb);
void tcp_cleanup() {  // tcp cleanup, to avoid memory crash.
  while (tcp_tw_pcbs) tcp_abort(tcp_tw_pcbs);
}

void deepSleep(int t) {
#if LOG_LEVEL == 6
  Log.verbose(F("HELP: Entering sleep mode for %d seconds." CR), t);
#endif
  uint32_t wake = t * 1000000;
  ESP.deepSleep(wake);
}

void printHeap(String prefix) {
#if defined(ESP8266)
  Log.notice(
      F("%s: Free-heap %d kb, Heap-rag %d %%, Max-block %d kb Stack=%d b." CR),
      prefix.c_str(), ESP.getFreeHeap() / 1024, ESP.getHeapFragmentation(),
      ESP.getMaxFreeBlockSize() / 1024, ESP.getFreeContStack());
#else  // defined (ESP32)
  Log.notice(F("HELP: Heap %d kb, FreeSketch %d kb." CR),
             ESP.getFreeHeap() / 1024, ESP.getFreeSketchSpace() / 1024);
#endif
}

// EOF
