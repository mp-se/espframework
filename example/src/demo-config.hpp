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
#ifndef SRC_DEMO_CONFIG_HPP_
#define SRC_DEMO_CONFIG_HPP_

#include <baseconfig.hpp>

class DemoConfig : public BaseConfig {
 private:
 public:
  DemoConfig(String baseMDNS, String fileName);

  void createJson(JsonObject& doc) const override;
  void parseJson(JsonObject& doc) override;
};

#endif  // SRC_DEMO_CONFIG_HPP_

// EOF
