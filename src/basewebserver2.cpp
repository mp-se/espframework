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
#ifndef ESPFWK_DISABLE_WEBSERVER

#ifdef ESPFWK_PSYCHIC_HTTP

#include <HTTPUpdate.h>

#include <baseconfig.hpp>
#include <basewebserver2.hpp>
#include <espframework.hpp>
#include <base64.h>
#if !defined(MAX_SKETCH_SPACE)
#define MAX_SKETCH_SPACE 0x1c0000
#warning "MAX_SKETCH_SPACE is not defined, using default value of 0x1c0000"
#endif

BaseWebServer::BaseWebServer(WebConfigInterface*config) { _webConfig = config; }

bool BaseWebServer::isAuthenticated(PsychicRequest *request) {
  resetWifiPortalTimer();

  if (request->hasHeader("Authorization")) {
    String authHeader = request->header("Authorization");
    if (authHeader == "Bearer " + _authToken) {
      Log.info(F("WEB : Authorized with token." CR));
      return true;
    }
  }

  Log.info(F("WEB : NOT Authorized." CR));
  return request->reply(401);
}

void BaseWebServer::loop() {
  if (!_server) return;

  if (_wifiSetup) {
    if (abs(static_cast<int32_t>((millis() - _wifiPortalTimer))) >
        (_webConfig->getWifiPortalTimeout() * 1000)) {
      Log.notice(F("WEB : Wifi portal timeout, reboot device." CR));
      delay(500);
      ESP_RESET();
    }
  }

  if (_rebootTask) {
    if (abs(static_cast<int32_t>((millis() - _rebootTimer))) > 1000) {
      Log.notice(F("WEB : Rebooting..." CR));
      delay(500);
      ESP_RESET();
    }
  }

  if (_wifiScanTask) {
    JsonDocument doc;

    doc[PARAM_STATUS] = false;
    doc[PARAM_SUCCESS] = true;
    doc[PARAM_MESSAGE] = "";

    JsonArray networks = doc[PARAM_NETWORKS].to<JsonArray>();

    Log.notice(F("WEB : Scanning for wifi networks." CR));
    int noNetwork = WiFi.scanNetworks(false, false);

    for (int i = 0; i < noNetwork; i++) {
      networks[i][PARAM_SSID] = WiFi.SSID(i);
      networks[i][PARAM_RSSI] = WiFi.RSSI(i);
      networks[i][PARAM_CHANNEL] = WiFi.channel(i);
      networks[i][PARAM_ENCRYPTION] = WiFi.encryptionType(i);
    }

    unsigned int ret = serializeJson(doc, _wifiScanData);
    Log.notice(F("WEB : Scan complete %d, %s." CR), ret, _wifiScanData.c_str());
    _wifiScanTask = false;
  }
}

esp_err_t BaseWebServer::webHandleUploadFirmware(PsychicRequest *request,
                                                 String filename,
                                                 uint64_t index, uint8_t *data,
                                                 size_t len, bool final) {
  if (!isAuthenticated(request)) {
    return ESP_FAIL;
  }

  uint32_t maxSketchSpace = MAX_SKETCH_SPACE;
  Log.verbose(F("WEB : BaseWebHandler callback for /api/firmware." CR));

  if (!index) {
    _uploadedSize = 0;
    if (!Update.begin(request->contentLength(), U_FLASH, LED_BUILTIN)) {
      _uploadReturn = 500;
      Log.error(F("WEB : Not enough space to store for this firmware (%d)." CR),
                request->contentLength());
    } else {
      _uploadReturn = 200;
      Log.notice(F("WEB : Start firmware upload, max sketch size %d kb, size "
                   "%d kb." CR),
                 maxSketchSpace / 1024, request->contentLength() / 1024);
    }
  }

  _uploadedSize += len;

  if (_uploadedSize > maxSketchSpace) {
    _uploadReturn = 500;
    Log.error(F("WEB : Firmware file is to large." CR));
  } else if (Update.write(data, len) != len) {
    _uploadReturn = 500;
    Log.notice(F("WEB : Writing firmware upload %d (%d)." CR), len,
               maxSketchSpace);
  } else {
    EspSerial.print(".");
  }

  if (final) {
    EspSerial.print("\n");
    Log.notice(F("WEB : Finished firmware upload." CR));
    request->reply(200);

    if (Update.end(true)) {
      // Calling reset here will not wait for all the data to be sent, lets wait
      // a second before rebooting in loop.
      Log.notice(F("WEB : Scheduling reboot." CR));
      _rebootTimer = millis();
      _rebootTask = true;
    } else {
      Log.error(F("WEB : Failed to finish firmware flashing, error %d" CR),
                Update.getError());
      _uploadReturn = 500;
    }
  }

  return ESP_OK;
}

esp_err_t BaseWebServer::webHandleUploadFile(PsychicRequest *request,
                                             String filename, size_t index,
                                             uint8_t *data, size_t len,
                                             bool final) {
  if (!isAuthenticated(request)) {
    return ESP_FAIL;
  }

  uint32_t maxFileSize = LittleFS.totalBytes() - LittleFS.usedBytes() - 4096;
  Log.verbose(
      F("WEB : BaseWebHandler callback for /api/filesystem/upload." CR));

  if (!index) {
    Log.notice(F("WEB : Start file upload, free space %d kb, size %d." CR),
               maxFileSize / 1024, request->contentLength());

    if (len > maxFileSize) {
      Log.error(F("WEB : File is to large to fit in file system." CR));
      return request->reply(500);
    }

    _tempFile = LittleFS.open("/" + filename, "w");
    _uploadReturn = 200;
  }

  if (len) {
    _tempFile.write(data, len);
    EspSerial.print(".");
  }

  if (final) {
    Log.notice(F("WEB : Finished file upload." CR));
    _tempFile.close();
    return request->reply(200);
  }

  return ESP_OK;
}

esp_err_t BaseWebServer::webHandlePageNotFound(PsychicRequest *request) {
  if (_wifiSetup) {
    return request->redirect("http://192.168.4.1");
  }

  if (request->method() == HTTP_GET)
    Log.warning(F("WEB : GET on %s not recognized." CR),
                request->url().c_str());
  else if (request->method() == HTTP_POST)
    Log.warning(F("WEB : POST on %s not recognized." CR),
                request->url().c_str());
  else if (request->method() == HTTP_PUT)
    Log.warning(F("WEB : PUT on %s not recognized." CR),
                request->url().c_str());
  else if (request->method() == HTTP_DELETE)
    Log.warning(F("WEB : DELETE on %s not recognized." CR),
                request->url().c_str());
  else
    Log.warning(F("WEB : Unknown on %s not recognized." CR),
                request->url().c_str());

  return request->redirect("/");
}

esp_err_t BaseWebServer::webHandleAuth(PsychicRequest *request) {
  Log.notice(F("WEB : webServer callback for /api/auth." CR));
  Log.notice(F("WEB : user: %s, pass: %s." CR), _webConfig->getAdminUser(), _webConfig->getAdminPass());

  if(isSslEnabled() && !_wifiSetup) {
    Log.notice(F("WEB : Performing basic authentication." CR));

    if( !request->authenticate(_webConfig->getAdminUser(), _webConfig->getAdminPass()) ) {
      return request->reply(401);
    }
  } 

  // Generate unique auth token
  uint8_t buf[16];
  for(int i = 0; i < 16; i++) buf[i] = random(256);
  _authToken = base64::encode(buf, 16);

  PsychicJsonResponse response(request);
  JsonObject obj = response.getRoot().as<JsonObject>();

  obj[PARAM_TOKEN] = _authToken;
  return response.send();
}

esp_err_t BaseWebServer::webHandleFileSystem(PsychicRequest *request,
                                             JsonVariant &json) {
  if (!isAuthenticated(request)) {
    return ESP_FAIL;
  }

  Log.notice(F("WEB : webServer callback for /api/filesystem." CR));
  JsonObject obj = json.as<JsonObject>();  

  if (!obj[PARAM_COMMAND].isNull()) {
    if (obj[PARAM_COMMAND].as<String>() == String("dir")) {
      Log.notice(F("WEB : File system listing requested." CR));
      PsychicJsonResponse response(request);
      JsonObject obj = response.getRoot().as<JsonObject>();

      obj[PARAM_TOTAL] = LittleFS.totalBytes();
      obj[PARAM_USED] = LittleFS.usedBytes();
      obj[PARAM_FREE] = LittleFS.totalBytes() - LittleFS.usedBytes();

      File root = LittleFS.open("/");
      File f = root.openNextFile();
      int i = 0;

      JsonArray arr = obj[PARAM_FILES].to<JsonArray>();
      while (f) {
        arr[i][PARAM_FILE] = "/" + String(f.name());
        arr[i][PARAM_SIZE] = static_cast<int>(f.size());
        f = root.openNextFile();
        i++;
      }
      f.close();
      root.close();
      return response.send();
    } else if (obj[PARAM_COMMAND].as<String>() == String("del")) {
      Log.notice(F("WEB : File system delete requested." CR));

      if (!obj[PARAM_FILE].isNull()) {
        String f = obj[PARAM_FILE];
        if (f.length() > 96) {
          Log.warning(F("WEB : Filename to long." CR));
          return request->reply(400);
        }
        LittleFS.remove(f);
        return request->reply(200);
      } else {
        return request->reply(400);
      }
    } else if (obj[PARAM_COMMAND].as<String>() == String("get")) {
      Log.notice(F("WEB : File system get requested." CR));
      if (!obj[PARAM_FILE].isNull()) {
        String f = obj[PARAM_FILE];
        if (f.length() > 96) {
          Log.warning(F("WEB : Filename to long." CR));
          return request->reply(400);
        }

        if (LittleFS.exists(obj[PARAM_FILE].as<String>())) {
          File file = LittleFS.open(f, "r");

          if (file) {
            PsychicResponse response(request);

            response.setContentType("application/octet-stream");
            response.setContentLength(file.size());
            response.sendHeaders();

            static uint8_t fs_chunk_buf[512];
            size_t len = 0;
            while (file.available()) {
              len = file.read(fs_chunk_buf, sizeof(fs_chunk_buf));
              response.sendChunk(fs_chunk_buf, len);
              if ((len & 0x3F) == 0) {
                delay(0);
              }
            }
            file.close();
            response.finishChunking();
          } else {
            request->reply(404);
            return ESP_FAIL;
          }
        } else {
          request->reply(404);
          return ESP_FAIL;
        }
      } else {
        return request->reply(400);
      }
    } else {
      Log.warning(F("WEB : Unknown file system command." CR));
      return request->reply(400);
    }
  } else {
    Log.warning(F("WEB : Unknown file system command." CR));
    return request->reply(400);
  }
  return ESP_OK;
}

esp_err_t BaseWebServer::webHandleWifiScan(PsychicRequest *request) {
  if (!isAuthenticated(request)) {
    return ESP_FAIL;
  }

  Log.notice(F("WEB : webServer callback for /api/wifi/scan." CR));
  _wifiScanTask = true;
  _wifiScanData = "";

  PsychicJsonResponse response(request);
  JsonObject obj = response.getRoot().as<JsonObject>();

  obj[PARAM_SUCCESS] = true;
  obj[PARAM_MESSAGE] = "Scheduled wifi scanning";
  return response.send();
}

esp_err_t BaseWebServer::webHandleWifiScanStatus(PsychicRequest *request) {
  if (!isAuthenticated(request)) {
    return ESP_FAIL;
  }

  Log.notice(F("WEB : webServer callback for /api/wifi/scan/status." CR));

  if (_wifiScanTask || !_wifiScanData.length()) {
    PsychicJsonResponse response(request);
    JsonObject obj = response.getRoot().as<JsonObject>();

    obj[PARAM_STATUS] = static_cast<bool>(_wifiScanTask);
    obj[PARAM_SUCCESS] = false;
    obj[PARAM_MESSAGE] =
        _wifiScanTask ? "Wifi scanning running" : "No scanning running";
    return response.send();
  } else {
    PsychicResponse response(request);

    response.setContent(_wifiScanData.c_str());
    response.setCode(200);
    return response.send();
  }

  return ESP_OK;
}

esp_err_t BaseWebServer::webHandleRestart(PsychicRequest *request) {
  if (!isAuthenticated(request)) {
    return ESP_FAIL;
  }

  Log.notice(F("WEB : webServer callback for /api/restart." CR));
  PsychicJsonResponse response(request);
  JsonObject obj = response.getRoot().as<JsonObject>();

  obj[PARAM_STATUS] = true;
  obj[PARAM_SUCCESS] = true;
  obj[PARAM_MESSAGE] = "Restarting...";
  response.send();
  _rebootTimer = millis();
  _rebootTask = true;
  return ESP_OK;
}

esp_err_t BaseWebServer::webHandlePing(PsychicRequest *request) {
  Log.notice(F("WEB : webServer callback for /api/ping." CR));
  PsychicJsonResponse response(request);
  JsonObject obj = response.getRoot().as<JsonObject>();

  obj[PARAM_STATUS] = true;
  return response.send();
}

void BaseWebServer::setupWebHandlers() {
  if (!_server) return;

  Log.notice(F("WEB : Setting up async web handlers." CR));

  _server->on(
      "/", HTTP_GET,
      (PsychicHttpRequestCallback)std::bind(&BaseWebServer::webReturnIndexHtml,
                                            this, std::placeholders::_1));
  _server->on(
      "/index.html", HTTP_GET,
      (PsychicHttpRequestCallback)std::bind(&BaseWebServer::webReturnIndexHtml,
                                            this, std::placeholders::_1));
  _server->on("/js/app.js", HTTP_GET,
              (PsychicHttpRequestCallback)std::bind(
                  &BaseWebServer::webReturnAppJs, this, std::placeholders::_1));
  _server->on(
      "/css/app.css", HTTP_GET,
      (PsychicHttpRequestCallback)std::bind(&BaseWebServer::webReturnAppCss,
                                            this, std::placeholders::_1));
  _server->on(
      "/favicon.ico", HTTP_GET,
      (PsychicHttpRequestCallback)std::bind(&BaseWebServer::webReturnFavicon,
                                            this, std::placeholders::_1));
  _server->on("/api/auth", HTTP_GET,
              (PsychicHttpRequestCallback)std::bind(
                  &BaseWebServer::webHandleAuth, this, std::placeholders::_1));
  _server->on("/api/wifi/status", HTTP_GET,
              (PsychicHttpRequestCallback)std::bind(
                  &BaseWebServer::webHandleWifiScanStatus, this,
                  std::placeholders::_1));
  _server->on(
      "/api/wifi", HTTP_GET,
      (PsychicHttpRequestCallback)std::bind(&BaseWebServer::webHandleWifiScan,
                                            this, std::placeholders::_1));
  _server->on(
      "/api/restart", HTTP_GET,
      (PsychicHttpRequestCallback)std::bind(&BaseWebServer::webHandleRestart,
                                            this, std::placeholders::_1));
  _server->on("/api/ping", HTTP_GET,
              (PsychicHttpRequestCallback)std::bind(
                  &BaseWebServer::webHandlePing, this, std::placeholders::_1));

  PsychicUploadHandler *fileUploadHandler = new PsychicUploadHandler();
  fileUploadHandler->onUpload((PsychicUploadCallback)std::bind(
      &BaseWebServer::webHandleUploadFile, this, std::placeholders::_1,
      std::placeholders::_2, std::placeholders::_3, std::placeholders::_4,
      std::placeholders::_5, std::placeholders::_6));
  _server->on("/api/filesystem/upload", HTTP_POST, fileUploadHandler);
  _server->on("/api/filesystem", HTTP_POST,
              (PsychicJsonRequestCallback)std::bind(
                  &BaseWebServer::webHandleFileSystem, this,
                  std::placeholders::_1, std::placeholders::_2));

  PsychicUploadHandler *firmwareUploadHandler = new PsychicUploadHandler();

  firmwareUploadHandler->onUpload((PsychicUploadCallback)std::bind(
      &BaseWebServer::webHandleUploadFirmware, this, std::placeholders::_1,
      std::placeholders::_2, std::placeholders::_3, std::placeholders::_4,
      std::placeholders::_5, std::placeholders::_6));
  _server->on("/api/firmware", HTTP_POST, firmwareUploadHandler);
  _server->on("*", HTTP_OPTIONS, [](PsychicRequest *request) {
    Log.notice(F("WEB : Got OPTIONS request url=%s." CR),
               request->url().c_str());
    return request->reply(200);
  });
  _server->onNotFound(std::bind(&BaseWebServer::webHandlePageNotFound, this,
                                std::placeholders::_1));
}

bool BaseWebServer::setupWebServer(bool skipSSL) {
  Log.notice(F("WEB : Configuring web server." CR));

  // Create the correct server wrappers
#if defined(ESPFWK_PSYCHIC_SSL)
  _server.reset(new PsychicHttpsServer());
  _redirectServer.reset(new PsychicHttpServer());
  _server->config.max_uri_handlers = 30;
  _server->ssl_config.httpd.max_uri_handlers = 20;
#else
  _server.reset(new PsychicHttpServer());
  _server->config.max_uri_handlers = 30;
#endif

#if defined(ESPFWK_PSYCHIC_SSL)
  if(!skipSSL) {
    File fCert = LittleFS.open("/server.crt");
    if (fCert) {
      Log.notice(F("WEB : Found server certificate /server.crt." CR));
      _sslCert = fCert.readString();
      fCert.close();

      File fKey = LittleFS.open("/server.key");
      if (fKey) {
        Log.notice(F("WEB : Found server certificate /server.key." CR));
        _sslKey = fKey.readString();
        fKey.close();
      } 
    }
  }

  if (_sslCert.length() && _sslKey.length()) {
    Log.notice(F("WEB : Starting web server using SSL." CR));
    _server->listen(443, _sslCert.c_str(), _sslKey.c_str());

    _redirectServer->config.ctrl_port =
        20424;  // just a random port different from the default one
    _redirectServer->listen(80);
    _redirectServer->onNotFound([](PsychicRequest *request) {
      String url = "https://" + request->host() + request->url();
      return request->redirect(url.c_str());
    });
  } else {
    Log.notice(
        F("WEB : Starting web server without SSL." CR));
    _server->listen(80);
  }

#else
  Log.notice(F("WEB : Starting web server." CR));
  _server->listen(80);
#endif

  MDNS.begin(_webConfig->getMDNS());
  MDNS.addService("http", "tcp", 80);

  File root = LittleFS.open("/");
  File f = root.openNextFile();
  while (f) {
    Log.notice(F("WEB : File=%s, %d bytes" CR), f.name(), f.size());
    if (!f.size()) {
      Log.notice(F("WEB : Empty file detected, removing file." CR));
      LittleFS.remove(f.name());
    }

    f = root.openNextFile();
  }
  f.close();
  root.close();

  setupWebHandlers();
  if (_webConfig->isCorsAllowed()) {
    DefaultHeaders::Instance().addHeader("Access-Control-Allow-Origin", "*");
    DefaultHeaders::Instance().addHeader("Access-Control-Allow-Methods",
                                         "GET, POST, PUT, DELETE, OPTIONS");
    DefaultHeaders::Instance().addHeader("Access-Control-Allow-Headers",
                                         "Content-Type, Authorization, Accept");
    DefaultHeaders::Instance().addHeader("Access-Control-Max-Age", "86400");
  }

  Log.notice(F("WEB : Web server started." CR));
  return true;
}

#endif  // ESPFWK_PSYCHIC_HTTP

#endif  // !ESPFWK_DISABLE_WEBSERVER

// EOF
