#include <AUnit.h>
#include <jsonfs.hpp>
#include <LittleFS.h>
#include <ArduinoJson.h>

test(JsonFS_SaveAndLoad) {
  LittleFS.begin();
  const char* filename = "/test.json";
  LittleFS.remove(filename);

  // Save JSON
  JsonFileSystemHelper helper(filename);
  JsonDocument doc;
  doc["foo"] = 42;
  doc["bar"] = "baz";
  bool saved = helper.saveJson(doc);
  assertTrue(saved);
  assertTrue(LittleFS.exists(filename));

  // Load JSON
  JsonDocument doc2;
  bool loaded = helper.loadJson(doc2);
  assertTrue(loaded);
  assertEqual(doc2["foo"].as<int>(), 42);
  assertEqual(String(doc2["bar"].as<const char*>()), String("baz"));

  LittleFS.remove(filename);
  LittleFS.end();
}

test(JsonFS_LoadNonexistent) {
  LittleFS.begin();
  const char* filename = "/doesnotexist.json";
  LittleFS.remove(filename);
  JsonFileSystemHelper helper(filename);
  JsonDocument doc;
  bool loaded = helper.loadJson(doc);
  assertFalse(loaded);
  LittleFS.end();
}
// EOF
