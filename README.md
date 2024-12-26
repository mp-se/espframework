![release](https://img.shields.io/github/v/release/mp-se/espframework?label=latest%20release)
![issues](https://img.shields.io/github/issues/mp-se/espframework)
![pr](https://img.shields.io/github/issues-pr/mp-se/espframework)

# ESP framework

This is a framework that I use in my own ESP projects as a base. It contains a lot of basic features that are needed. It has been tested with the following targets: ESP8266, ESP32, ESP32S2, ESP32C3, ESP32S3

## Structure

The master branch dones not contain any code. There are a number of version of the framework which is stored in seprate branches for easy maintenance

### Branch 1.0 (Deprecated)

- This version is no longer maintained and its no longer the primary version for my projects.

### Branch 1.1 (Primary branch)

- Updated with newer version of the dependant libraries like Arduino Json 7 and Async Webserver
- ArduinoLog and incbin has now been merged into the library for easier maintenance since they are no longer maintained by their authors.

### Branch 2.0 (Experimental)

- Based on 1.1 this is migreted to work with Arduino 3.x
