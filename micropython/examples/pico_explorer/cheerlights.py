import WIFI_CONFIG
from network_manager import NetworkManager
import uasyncio
from urllib import urequest
from picographics import PicoGraphics, DISPLAY_PICO_EXPLORER
import time
import ujson
from pimoroni import RGBLED

"""
This Pico W example sets your Pico Explorer's screen to the current #cheerlights colour.
Find out more about Cheerlights at https://cheerlights.com/
If you wire up a four legged RGB LED to GP0, GP1 and GP2 (and the long leg to ground)
it will change the colour of that too!
"""

URL = "http://api.thingspeak.com/channels/1417/field/2/last.json"
UPDATE_INTERVAL = 120  # refresh interval in secs. Be nice to free APIs!


def status_handler(mode, status, ip):
    # connect to the internets
    display.set_pen(BLACK)
    display.clear()
    display.set_pen(WHITE)
    display.text("Network: {}".format(WIFI_CONFIG.SSID), 10, 10, scale=2)
    status_text = "Connecting..."
    if status is not None:
        if status:
            status_text = "Connection successful!"
        else:
            status_text = "Connection failed!"

    display.text(status_text, 10, 30, scale=2)
    display.text("IP: {}".format(ip), 10, 60, scale=2)
    display.update()


def hex_to_rgb(hex):
    # converts a hex colour code into RGB
    h = hex.lstrip('#')
    r, g, b = (int(h[i:i + 2], 16) for i in (0, 2, 4))
    return r, g, b


# set up wifi
network_manager = NetworkManager(WIFI_CONFIG.COUNTRY, status_handler=status_handler)

# set up the display
display = PicoGraphics(display=DISPLAY_PICO_EXPLORER)
display.set_backlight(1.0)

# some constants we'll use for drawing
WHITE = display.create_pen(255, 255, 255)
BLACK = display.create_pen(0, 0, 0)

WIDTH, HEIGHT = display.get_bounds()
display.set_font("bitmap8")

# set up an old school 4 pin RGB LED connected to GP0, GP1 and GP2
led = RGBLED(0, 1, 2, invert=False)

while True:
    # connect to wifi
    uasyncio.get_event_loop().run_until_complete(network_manager.client(WIFI_CONFIG.SSID, WIFI_CONFIG.PSK))

    # open the json file
    print(f"Requesting URL: {URL}")
    socket = urequest.urlopen(URL)
    data = ujson.load(socket)
    socket.close()
    print("Data obtained!")

    # extract hex colour from the data
    hex = data['field2']

    # and convert it to RGB
    r, g, b = hex_to_rgb(hex)

    # draw stuff on the screen
    colour = display.create_pen(r, g, b)
    display.set_pen(colour)
    display.clear()
    display.set_pen(BLACK)
    display.text(hex, 10, 10, WIDTH - 10, scale=6)
    display.text("Tweet a colour at #cheerlights to change!", 10, 130, WIDTH - 10, scale=3)
    display.update()
    print(f"Screen set to {hex}")

    # light up RGB LED
    led.set_rgb(r, g, b)
    print(f"RGB LED set to {hex}")

    # sleep
    print(f"""Sleeping for {UPDATE_INTERVAL} seconds.
        """)
    time.sleep(UPDATE_INTERVAL)
