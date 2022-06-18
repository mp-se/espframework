import unittest, requests, json, time

# python3 -m unittest -v testcase_api.py -v
# python3 -m unittest -v testcase_api.API.test_config_post_write

host = "192.168.1.200"
id   = "7376ef"
ver  = "0.0.5"

def call_api_post( path, json ):
    url = "http://" + host + path
    return requests.post( url, data=json )

def call_api_get( path ):
    url = "http://" + host + path
    return requests.get( url )

class API(unittest.TestCase):

    def test_config_wifi_read(self):
        r = call_api_get( "/api/config" )
        j = json.loads(r.text)
        self.assertNotEqual(j["mdns"], "hello")
        self.assertNotEqual(j["wifi-ssid"], "hello")
        self.assertNotEqual(j["wifi-ssid2"], "hello")
        self.assertNotEqual(j["wifi-pass"], "hello")
        self.assertNotEqual(j["wifi-pass2"], "hello")

    def test_config_wifi_write(self):
        j = {"id":id,"mdns":"testing","wifi-ssid":"skip","wifi-pass":"skip","wifi-ssid2":"skip","wifi-pass2":"skip"}
        r = call_api_post( "/api/config", j )
        self.assertEqual(r.status_code, 200)

        r = call_api_get( "/api/config" )
        j = json.loads(r.text)
        self.assertEqual(j["mdns"], "testing")
        # These should be skipped from web interface
        self.assertNotEqual(j["wifi-ssid"], "skip")
        self.assertNotEqual(j["wifi-ssid2"], "skip")
        self.assertNotEqual(j["wifi-pass"], "skip")
        self.assertNotEqual(j["wifi-pass2"], "skip")

    def test_config_ota_read(self):
        r = call_api_get( "/api/config" )
        j = json.loads(r.text)
        self.assertNotEqual(j["ota-url"], "hello")

    def test_config_ota_write(self):
        j = {"id":id,"ota-url":"http://skip.com"}
        r = call_api_post( "/api/config", j )
        self.assertEqual(r.status_code, 200)

        r = call_api_get( "/api/config" )
        j = json.loads(r.text)
        self.assertEqual(j["ota-url"], "http://skip.com")

    def test_config_base_read(self):
        r = call_api_get( "/api/config" )
        j = json.loads(r.text)
        self.assertNotEqual(j["temp-format"], "h")

    def test_config_base_write(self):
        j = {"id":id,"temp-format":"F"}
        r = call_api_post( "/api/config", j )
        self.assertEqual(r.status_code, 200)

        r = call_api_get( "/api/config" )
        j = json.loads(r.text)
        self.assertEqual(j["temp-format"], "F")

        j = {"id":id,"temp-format":"C"}
        r = call_api_post( "/api/config", j )
        self.assertEqual(r.status_code, 200)

        r = call_api_get( "/api/config" )
        j = json.loads(r.text)
        self.assertEqual(j["temp-format"], "C")

    def test_config_post_read(self):
        r = call_api_get( "/api/config" )
        j = json.loads(r.text)
        self.assertNotEqual(j["http-post-target"], "hello")
        self.assertNotEqual(j["http-post-header1"], "hello")
        self.assertNotEqual(j["http-post-header1"], "hello")
        self.assertNotEqual(j["http-get-target"], "hello")
        self.assertNotEqual(j["http-get-header1"], "hello")
        self.assertNotEqual(j["http-get-header2"], "hello")
        self.assertNotEqual(j["influxdb2-target"], "hello")
        self.assertNotEqual(j["influxdb2-org"], "hello")
        self.assertNotEqual(j["influxdb2-bucket"], "hello")
        self.assertNotEqual(j["influxdb2-token"], "hello")
        self.assertNotEqual(j["mqtt-target"], "hello")
        self.assertNotEqual(j["mqtt-port"], 1234)
        self.assertNotEqual(j["mqtt-user"], "hello")
        self.assertNotEqual(j["mqtt-pass"], "hello")

    def test_config_post_write(self):
        j = {"id":id,"http-post-target":"http://skip.com", "http-post-header1": "header1", "http-post-header2": "header2", "http-get-target": "http://skip.com", "http-get-header1": "header3", "http-get-header2": "header4", 
        "influxdb2-target": "http://skip.com", "influxdb2-org": "org", "influxdb2-bucket": "bucket", "influxdb2-token": "token", "mqtt-target": "skip.com", "mqtt-port": 234, "mqtt-user": "user", "mqtt-pass": "pass" }
        r = call_api_post( "/api/config", j )
        self.assertEqual(r.status_code, 200)

        r = call_api_get( "/api/config" )
        j = json.loads(r.text)
        self.assertEqual(j["http-post-target"], "http://skip.com")
        self.assertEqual(j["http-post-header1"], "header1")
        self.assertEqual(j["http-post-header2"], "header2")
        self.assertEqual(j["http-get-target"], "http://skip.com")
        self.assertEqual(j["http-get-header1"], "header3")
        self.assertEqual(j["http-get-header2"], "header4")
        self.assertEqual(j["influxdb2-target"], "http://skip.com")
        self.assertEqual(j["influxdb2-org"], "org")
        self.assertEqual(j["influxdb2-bucket"], "bucket")
        self.assertEqual(j["influxdb2-token"], "token")
        self.assertEqual(j["mqtt-target"], "skip.com")
        self.assertEqual(j["mqtt-port"], 234)
        self.assertEqual(j["mqtt-user"], "user")
        self.assertEqual(j["mqtt-pass"], "pass")

if __name__ == '__main__':
    unittest.main()

