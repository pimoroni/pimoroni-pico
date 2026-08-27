"""
List all the humans who are currently in space!
You will need to add your wireless SSID and password to secrets.py (and save this file to your Pico)
This example was adapted from one written by Les Pounder for Tom's Hardware: https://www.tomshardware.com/how-to/connect-raspberry-pi-pico-w-to-the-internet
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

astronauts = requests.get("http://api.open-notify.org/astros.json").json()
number = astronauts["number"]

print(f"There are currently {number} humans in space:")
for i in range(number):
    print(astronauts["people"][i]["name"])
