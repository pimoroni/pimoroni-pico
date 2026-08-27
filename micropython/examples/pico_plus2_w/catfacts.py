"""
Get a cat fact from t'internet!
You will need to add your wireless SSID and password to secrets.py (and save this file to your Pico)
"""

import network
import requests
from secrets import WIFI_SSID, WIFI_PASSWORD
from time import sleep

# connect to wifi
wlan = network.WLAN(network.STA_IF)
wlan.active(True)
wlan.connect(WIFI_SSID, WIFI_PASSWORD)
while wlan.isconnected() is False:
    print("Waiting for connection...")
    sleep(1)

request = requests.get("http://catfact.ninja/fact").json()
fact = request["fact"]
print("Cat fact!")
print(fact)
