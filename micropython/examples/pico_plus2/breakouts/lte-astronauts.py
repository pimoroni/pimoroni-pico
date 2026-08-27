"""
List all the humans who are currently in space!

This example was adapted from one written by Les Pounder for Tom's Hardware: https://www.tomshardware.com/how-to/connect-raspberry-pi-pico-w-to-the-internet
"""

import lte
import time
import requests

MOBILE_APN = "iot.1nce.net"

con = lte.LTE(MOBILE_APN)
con.start_ppp()

try:
    t_start = time.time()
    astronauts = requests.get("http://api.open-notify.org/astros.json").json()
    number = astronauts["number"]

    print(f"There are currently {number} humans in space:")
    for i in range(number):
        print(astronauts["people"][i]["name"])

finally:
    t_end = time.time()

    print(f"Took: {t_end - t_start} seconds")

    print("Disconnecting...")
    con.stop_ppp()
    print("Done!")
