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
