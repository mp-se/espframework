/*
 * ESPFramework
 * Copyright (c) 2021-2026 Magnus
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 */
#include <templating.hpp>

// the useDefaultTemplate param is there to support unit tests.
const char* TemplatingEngine::create(const char* base) {
#if LOG_LEVEL == 6
  Log.verbose(F("TPL : Base '%s'." CR), base);
#endif

  // Insert data into template.
  transform(base);

#if LOG_LEVEL == 6
  Log.verbose(F("TPL : Transformed '%s'." CR), !_output.get() ? "" : _output.get());
#endif

  if (_output) return _output.get();

  return "";
}

// EOF
