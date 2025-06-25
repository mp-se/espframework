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
#include <AUnit.h>
#include <utils.hpp>

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

// EOF
