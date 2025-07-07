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
#include <templating.hpp>

test(TemplatingEngine_SimpleReplace) {
  TemplatingEngine tpl;
  tpl.setVal("${name}", "World");
  const char* result = tpl.create("Hello, ${name}!");
  assertEqual(String(result), String("Hello, World!"));
}

test(TemplatingEngine_MultipleReplace) {
  TemplatingEngine tpl;
  tpl.setVal("${adj}", "awesome");
  tpl.setVal("${noun}", "framework");
  const char* result = tpl.create("This is an ${adj} ${noun}.");
  assertEqual(String(result), String("This is an awesome framework."));
}

test(TemplatingEngine_IntAndFloat) {
  TemplatingEngine tpl;
  tpl.setVal("${int}", 42);
  tpl.setVal("${float}", 3.14f, 2);
  const char* result = tpl.create("Int: ${int}, Float: ${float}");
  assertTrue(String(result).startsWith("Int: 42, Float: 3.14"));
}

// EOF
