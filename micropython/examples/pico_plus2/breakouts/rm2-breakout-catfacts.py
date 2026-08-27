"""
Get a cat fact from t'internet!
You will need to add your wireless SSID and password to secrets.py (and save this file to your Pico)
"""

import network
import requests
from secrets import WIFI_SSID, WIFI_PASSWORD
from time import sleep

# Specify the pins that that wireless module is connected to
# The pins below are for a Pimoroni Pico Plus 2 with a RM2 breakout connected via SP/CE
wlan = network.WLAN(network.STA_IF, pin_on=32, pin_out=35, pin_in=35, pin_wake=35, pin_clock=34, pin_cs=33)

# connect to wifi
wlan.active(True)
wlan.connect(WIFI_SSID, WIFI_PASSWORD)
while wlan.isconnected() is False:
    print("Waiting for connection...")
    sleep(1)

request = requests.get("http://catfact.ninja/fact").json()
fact = request["fact"]
print("Cat fact!")
print(fact)
