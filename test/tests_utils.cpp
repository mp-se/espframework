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
#include <AUnit.h>
#include <utils.hpp>

// Forward declare for test build if not included by default
unsigned char h2int(char c);
extern void detectChipRevision();
extern bool isEsp32c3();

test(Utils_ConvertCtoF) {
  assertNear(convertCtoF(0.0), 32.0, 0.01);
  assertNear(convertCtoF(100.0), 212.0, 0.01);
}

test(Utils_ConvertFtoC) {
  assertNear(convertFtoC(32.0), 0.0, 0.01);
  assertNear(convertFtoC(212.0), 100.0, 0.01);
}

test(Utils_ConvertKGtoLBS) {
  assertNear(convertKGtoLBS(1.0), 2.20462, 0.0001);
  assertNear(convertKGtoLBS(0.0), 0.0, 0.0001);
}

test(Utils_ConvertLBStoKG) {
  assertNear(convertLBStoKG(2.20462), 1.0, 0.001);
  assertNear(convertLBStoKG(0.0), 0.0, 0.0001);
}

test(Utils_ConvertCLtoUSOZ) {
  assertNear(convertCLtoUSOZ(10.0), 3.4, 0.01);
}

test(Utils_ConvertCLtoUKOZ) {
  assertNear(convertCLtoUKOZ(10.0), 3.5211, 0.01);
  assertNear(convertCLtoUKOZ(0.0), 0.0, 0.01);
}

test(Utils_ConvertUSOZtoCL) {
  assertNear(convertUSOZtoCL(3.4), 10.0, 0.01);
  assertNear(convertUSOZtoCL(0.0), 0.0, 0.01);
}

test(Utils_ConvertUKOZtoCL) {
  assertNear(convertUKOZtoCL(3.5211), 10.0, 0.01);
}

test(Utils_ConvertToPlato) {
  assertNear(convertToPlato(1.050), 12.33, 0.02);
  assertNear(convertToPlato(0.0), 0.0, 0.01);
}

test(Utils_ConvertToSG) {
  assertNear(convertToSG(12.369), 1.05, 0.01);
}

test(Utils_ReduceFloatPrecision) {
  float f = 3.14159;
  float reduced = reduceFloatPrecision(f, 2);
  assertNear(reduced, 3.14, 0.01);
}

test(Utils_ConvertFloatToString) {
  char buf[16];
  convertFloatToString(3.14159, buf, 2);
  assertEqual(String(buf), String("  3.14"));
}

test(Utils_UrlEncodeDecode) {
  String original = "hello world!";
  String encoded = urlencode(original);
  String decoded = urldecode(encoded);
  assertEqual(decoded, original);
}

test(Utils_ReduceFloatPrecision_EdgeCases) {
  assertNear(reduceFloatPrecision(-3.14159, 2), -3.14, 0.01);
  assertNear(reduceFloatPrecision(0.0, 2), 0.0, 0.01);
  assertNear(reduceFloatPrecision(123.456789, 4), 123.4568, 0.0001);
  assertNear(reduceFloatPrecision(1.9999, 0), 2.0, 0.1);
}

test(Utils_ConvertFloatToString_EdgeCases) {
  char buf[16];
  convertFloatToString(-3.14159, buf, 3);
  assertEqual(String(buf), String("-3.142")); 
  convertFloatToString(0.0, buf, 1);
  assertEqual(String(buf), String("   0.0")); // Add leading spaces to match actual output
  convertFloatToString(123.456, buf, 0);
  assertEqual(String(buf), String("   123"));
}

test(Utils_h2int) {
  assertEqual(h2int('0'), 0);
  assertEqual(h2int('9'), 9);
  assertEqual(h2int('a'), 10);
  assertEqual(h2int('f'), 15);
  assertEqual(h2int('A'), 10);
  assertEqual(h2int('F'), 15);
  assertEqual(h2int('z'), 0); // invalid
  assertEqual(h2int('!'), 0); // invalid
}

test(Utils_UrlEncodeDecode_EdgeCases) {
  assertEqual((int)urlencode("").length(), 0);
  assertEqual((int)urldecode("").length(), 0);
  String special = "!@#$%^&*()_+";
  String encoded = urlencode(special);
  String decoded = urldecode(encoded);
  assertEqual(decoded, special);
  String alreadyEncoded = "%21%40%23";
  assertEqual(urldecode(alreadyEncoded), "!@#");
  String onlyPercent = "%25";
  assertEqual(urldecode(onlyPercent), "%");
}

test(Utils_Callability) {
  // These are no-op or hardware/side-effect, just ensure they can be called
  tcp_cleanup();
  // deepSleep(0); // Don't actually sleep in test
  printHeap("test");
  // forcedReset(); // Don't actually reset in test
  // espReset(); // Don't actually reset in test
  detectChipRevision();
  // checkResetReason(); // Platform-specific, may not be safe to call in test
  assertTrue(true); // If no crash, test passes
}

// EOF
