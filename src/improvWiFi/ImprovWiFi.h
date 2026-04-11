/*
 * ESPFramework
 * Copyright (c) 2025-2026 Magnus
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
#ifndef SRC_IMPROVWIFI_H_
#define SRC_IMPROVWIFI_H_

#include <improvWiFi/improv.h>

#include <vector>

class ImprovWiFi {
 public:
  ImprovWiFi(const char *firmwareName, const char *version, const char *chip,
             const char *deviceName);

  void loop();

  void setWiFiCallback(
      std::function<void(const char *, const char *)> callback);
  void setInfoCallback(std::function<void(const char *)> callback);
  void setDebugCallback(std::function<void(const char *)> callback);

  bool isConfigInitiated() { return infoConfigInitiated; }
  bool isConfigCompleted() { return infoConfigCompleted; }

 private:
  void getAvailableWifiNetworks();
  void sendProvisioned(improv::Command responseTo);

  void set_state(improv::State state);
  void send_response(std::vector<uint8_t> &response);
  void set_error(improv::Error error);

  bool onCommandCallback(improv::ImprovCommand cmd);
  void onErrorCallback(improv::Error err);

  char msgBuf[16];
  uint8_t x_buffer[50];
  uint8_t x_position = 0;
  const char *firmwareName, *version, *chip, *deviceName;

  improv::Command pendingResponse;
  int64_t pendingCmdReceived;

  bool infoConfigInitiated = false, infoConfigCompleted = false;

  std::function<void(const char *)> infoCallback;
  std::function<void(const char *)> debugCallback;
  std::function<void(const char *, const char *)> wifiCallback;
};

#endif  // SRC_IMPROVWIFI_H_
