![release](https://img.shields.io/github/v/release/mp-se/espframework?label=latest%20release)
![issues](https://img.shields.io/github/issues/mp-se/espframework)
![pr](https://img.shields.io/github/issues-pr/mp-se/espframework)

# ESP framework

This is a framework that I use in my own ESP projects as a base. It contains a lot of basic features that are needed. It has been tested with the following targets: ESP8266, ESP32, ESP32S2, ESP32C3, ESP32S3

- [ESP framework](#esp-framework)
  - [Features](#features)
- [Code samples](#code-samples)
  - [class BaseConfig](#class-baseconfig)
  - [class SerialDebug](#class-serialdebug)
  - [class SerialWebSocket](#class-serialwebsocket)
  - [class WifiConnection](#class-wificonnection)
  - [class OtaUpdate](#class-otaupdate)
  - [class BaseAsyncWebHandler and BaseWebHandler](#class-baseasyncwebhandler-and-basewebhandler)
  - [class TemplatingEngine](#class-templatingengine)
  - [class BasePush](#class-basepush)
  - [class PerfLogging](#class-perflogging)
  - [Misc helper methods](#misc-helper-methods)
- [Credits](#credits)

## Supported targets

- ESP8266
- ESP32
- ESP32S2
- ESP32C3
- ESP32S3

## Features

1. **WIFI Connection**
  
    WIFI connection handler including a wifi configuration manager. You can configure up to two WIFI networks and the device will switch between them if there are connection errors. This also include a 
  double reset detector  for entering the WIFI setup mode.

2. **OTA functionallity**

    Checks for a newer software version on the provided URL. If found it will perform an update during the boot sequence.

3. **Web Server (Sync and Async)**

    Basic web server functionallity (support both async and sync versions). Async is faster but requires long running tasks to be run in the loop or the watchdog will trigger.

    [How to work with AsynWebServer](https://github.com/me-no-dev/ESPAsyncWebServer)
    
    All web pages will be embedded into the binary so they will not consume any space on the file system and it also ensure that the right html files are included.

    There are a number of html pages included by default and you can add as many pages as you want (or whats fit into memory).
    * Index.htm (Main page)
    * Config.htm (For configuration/settings)
    * Upload.htm (For manually uploading new firmware)
    * About.htm  (About page)
    * Serial console

    The example files uses jquery and bootstrap but you can choose to customize these in any way that you want.

3. **Configuration**

    Extensible configuration class that persists data on the file system on the device. 
    
    [How to work with JsonDocuments](https://github.com/bblanchon/ArduinoJson)

4. **Logging**

    Logging APIs that can be used to log data to serial port, tx/rx pins and websocket (web page). Support multiple logging levels. 
    
    [How to do logging](https://github.com/thijse/Arduino-Log)

5. **Templating**
   
    Use a template string and transform that using data labels. Perfect for creating payloads for pushing data to external services.

6. **Push**

    Pushing data to external endpoints using either; HTTP GET, HTTP POST, INFLUXDB v2 or MQTT. Supports SSL.

7. **Performance profiling**

    Profile code segments and push result to an influx database for later analysis.

8. **Utils**

    Conversion functions between volumes, weight and temperature.

# Code samples
There is an example application with all included files for platformio. So this section will highlight the main parts for using the framework. It consists of 3 parts:
* src - Source and Header files for the classes
* html - HTML files that needs to be Minified (made smaller). See VSCODE Minify plugin.
* test - Example json files that can be used to test html parts. 
* platformio.ini - Build file for platformio in VSCODE

## class BaseConfig
This is a base class of the framework that stores all configuration parameters. It has a few parameters implemented but you need to extend this to store your own custom paramters. It handles read / write and parsing of the pre-defined parameters.

You can create your own class that extends this one. This way you can add your own parameters and still use the basic functionallity. You can implement the get/set methods for your own parameters and create / parse the corresponding variables. See the example classes for more information.

[How to work with JsonDocuments](https://github.com/bblanchon/ArduinoJson)

```cpp
class DemoConfig : public BaseConfig {
 private:
  // Add your own variables here
  String _param;

 public:
  DemoConfig(String baseMDNS, String fileName);

  void createJson(DynamicJsonDocument& doc, bool skipSecrets = true);
  void parseJson(DynamicJsonDocument& doc);

  // Add your get set methods
  void setParam(String& p);
  String getParam();
};
```

This is an basic implementation, you need to call the relevant methods in the base class in order to extract the basic configuration settings that you want to use.
```cpp
DemoConfig::DemoConfig(String baseMDNS, String fileName)
    : BaseConfig(baseMDNS, fileName) {}

void DemoConfig::createJson(DynamicJsonDocument& doc, bool skipSecrets) {
  // Call base class functions
  createJsonBase(doc, skipSecrets);
  createJsonWifi(doc, skipSecrets);
  createJsonOta(doc, skipSecrets);
  createJsonPush(doc, skipSecrets);

  // Handle project specific config
}

void DemoConfig::parseJson(DynamicJsonDocument& doc) {
  // Call base class functions
  parseJsonBase(doc);
  parseJsonWifi(doc);
  parseJsonOta(doc);
  parseJsonPush(doc);

  // Handle project specific config
}
```

Create a global instance of you configuration class, the parameters are the mdns name and path to where the configuration is stored.
```cpp
DemoConfig myConfig("mdnsbase", "/esplib.cfg");
```

There are two init methods that needs to be called to initialize the configuration properly.
```cpp
void setup() {
  myConfig.checkFileSystem(); // Check and format filesystem if needed
  myConfig.loadFile(); // Load from disk
}
```

## class SerialDebug
This is a wrapper that handles the initialization of the log output. Default is using the serial port but it can also be configured to use TX/RX pins on the chip so you can read the serial log if the device is running on battery power (you need a RS232 -> USB cable for this to work). 


You can set the default log level in the platformio configuration file.
```cpp
-D LOG_LEVEL=5
```

This define will redirect the serial console to the TX/RX pins of the device. Useful when debugging a device on battery power.
```cpp
-D USE_SERIAL_PINS
```

If you want to use the websocket serial this needs to be configured once webserver is started and this is shown as a seprate example.
```cpp
SerialDebug mySerial(115200L); // Serial speed as param
```

Logging examples with support for printf() parameters. 

See: https://github.com/thijse/Arduino-Log for more examples on logging.
```cpp
Log.warning(F("Log entry %d" CR), _integer);
Log.error(F("Log entry %d" CR), _integer);
Log.notice(F("Log entry %d" CR), _integer);
```

## class SerialWebSocket
Creating a global instance for serial websocket listener
```cpp
SerialWebSocket mySerialWebSocket;
```

To connect the serial websocket you need to get the webserver pointer from the async webhandler. The second param allows you to specify a secondary port that data is also written to, for example sending data to the serial port as well as the WebSocket.
```cpp
// This example will stream the serial console both to hardware serial and the 
// websocket (NULL will disable that port). 
void setup() {
  mySerialWebSocket.begin(myAsyncWebHandler.getWebServer(), &Serial);
  mySerial.begin(&mySerialWebSocket); // Reinitialzie the logging class with web socket as the primary output.
}

void loop() {
  mySerialWebSocket.loop(); // Will run the cleanup tasks such as removing closed client.
}

```

## class WifiConnection
For WIFI access create a global variable of the wifi class.
```cpp
// It requires a couple of paramaters. Reference to your configuration class, access 
// point (ssid + password), hostname/mdns. The last two can be used to hardcode wifi 
// ssid and password if you dont wan't to use the wifi manager. Empty string will disable 
// that function 

WifiConnection myWifi(&myConfig, "espSSID", "password", "esplib", "", "");
```

The following code will initialize the wifi setup but also check the double reset feature. 
```cpp
void setup() {
  myWifi.init();
  if (!myWifi.hasConfig() || myWifi.isDoubleResetDetected()) {
    myWifi.startPortal(); // Start the wifi manager
  }

  myWifi.connect(); // Perform the connection
  myWifi.timeSync(); // Do an NTP sync (not required)
}

void loop() {
  myWifi.loop(); // Will run the background tasks
}
```

## class OtaUpdate
The is how you enable the OTA feature. Create a global instance.
```cpp
// Params is your configuration class and the current version string in format major.minor.patcj

OtaUpdate myOta(&myConfig, "0.0.0");
```

This will check if the wifi is connected and if a newer version exist
```cpp
void setup() {
  if (!myWifi.isConnected() || myOta.checkFirmwareVersion()) {
    myOta.updateFirmware(); // Perform OTA
  }
}
```

## class BaseAsyncWebHandler and BaseWebHandler
To create a webserver you need to create your own class that inherits from the base class. This example shows
the async server but the structure is simular for the standard web handler.

If you want to use the AsyncWebServer you need to add this define to the platformio.ini file.
```cpp
-D USE_ASYNC_WEB
```

```cpp
class DemoAsyncWebHandler : public BaseAsyncWebHandler {
 private:
  DemoPush *_push;

  // Method to return a html page stored in memory
  void webReturnTestHtm(AsyncWebServerRequest *request) {
    request->send_P(200, "text/html", (const uint8_t *)testHtmStart,
                    strlen(reinterpret_cast<const char *>(&testHtmStart[0])));
  }

  void setupAsyncWebHandlers();

  // Method to handle incoming request for a defined endpoint
  void webHandleStatus(AsyncWebServerRequest *request);

 public:
  explicit DemoAsyncWebHandler(WebConfig *config, DemoPush *push);
};
```

```cpp
DemoAsyncWebHandler::DemoAsyncWebHandler(WebConfig *config, DemoPush *push)
    : BaseAsyncWebHandler(config) {
  _push = push;
}

void DemoAsyncWebHandler::setupAsyncWebHandlers() {
  BaseAsyncWebHandler::setupAsyncWebHandlers();

  // Add bindings for various endpoints
  _server->on("/test.htm", std::bind(&DemoAsyncWebHandler::webReturnTestHtm, 
      this, std::placeholders::_1));
  _server->on("/api/status", HTTP_GET, std::bind(&DemoAsyncWebHandler::webHandleStatus, 
      this, std::placeholders::_1));
}
```

Create a global instance of your class
```cpp
DemoWebHandler myWebHandler(&myConfig, &myPush);
```

Initialize and run the webserver.
```cpp
void setup() {
  if (myWifi.isConnected()) {
    myAsyncWebHandler.setupAsyncWebServer();
  }
}

void loop() {
  myAsyncWebHandler.loop();
}
```

## class TemplatingEngine
This can be used to transform a template into a document with valid details, for instance sending data to an external service. It bascially an advanced string replace function that is optimized for low memory consumption.
```cpp
TemplatingEngine engine;
const char *template = "{ 'name': '${mdns}' }";

engine.setVal("${mdns}", myConfig.getMDNS());
const char *out = engine.create(template);
```

## class BasePush
This can be used to send data to an external endpoint. The configurtion parameters are stored in the configuration class and is fetched from there.
```cpp
BasePush push(&myConfig);

push.sendHttpPost("Payload"); // Send to configured http post
push.sendHttpGet("Payload");  // Send to configured http get
push.sendInfluxDb2("Payload"); // Send to configured influxdb2
push.sendMqtt("Topic:payload|Topic2:payload2|"); // Send to mqtt, can contain multiple topics and payloads. 
```

## class PerfLogging
Used to collect profiling data and send that to an influx database.

In order to enable this feature you need to create a number of defines in the plataformio.ini file for the target. This also includes the parameters needed for the influxdb interaction. Influxdb v2 is assumed.

```cpp
-D PERF_ENABLE // Will activate the code for this featurs
-D PERF_INFLUX_TARGET=\""\""
-D PERF_INFLUX_BUCKET=\""\""
-D PERF_INFLUX_ORG=\""\""
-D PERF_INFLUX_TOKEN=\""\""
```

Creating an instance of the class.
```cpp
PerfLogging perf;
perf.getInstance().setBaseConfig(&myConfig);
```

Using the macros for collecting data
```cpp
PERF_BEGIN(id); // Start measuring with the defined ID (ID needs to be unique)
PERF_END(id); // Stop measuring with the defined ID
PERF_CLEAR(); // Remove all data
PERF_PUSH(); // Send data to server
```

```cpp
PERF_BEGIN("read-temp"); 
  // Do some stuff here that you want to measure. 
PERF_END("read-temp"); 

PERF_PUSH(); // Push the data once you have collected enough, or you want to start a new loop
```

## Misc helper methods
Format conversion methods
```cpp
float convertCtoF(float c);
float convertFtoC(float f);
float convertKGtoLBS(float kg);
float convertLBStoKG(float lbs);
float convertCLtoUSOZ(float cl);
float convertCLtoUKOZ(float cl);
float convertUSOZtoCL(float usoz);
float convertUKOZtoCL(float ukoz);
```

Create strings with correct number of decimals
```cpp
float reduceFloatPrecision(float f, int dec);
char* convertFloatToString(float f, char* buffer, int dec);
```

Other helper methods
```cpp
void tcp_cleanup(); // Do cleanup on lingering tcp connections that can cause crash on esp8266
void deepSleep(int t); // Sleep for t seconds.
void printHeap(String prefix); // Print out current heap information to logger
void ledOn(LedColor l = LedColor::WHITE); // Set the builtin led, supports RGB leds on C3/S3 boards
void ledOff();
```

# Credits
This library is based on the following projects, without the work of these authors this would have been impossible.

- [https://github.com/mp-se/Arduino-Log](https://github.com/mp-se/Arduino-Log)
- [https://github.com/mp-se/ArduinoJson](https://github.com/mp-se/ArduinoJson)
- [https://github.com/mp-se/arduino-mqtt](https://github.com/mp-se/arduino-mqtt)
- [https://github.com/mp-se/incbin](https://github.com/mp-se/incbin)
- [https://github.com/mp-se/ESPAsyncWebServer](https://github.com/mp-se/ESPAsyncWebServer)
- [https://github.com/mp-se/ESPAsyncTCP](https://github.com/mp-se/ESPAsyncTCP)
- [https://github.com/esp8266/Arduino](https://github.com/esp8266/Arduino)
- [https://github.com/espressif/arduino-esp32](https://github.com/espressif/arduino-esp32)
