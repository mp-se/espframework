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
  
    if(!isSslEnabled()) {
      String altToken("Bearer ");
      altToken += _webConfig->getID();
      if (authHeader == altToken) {
        Log.info(F("WEB : Authorized with device id." CR));
        return true;
      }
    }
  }

  Log.info(F("WEB : NOT Authorized." CR));
  request->response()->send(401);
  return false;
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
      return ESP_FAIL;  // Abort upload immediately on init failure
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
    Log.error(F("WEB : Firmware file is too large." CR));
    Update.abort();  // Cleanup on size error
    return ESP_FAIL;
  } else if (Update.write(data, len) != len) {
    _uploadReturn = 500;
    Log.error(F("WEB : Failed writing firmware chunk, bytes: %d." CR), len);
    Update.abort();  // Cleanup on write error
    return ESP_FAIL;
  } else {
    EspSerial.print(".");
  }

  if (final) {
    EspSerial.print("\n");
    Log.notice(F("WEB : Finished firmware upload." CR));

    if (Update.end(true)) {
      Log.notice(F("WEB : Firmware verified and scheduled for reboot." CR));
      request->response()->send(200);
      
      // Schedule reboot after response is sent
      _rebootTimer = millis();
      _rebootTask = true;
    } else {
      Log.error(F("WEB : Failed to finish firmware flashing, error %d." CR),
                Update.getError());
      request->response()->send(500);
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

    if (request->contentLength() > maxFileSize) {
      Log.error(F("WEB : File is too large to fit in file system." CR));
      return ESP_FAIL;  // Abort upload immediately
    }

    _tempFile = LittleFS.open("/" + filename, "w");
    if (!_tempFile) {
      Log.error(F("WEB : Failed to create file for upload: %s." CR), filename.c_str());
      return ESP_FAIL;
    }
    _uploadReturn = 200;
  }

  if (len) {
    if (_tempFile.write(data, len) != len) {
      Log.error(F("WEB : Failed writing file data, requested %d bytes." CR), len);
      _tempFile.close();
      LittleFS.remove("/" + filename);  // Cleanup incomplete file
      _uploadReturn = 500;
      return ESP_FAIL;
    }
    EspSerial.print(".");
  }

  if (final) {
    Log.notice(F("WEB : Finished file upload." CR));
    _tempFile.close();
    if (_uploadReturn == 200) {
      return request->response()->send(200);
    } else {
      return request->response()->send(500);
    }
  }

  return ESP_OK;
}

esp_err_t BaseWebServer::webHandlePageNotFound(PsychicRequest *request) {
  PsychicResponse response(request);
  if (_wifiSetup) {
    response.setCode(301);
    return response.redirect("http://192.168.4.1");
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

  response.setCode(301);
  return response.redirect("/");
}

esp_err_t BaseWebServer::webHandleAuth(PsychicRequest *request) {
  PsychicResponse response(request);
  Log.notice(F("WEB : webServer callback for /api/auth." CR));
  Log.notice(F("WEB : user: %s, pass: %s." CR), _webConfig->getAdminUser(), _webConfig->getAdminPass());

  if(isSslEnabled() && !_wifiSetup) {
    Log.notice(F("WEB : Performing basic authentication." CR));

    if( !request->authenticate(_webConfig->getAdminUser(), _webConfig->getAdminPass()) ) {
      return response.send(401);
    }
  } 

  // Generate unique auth token
  uint8_t buf[16];
  for(int i = 0; i < 16; i++) buf[i] = random(256);
  _authToken = base64::encode(buf, 16);

  // Use a small JsonDocument to avoid stack overflow on ESP32C3
  JsonDocument doc;
  doc[PARAM_TOKEN] = _authToken;
  
  response.addHeader("Content-Type", "application/json");
  String jsonStr;
  serializeJson(doc, jsonStr);
  return response.send(200, "application/json", jsonStr.c_str());
}

esp_err_t BaseWebServer::webHandleFileSystem(PsychicRequest *request,
                                             PsychicResponse *response,
                                             JsonVariant &json) {
  if (!isAuthenticated(request)) {
    return ESP_FAIL;
  }

  Log.notice(F("WEB : webServer callback for /api/filesystem." CR));
  JsonObject obj = json.as<JsonObject>();  

  if (!obj[PARAM_COMMAND].isNull()) {
    if (obj[PARAM_COMMAND].as<String>() == String("dir")) {
      Log.notice(F("WEB : File system listing requested." CR));
      JsonDocument doc;

      doc[PARAM_TOTAL] = LittleFS.totalBytes();
      doc[PARAM_USED] = LittleFS.usedBytes();
      doc[PARAM_FREE] = LittleFS.totalBytes() - LittleFS.usedBytes();

      File root = LittleFS.open("/");
      File f = root.openNextFile();
      int i = 0;

      JsonArray arr = doc[PARAM_FILES].to<JsonArray>();
      while (f) {
        arr[i][PARAM_FILE] = "/" + String(f.name());
        arr[i][PARAM_SIZE] = static_cast<int>(f.size());
        f = root.openNextFile();
        i++;
      }
      f.close();
      root.close();
      
      String jsonStr;
      serializeJson(doc, jsonStr);
      return response->send(200, "application/json", jsonStr.c_str());
    } else if (obj[PARAM_COMMAND].as<String>() == String("del")) {
      Log.notice(F("WEB : File system delete requested." CR));

      if (!obj[PARAM_FILE].isNull()) {
        String f = obj[PARAM_FILE];
        if (f.length() > 96) {
          Log.warning(F("WEB : Filename to long." CR));
          return response->send(400);
        }
        LittleFS.remove(f);
        return response->send(200);
      } else {
        return response->send(400);
      }
    } else if (obj[PARAM_COMMAND].as<String>() == String("get")) {
      Log.notice(F("WEB : File system get requested." CR));
      if (!obj[PARAM_FILE].isNull()) {
        String f = obj[PARAM_FILE];
        if (f.length() > 96) {
          Log.warning(F("WEB : Filename to long." CR));
          return response->send(400);
        }

        if (LittleFS.exists(obj[PARAM_FILE].as<String>())) {
          File file = LittleFS.open(f, "r");

          if (file) {
            response->setContentType("application/octet-stream");
            response->setContentLength(file.size());
            response->sendHeaders();

            static uint8_t fs_chunk_buf[512];
            size_t len = 0;
            while (file.available()) {
              len = file.read(fs_chunk_buf, sizeof(fs_chunk_buf));
              response->sendChunk(fs_chunk_buf, len);
              if ((len & 0x3F) == 0) {
                delay(0);
              }
            }
            file.close();
            response->finishChunking();
          } else {
            return response->send(404);
          }
        } else {
          return response->send(404);
        }
      } else {
        return response->send(400);
      }
    } else {
      Log.warning(F("WEB : Unknown file system command." CR));
      return response->send(400);
    }
  } else {
    Log.warning(F("WEB : Unknown file system command." CR));
    return response->send(400);
  }
  return ESP_OK;
}

esp_err_t BaseWebServer::webHandleWifiScan(PsychicRequest *request) {
  PsychicResponse response(request);
  if (!isAuthenticated(request)) {
    return ESP_FAIL;
  }

  Log.notice(F("WEB : webServer callback for /api/wifi/scan." CR));
  _wifiScanTask = true;
  _wifiScanData = "";

  JsonDocument doc;
  doc[PARAM_SUCCESS] = true;
  doc[PARAM_MESSAGE] = "Scheduled wifi scanning";
  
  response.addHeader("Content-Type", "application/json");
  String jsonStr;
  serializeJson(doc, jsonStr);
  return response.send(200, "application/json", jsonStr.c_str());
}

esp_err_t BaseWebServer::webHandleWifiScanStatus(PsychicRequest *request) {
  PsychicResponse response(request);
  if (!isAuthenticated(request)) {
    return ESP_FAIL;
  }

  Log.notice(F("WEB : webServer callback for /api/wifi/scan/status." CR));

  if (_wifiScanTask || !_wifiScanData.length()) {
    JsonDocument doc;
    doc[PARAM_STATUS] = static_cast<bool>(_wifiScanTask);
    doc[PARAM_SUCCESS] = false;
    doc[PARAM_MESSAGE] =
        _wifiScanTask ? "Wifi scanning running" : "No scanning running";
    
    response.addHeader("Content-Type", "application/json");
    String jsonStr;
    serializeJson(doc, jsonStr);
    return response.send(200, "application/json", jsonStr.c_str());
  } else {
    response.setContent(_wifiScanData.c_str());
    response.setCode(200);
    return response.send();
  }

  return ESP_OK;
}

esp_err_t BaseWebServer::webHandleWifiClear(PsychicRequest *request) {
  PsychicResponse response(request);
  if (!isAuthenticated(request)) {
    return ESP_FAIL;
  }

  Log.notice(F("WEB : webServer callback for /api/wifi/clear." CR));
  
  JsonDocument doc;
  doc[PARAM_STATUS] = true;
  doc[PARAM_SUCCESS] = true;
  doc[PARAM_MESSAGE] = "WiFi credentials cleared";
  
  response.addHeader("Content-Type", "application/json");
  String jsonStr;
  serializeJson(doc, jsonStr);
  response.send(200, "application/json", jsonStr.c_str());
  
  // Schedule actual WiFi clear in a moment
  _rebootTimer = millis();
  _wifiSetup = true;  // Return to WiFi setup mode
  return ESP_OK;
}

esp_err_t BaseWebServer::webHandleRestart(PsychicRequest *request) {
  PsychicResponse response(request);
  if (!isAuthenticated(request)) {
    return ESP_FAIL;
  }

  Log.notice(F("WEB : webServer callback for /api/restart." CR));
  JsonDocument doc;
  doc[PARAM_STATUS] = true;
  doc[PARAM_SUCCESS] = true;
  doc[PARAM_MESSAGE] = "Restarting...";
  
  response.addHeader("Content-Type", "application/json");
  String jsonStr;
  serializeJson(doc, jsonStr);
  response.send(200, "application/json", jsonStr.c_str());
  _rebootTimer = millis();
  _rebootTask = true;
  return ESP_OK;
}

esp_err_t BaseWebServer::webHandlePing(PsychicRequest *request) {
  PsychicResponse response(request);
  Log.notice(F("WEB : webServer callback for /api/ping." CR));
  bool auth = false;

  if (request->hasHeader("Authorization")) {
    String authHeader = request->header("Authorization");
    if (authHeader == "Bearer " + _authToken) {
      Log.info(F("WEB : Authorized with token." CR));
      auth = true;
    }
  
    if(!isSslEnabled()) {
      String altToken("Bearer ");
      altToken += _webConfig->getID();
      if (authHeader == altToken) {
        Log.info(F("WEB : Authorized with device id." CR));
        auth = true;
      }
    }
  }

  JsonDocument doc;
  doc[PARAM_STATUS] = true;
  doc[PARAM_AUTHENTICATED] = auth;
  
  response.addHeader("Content-Type", "application/json");
  String jsonStr;
  serializeJson(doc, jsonStr);
  return response.send(200, "application/json", jsonStr.c_str());
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
                  std::placeholders::_1, std::placeholders::_2,
                  std::placeholders::_3));

  PsychicUploadHandler *firmwareUploadHandler = new PsychicUploadHandler();

  firmwareUploadHandler->onUpload((PsychicUploadCallback)std::bind(
      &BaseWebServer::webHandleUploadFirmware, this, std::placeholders::_1,
      std::placeholders::_2, std::placeholders::_3, std::placeholders::_4,
      std::placeholders::_5, std::placeholders::_6));
  _server->on("/api/firmware", HTTP_POST, firmwareUploadHandler);
  _server->on("*", HTTP_OPTIONS, [](PsychicRequest *request, PsychicResponse *response) {
    Log.notice(F("WEB : Got OPTIONS request url=%s." CR),
               request->url().c_str());
    return response->send(200);
  });
  _server->onNotFound([this](PsychicRequest *request, PsychicResponse *response) {
    return webHandlePageNotFound(request);
  });
}

bool BaseWebServer::setupWebServer(bool skipSSL, SerialWebSocket* serialWs, Print* secondary) {
  Log.notice(F("WEB : Configuring web server." CR));

  // Check for SSL certificates first
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

  if (isSslEnabled()) {
    // SSL certificates available - use HTTPS server
    PsychicHttpsServer *httpsServer = new PsychicHttpsServer();
    httpsServer->ssl_config.httpd.max_uri_handlers = 10;
    _server.reset(httpsServer);
    _server->config.max_uri_handlers = 15;
    _redirectServer.reset(new PsychicHttpServer());
  } else {
    // No SSL certificates - use plain HTTP
    _server.reset(new PsychicHttpServer());
    _server->config.max_uri_handlers = 15;
  }
#else
  _server.reset(new PsychicHttpServer());
  _server->config.max_uri_handlers = 15;
#endif

  // Start the server
#if defined(ESPFWK_PSYCHIC_SSL)
  if (isSslEnabled()) {
    Log.notice(F("WEB : Starting web server using SSL." CR));
    PsychicHttpsServer *httpsServer = static_cast<PsychicHttpsServer*>(_server.get());
    httpsServer->setCertificate(_sslCert.c_str(), _sslKey.c_str());
    _server->setPort(443);
    
    // Register WebSocket handler BEFORE starting server (required in v2.1.1)
    if (serialWs) {
      serialWs->begin(_server.get(), secondary);
    }
    
    setupWebHandlers();
    _server->start();

    _redirectServer->config.ctrl_port =
        20424;  // just a random port different from the default one
    _redirectServer->setPort(80);
    _redirectServer->onNotFound([](PsychicRequest *request, PsychicResponse *response) {
      Log.notice(F("WEB : Redirecting HTTP to HTTPS for %s." CR), request->url().c_str());
      String url = "https://" + request->host() + request->url();
      response->setCode(301);
      return response->redirect(url.c_str());
    });
    _redirectServer->start();
  } else {
    Log.notice(F("WEB : Starting web server without SSL." CR));
    _server->setPort(80);
    
    // Register WebSocket handler BEFORE starting server (required in v2.1.1)
    if (serialWs) {
      serialWs->begin(_server.get(), secondary);
    }
    
    setupWebHandlers();
    _server->start();
  }
#else
  Log.notice(F("WEB : Starting web server." CR));
  _server->setPort(80);
  _server->start();
  setupWebHandlers();
#endif

  MDNS.begin(_webConfig->getMDNS());
  MDNS.addService("http", "tcp", 80);
  MDNS.addServiceTxt("http", "tcp", "ver", CFG_APPVER);
  MDNS.addServiceTxt("http", "tcp", "app", CFG_APPNAME);
  MDNS.addServiceTxt("http", "tcp", "id", _webConfig->getID());

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
