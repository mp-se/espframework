/*
MIT License

Copyright (c) 2021-22 Magnus

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
#include <LittleFS.h>

#include <baseconfig.hpp>
#include <basewebhandler.hpp>
#include <espframework.hpp>

#define MAX_SKETCH_SPACE 1044464

BaseWebHandler::BaseWebHandler(WebConfig* config, bool wifiSetup) { _webConfig = config; _wifiSetup = wifiSetup; }

void BaseWebHandler::loop() {
  // TEMPORARY MDNS.update(); 
  _server->handleClient();
}

void BaseWebHandler::webHandleConfigRead() {
  Log.notice(F("WEB : BaseWebHandler callback for /api/config(get)." CR));

  DynamicJsonDocument doc(2048);
  _webConfig->createJson(doc, true); // will not include ssid passwords

#if LOG_LEVEL == 6
  serializeJson(doc, Serial);
  Serial.print(CR);
#endif

  String out;
  out.reserve(2048);
  serializeJson(doc, out);
  _server->send(200, "application/json", out.c_str());
}

void BaseWebHandler::webHandleConfigWrite() {
  String id = _server->arg(PARAM_ID);
  Log.notice(F("WEB : BaseWebHandler callback for /api/config(write)." CR));

  if (!id.equalsIgnoreCase(_webConfig->getID())) {
    Log.error(F("WEB : Wrong ID received %s, expected %s" CR), id.c_str(),
              _webConfig->getID());
    _server->send(400, "text/plain", "Invalid ID.");
    return;
  }

  DynamicJsonDocument doc(2048);

  // Mapping post format to json for parsing in config class
  for (int i = 0; i < _server->args(); i++) {
    String arg = _server->argName(i);

    if (!arg.compareTo("plain") || !arg.compareTo(PARAM_ID) ||
        !arg.compareTo(PARAM_PASS) || !arg.compareTo(PARAM_PASS2) ||
        !arg.compareTo(PARAM_SSID) || !arg.compareTo(PARAM_SSID2)) {
      Log.verbose(F("WEB : Skipping param %ss" CR), arg.c_str());
    } else {
      Log.verbose(F("WEB : Adding param %s=%s" CR), arg.c_str(),
                  _server->arg(i).c_str());
      doc[arg] = _server->arg(i);
    }
  }

#if LOG_LEVEL == 6
  serializeJson(doc, Serial);
  Serial.print(CR);
#endif

  _webConfig->parseJson(doc);
  _webConfig->saveFile();

  String path = "/config.htm";

  if (_server->hasArg("section")) path += _server->arg("section").c_str();

  _server->sendHeader("Location", path, true);
  _server->send(302, "text/plain", "Config saved");
}

void BaseWebHandler::webHandleUploadFile() {
  Log.verbose(F("WEB : BaseWebHandler callback for /api/upload(post)." CR));
  HTTPUpload& upload = _server->upload();
  String f = upload.filename;
  bool firmware = false;

  if (f.endsWith(".bin")) {
    firmware = true;
  }

#if LOG_LEVEL == 6
  Log.verbose(
      F("WEB : webServer callback for /api/upload, receiving file %s, %d(%d) "
        "firmware=%s." CR),
      f.c_str(), upload.currentSize, upload.totalSize, firmware ? "yes" : "no");
#endif

  if (firmware) {
    // Handle firmware update, hardcode since function return wrong value.
    // (ESP.getFreeSketchSpace() - 0x1000) & 0xFFFFF000;
    uint32_t maxSketchSpace = MAX_SKETCH_SPACE;

    if (upload.status == UPLOAD_FILE_START) {
      _uploadReturn = 200;
      Log.notice(F("WEB : Start firmware upload, max sketch size %d kb." CR),
                 maxSketchSpace / 1024);

      if (!Update.begin(maxSketchSpace, U_FLASH, PIN_LED)) {
        Log.error(F("WEB : Not enough space to store for this firmware." CR));
        _uploadReturn = 500;
      }
    } else if (upload.status == UPLOAD_FILE_WRITE) {
      Log.notice(F("WEB : Writing firmware upload %d (%d)." CR),
                 upload.totalSize, maxSketchSpace);

      if (upload.totalSize > maxSketchSpace) {
        Log.error(F("WEB : Firmware file is to large." CR));
        _uploadReturn = 500;
      } else if (Update.write(upload.buf, upload.currentSize) !=
                 upload.currentSize) {
        Log.warning(F("WEB : Firmware write was unsuccessful." CR));
        _uploadReturn = 500;
      }
    } else if (upload.status == UPLOAD_FILE_END) {
      Log.notice(F("WEB : Finish firmware upload." CR));
      if (Update.end(true)) {
        _server->send(200);
        delay(500);
        ESP_RESET();
      } else {
        Log.error(F("WEB : Failed to finish firmware flashing error=%d" CR),
                  Update.getError());
        _uploadReturn = 500;
      }
    } else {
      Update.end();
      Log.notice(F("WEB : Firmware flashing aborted." CR));
      _uploadReturn = 500;
    }

    delay(0);

  } else {
    // Handle file upload
    if (upload.status == UPLOAD_FILE_START) {
      _uploadReturn = 200;
      Log.notice(F("WEB : Start file upload." CR));
      _uploadFile = LittleFS.open(f, "w");
    } else if (upload.status == UPLOAD_FILE_WRITE) {
      Log.notice(F("WEB : Writing file upload." CR));
      if (_uploadFile) _uploadFile.write(upload.buf, upload.currentSize);
    } else if (upload.status == UPLOAD_FILE_END) {
      Log.notice(F("WEB : Finish file upload." CR));
      if (_uploadFile) {
        _uploadFile.close();
        Log.notice(F("WEB : File uploaded %d bytes." CR), upload.totalSize);
      }
      _server->sendHeader("Location", "/");
      _server->send(303);
    } else {
      _server->send(500, "text/plain", "Could not upload file.");
    }
  }
}

void BaseWebHandler::webHandlePageNotFound() {
  Log.error(F("WEB : URL not found %s received." CR), _server->uri().c_str());
  _server->send(404, "text/plain", F("URL not found"));
}

void BaseWebHandler::setupWebHandlers() {
  if (_wifiSetup) {
    Log.notice(F("WEB : Setting up web handlers (wifi config)." CR));
    _server->on("/", std::bind(&BaseWebHandler::webReturnWifiHtm, this));
  } else {
    Log.notice(F("WEB : Setting up web handlers (normal)." CR));
    _server->on("/", std::bind(&BaseWebHandler::webReturnIndexHtm, this));
    _server->on("/index.htm",
                std::bind(&BaseWebHandler::webReturnIndexHtm, this));
    _server->on("/config.htm",
                std::bind(&BaseWebHandler::webReturnConfigHtm, this));
    _server->on("/about.htm",
                std::bind(&BaseWebHandler::webReturnAboutHtm, this));
    _server->on("/upload.htm",
                std::bind(&BaseWebHandler::webReturnUploadHtm, this));
    _server->on("/api/config", HTTP_GET,
                std::bind(&BaseWebHandler::webHandleConfigRead, this));
    _server->on("/api/upload", HTTP_POST,
                std::bind(&BaseWebHandler::webReturnOK, this),
                std::bind(&BaseWebHandler::webHandleUploadFile, this));
    _server->on("/api/config", HTTP_POST,
                std::bind(&BaseWebHandler::webHandleConfigWrite, this));
    /* Not used since they consume to much space on an ESP8266
    _server->on("/bootstrap.css",
                std::bind(&BaseWebHandler::webReturnBoostrapCss, this));
    _server->on("/bootstrap.js",
                std::bind(&BaseWebHandler::webReturnBoostrapJs, this));
    _server->on("/jquery.js",
                std::bind(&BaseWebHandler::webReturnJqueryJs, this));*/
  }

  _server->onNotFound(std::bind(&BaseWebHandler::webHandlePageNotFound, this));
}

bool BaseWebHandler::setupWebServer() {
  Log.notice(F("WEB : Configuring web server." CR));

  _server = new ESP8266WebServer();

  MDNS.begin(_webConfig->getMDNS());
  MDNS.addService("http", "tcp", 80);

  FSInfo fs;
  LittleFS.info(fs);
  Log.notice(F("WEB : File system Total=%d, Used=%d." CR), fs.totalBytes,
             fs.usedBytes);
  Dir dir = LittleFS.openDir("/");
  while (dir.next()) {
    Log.notice(F("WEB : File=%s, %d bytes" CR), dir.fileName().c_str(),
               dir.fileSize());
    if (!dir.fileSize()) {
      Log.notice(F("WEB : Empty file detected, removing file." CR));
      LittleFS.remove(dir.fileName().c_str());
    }
  }

  setupWebHandlers();
  _server->begin();
  Log.notice(F("WEB : Web server started." CR));
  return true;
}

// EOF
