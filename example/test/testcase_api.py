import unittest, requests, json, time

# python3 -m unittest -v testcase_api.py -v
# python3 -m unittest -v testcase_api.API.test_config_1

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

    # Check that all parameters exist, will require a full build with all sensors
    #def test_config(self):
    #    r = call_api_get( "/api/config" )
    #    j = json.loads(r.text)

    # Check that all parameters exist
    def test_config_1(self):
        j = {"id":"7376ef","temp-format":"F","mdns":"testing","wifi-ssid":"skip","wifi-pass":"skip","wifi-ssid2":"skip","wifi-pass2":"skip","ota-url":"testing"}
        r = call_api_post( "/api/config", j )
        self.assertEqual(r.status_code, 200)

        r = call_api_get( "/api/config" )
        j = json.loads(r.text)
        print(j)

if __name__ == '__main__':
    unittest.main()

