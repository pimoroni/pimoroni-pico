import WIFI_CONFIG
from network_manager import NetworkManager
import uasyncio
from urequests import get
import time
import ujson
import plasma
from plasma import plasma2040

"""
This Plasma Stick example sets your LED strip to the current #cheerlights colour.
Find out more about the Cheerlights API at https://cheerlights.com/
"""

URL = 'http://api.thingspeak.com/channels/1417/field/2/last.json'
UPDATE_INTERVAL = 120  # refresh interval in secs. Be nice to free APIs!

# Set how many LEDs you have
NUM_LEDS = 50


def status_handler(mode, status, ip):
    # reports wifi connection status
    print(mode, status, ip)
    print('Connecting to wifi...')
    # flash while connecting
    for i in range(NUM_LEDS):
        led_strip.set_rgb(i, 255, 255, 255)
        time.sleep(0.02)
    for i in range(NUM_LEDS):
        led_strip.set_rgb(i, 0, 0, 0)
    if status is not None:
        if status:
            print('Connection successful!')
        else:
            print('Connection failed!')
            # light up red if connection fails
            for i in range(NUM_LEDS):
                led_strip.set_rgb(i, 255, 0, 0)
            

def hex_to_rgb(hex):
    # converts a hex colour code into RGB
    h = hex.lstrip('#')
    r, g, b = (int(h[i:i + 2], 16) for i in (0, 2, 4))
    return r, g, b


# set up the WS2812 / NeoPixel™ LEDs
led_strip = plasma.WS2812(NUM_LEDS, 0, 0, plasma2040.DAT)

# start updating the LED strip
led_strip.start()

# set up wifi
network_manager = NetworkManager(WIFI_CONFIG.COUNTRY, status_handler=status_handler)
uasyncio.get_event_loop().run_until_complete(network_manager.client(WIFI_CONFIG.SSID, WIFI_CONFIG.PSK))

while True:
    # open the json file
    print(f"Requesting URL: {URL}")
    data = get(URL).json()
    print("Data obtained!")

    # extract hex colour from the data
    hex = data['field2']

    # and convert it to RGB
    r, g, b = hex_to_rgb(hex)
    
    # light up the LEDs
    for i in range(NUM_LEDS):
        led_strip.set_rgb(i, r, g, b)
    print(f"LEDs set to {hex}")

    # sleep
    print(f"""Sleeping for {UPDATE_INTERVAL} seconds.
        """)
    time.sleep(UPDATE_INTERVAL)