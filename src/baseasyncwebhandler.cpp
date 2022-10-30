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
#include <baseconfig.hpp>
#include <baseasyncwebhandler.hpp>
#include <espframework.hpp>
#include <log.hpp>

#if defined(ESP8266)
#include <LittleFS.h>
#define MAX_SKETCH_SPACE 1044464
#else
#include <FS.h>
#include <HTTPUpdate.h>
#include <LittleFS.h>
#define MAX_SKETCH_SPACE 1835008
#endif

BaseAsyncWebHandler::BaseAsyncWebHandler(WebConfig* config, int dynamicJsonSize) {
  _webConfig = config;
  _dynamicJsonSize = dynamicJsonSize;
}

void BaseAsyncWebHandler::loop() {
  if (!_server) return;

#if defined(ESP8266)
  MDNS.update();
#endif

  if (_reboot) {
    if (abs((int32_t)(millis() - _rebootTimer)) > 1000) {
      ESP_RESET();
    }
  }
}

void BaseAsyncWebHandler::webHandleConfigRead(AsyncWebServerRequest *request) {
  Log.notice(F("WEB : BaseWebHandler callback for /api/config(get)." CR));

  DynamicJsonDocument doc(_dynamicJsonSize);
  _webConfig->createJson(doc, true); // will not include ssid passwords

  String out;
  out.reserve(_dynamicJsonSize);
  serializeJson(doc, out);
  doc.clear();
  request->send(200, "application/json", out.c_str());
}

void BaseAsyncWebHandler::webHandleConfigWrite(AsyncWebServerRequest *request) {
  String id = request->arg(PARAM_ID);
  Log.notice(F("WEB : BaseWebHandler callback for /api/config(write)." CR));

  if (!id.equalsIgnoreCase(_webConfig->getID())) {
    Log.error(F("WEB : Wrong ID received %s, expected %s" CR), id.c_str(),
              _webConfig->getID());
    request->send(400, "text/plain", "Invalid ID.");
    return;
  }

  DynamicJsonDocument doc(_dynamicJsonSize);

  // Mapping post format to json for parsing in config class
  for (int i = 0; i < request->args(); i++) {
    String arg = request->argName(i);

    if (!arg.compareTo("plain") || !arg.compareTo(PARAM_ID) ||
        !arg.compareTo(PARAM_PASS) || !arg.compareTo(PARAM_PASS2) ||
        !arg.compareTo(PARAM_SSID) || !arg.compareTo(PARAM_SSID2)) {
      Log.verbose(F("WEB : Skipping param %ss" CR), arg.c_str());
    } else {
      Log.verbose(F("WEB : Adding param %s=%s" CR), arg.c_str(),
                  request->arg(i).c_str());
      doc[arg] = request->arg(i);
    }
  }

  _webConfig->parseJson(doc);
  doc.clear();
  _webConfig->saveFile();

  String path = "/config.htm";

  if (request->hasArg("section")) path += request->arg("section").c_str();

  AsyncWebServerResponse *response = request->beginResponse(302, "text/plain", "Config saved"); 
  response->addHeader("Location", path);
  request->send(response);
}

void BaseAsyncWebHandler::webHandleUploadFile(AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final) {
  uint32_t maxSketchSpace = MAX_SKETCH_SPACE;
  Log.verbose(F("WEB : BaseWebHandler callback for /api/upload(post)." CR));

  if(!index){
    _uploadedSize = 0;
    Update.runAsync(true);
    if (!Update.begin(maxSketchSpace, U_FLASH, PIN_LED)) {
      _uploadReturn = 500;
      Log.error(F("WEB : Not enough space to store for this firmware." CR));
    } else {
      _uploadReturn = 200;
      Log.notice(F("WEB : Start firmware upload, max sketch size %d kb." CR),
                 maxSketchSpace / 1024);
    }
  }

  _uploadedSize += len;

  if (_uploadedSize > maxSketchSpace) {
    _uploadReturn = 500;
    Log.error(F("WEB : Firmware file is to large." CR));
  } else if (Update.write(data, len) != len) {
    _uploadReturn = 500;
    Log.notice(F("WEB : Writing firmware upload %d (%d)." CR),
                len, maxSketchSpace);
  } else {
    EspSerial.print(".");
  }

  if(final){
    EspSerial.print("\n");
    Log.notice(F("WEB : Finished firmware upload." CR));
    request->send(200);

    if (Update.end(true)) {
      // Calling reset here will not wait for all the data to be sent, lets wait a second before rebooting in loop.
      _rebootTimer = millis();
      _reboot = true;
    } else {
      Log.error(F("WEB : Failed to finish firmware flashing" CR));
      _uploadReturn = 500;
    }
  }
}

void BaseAsyncWebHandler::webHandlePageNotFound(AsyncWebServerRequest *request) {
  Log.error(F("WEB : URL not found %s received." CR), request->url().c_str());
  request->send(404, "text/plain", F("URL not found"));
}

void BaseAsyncWebHandler::setupAsyncWebHandlers() {
  if (!_server) return;

  MDNS.begin(_webConfig->getMDNS());

  Log.notice(F("WEB : Setting up async web handlers." CR));
  _server->on("/", HTTP_GET, std::bind(&BaseAsyncWebHandler::webReturnIndexHtm, this, std::placeholders::_1));
  _server->on("/index.htm", HTTP_GET,
              std::bind(&BaseAsyncWebHandler::webReturnIndexHtm, this, std::placeholders::_1));
  _server->on("/config.htm", HTTP_GET,
              std::bind(&BaseAsyncWebHandler::webReturnConfigHtm, this, std::placeholders::_1));
  _server->on("/about.htm", HTTP_GET,
              std::bind(&BaseAsyncWebHandler::webReturnAboutHtm, this, std::placeholders::_1));
  _server->on("/upload.htm", HTTP_GET,
              std::bind(&BaseAsyncWebHandler::webReturnUploadHtm, this, std::placeholders::_1));
  _server->on("/api/config", HTTP_GET,
              std::bind(&BaseAsyncWebHandler::webHandleConfigRead, this, std::placeholders::_1));
  _server->on("/api/upload", HTTP_POST,
              std::bind(&BaseAsyncWebHandler::webReturnOK, this, std::placeholders::_1),
              std::bind(&BaseAsyncWebHandler::webHandleUploadFile, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5, std::placeholders::_6));
  _server->on("/api/config", HTTP_POST,
              std::bind(&BaseAsyncWebHandler::webHandleConfigWrite, this, std::placeholders::_1));
  _server->onNotFound(std::bind(&BaseAsyncWebHandler::webHandlePageNotFound, this, std::placeholders::_1));
}

bool BaseAsyncWebHandler::setupAsyncWebServer() {
  Log.notice(F("WEB : Configuring web server." CR));

  _server = new AsyncWebServer(80);

  MDNS.begin(_webConfig->getMDNS());
  MDNS.addService("http", "tcp", 80);

#if defined(ESP8266)
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
#else
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
#endif

  setupAsyncWebHandlers();
  _server->begin();
  Log.notice(F("WEB : Web server started." CR));
  return true;
}

// EOF
