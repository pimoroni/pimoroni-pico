import WIFI_CONFIG
from network_manager import NetworkManager
import time
import uasyncio
import ujson
from urllib import urequest
from picographics import PicoGraphics, DISPLAY_INKY_PACK
from pimoroni import Button


"""
Simple demo to get a random activity from BoredAPI.com
"""


button_a = Button(12)
button_b = Button(13)
button_c = Button(14)

graphics = PicoGraphics(DISPLAY_INKY_PACK)
graphics.set_font("bitmap8")

WIDTH, HEIGHT = graphics.get_bounds()
ENDPOINT = "https://www.boredapi.com/api/activity"


def status_handler(mode, status, ip):
    graphics.set_update_speed(2)
    graphics.set_pen(15)
    graphics.clear()
    graphics.set_pen(0)
    graphics.text("Network: {}".format(WIFI_CONFIG.SSID), 10, 10, scale=2)
    status_text = "Connecting..."
    if status is not None:
        if status:
            status_text = "Connection successful!"
        else:
            status_text = "Connection failed!"

    graphics.text(status_text, 10, 30, scale=2)
    graphics.text("IP: {}".format(ip), 10, 60, scale=2)
    graphics.update()


network_manager = NetworkManager(WIFI_CONFIG.COUNTRY, status_handler=status_handler)


def update():
    uasyncio.get_event_loop().run_until_complete(network_manager.client(WIFI_CONFIG.SSID, WIFI_CONFIG.PSK))

    url = ENDPOINT
    print("Requesting URL: {}".format(url))
    j = ujson.load(urequest.urlopen(url))

    print(j)

    graphics.set_update_speed(1)
    graphics.set_pen(15)
    graphics.clear()
    graphics.set_pen(0)
    graphics.text(j["activity"], 10, 10, wordwrap=WIDTH - 20, scale=2)
    graphics.text(j["type"], 10, 108, scale=2)

    graphics.update()


# Run continuously.
# Be friendly to the API you're using!
while True:
    update()

    while not button_a.is_pressed:
        time.sleep(0.1)
