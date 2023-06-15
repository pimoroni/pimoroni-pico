# This Stellar Unicorn example updates a pixel every two(ish) minutes
# to display the most recent #cheerlights colour. Discover the most popular
# colours over time, or use it as an avant garde (but colourful) 16 hour clock!
# Find out more about the Cheerlights API at https://cheerlights.com/
#
# To run this example you'll need WIFI_CONFIG.py and network_manager.py from
# the pimoroni-pico micropython/examples/common folder

import WIFI_CONFIG
from network_manager import NetworkManager
import uasyncio
import urequests
import time
import random
from machine import Timer, Pin
from stellar import StellarUnicorn
from picographics import PicoGraphics, DISPLAY_STELLAR_UNICORN as DISPLAY, PEN_P8 as PEN

URL = 'http://api.thingspeak.com/channels/1417/field/1/last.txt'

UPDATE_INTERVAL = 60 * 60 / 16  # refresh interval in secs. Be nice to free APIs!
# Calculated as 60 minutes * 60 seconds divided by number of pixels per row
# so that a row of LEDs equates (approximately) to an hour

CHEERLIGHTS_COLOR_VALUES = [
    (0x00, 0x00, 0x00),  # Black/Unlit
    (0xFF, 0x00, 0x00),
    (0x00, 0x80, 0x00),
    (0x00, 0x00, 0xFF),
    (0x00, 0xFF, 0xFF),
    (0xFF, 0xFF, 0xFF),
    (0xFD, 0xF5, 0xE6),
    (0x80, 0x00, 0x80),
    (0xFF, 0x00, 0xFF),
    (0xFF, 0xFF, 0x00),
    (0xFF, 0xA5, 0x00),
    (0xFF, 0xC0, 0xCB),
]

CHEERLIGHTS_COLOR_NAMES = [
    "black",  # Black/Unlit, not part of cheerlights colours
    "red",
    "green",
    "blue",
    "cyan",
    "white",
    "oldlace",
    "purple",
    "magenta",
    "yellow",
    "orange",
    "pink"
]


def status_handler(mode, status, ip):
    # reports wifi connection status
    print(mode, status, ip)
    print('Connecting to wifi...')
    if status is not None:
        if status:
            print('Wifi connection successful!')
        else:
            print('Wifi connection failed!')


def get_data():
    global index
    # open the json file
    if UPDATE_INTERVAL >= 60:
        print(f'Requesting URL: {URL}')
        r = urequests.get(URL)
        name = r.content.decode("utf-8").strip()
        r.close()
        print('Data obtained!')

    else:
        print("Random test colour!")
        # For sped-up testing we don't want to hit the API at all
        name = random.choice(CHEERLIGHTS_COLOR_NAMES[1:])

    # flash the onboard LED after getting data
    pico_led.value(True)
    time.sleep(0.2)
    pico_led.value(False)

    # add the new hex colour to the end of the array
    if index == (width * height):
        index = 0
        graphics.clear()

    colour_array[index] = CHEERLIGHTS_COLOR_NAMES.index(name)
    index += 1
    print(f'Colour added to array: {name}')

    su.update(graphics)
    print("LEDs updated!")


su = StellarUnicorn()
width = StellarUnicorn.WIDTH
height = StellarUnicorn.HEIGHT

# set up a buffer to store the colours
colour_array = bytearray(width * height)

# We'll use palette mode, so just make the colour list the display buffer
graphics = PicoGraphics(DISPLAY, pen_type=PEN, buffer=colour_array)

# Set up the palette with cheerlights colour values
graphics.set_palette(CHEERLIGHTS_COLOR_VALUES)
graphics.set_pen(0)
graphics.clear()

# Keep track of the pixel we're lighting
index = 0

su.set_brightness(0.5)

# set up the Pico W's onboard LED
pico_led = Pin('LED', Pin.OUT)

# set up wifi
try:
    network_manager = NetworkManager(WIFI_CONFIG.COUNTRY, status_handler=status_handler)
    uasyncio.get_event_loop().run_until_complete(network_manager.client(WIFI_CONFIG.SSID, WIFI_CONFIG.PSK))
except Exception as e:
    print(f'Wifi connection failed! {e}')

# get the first lot of data
get_data()

# start timer (the timer will call the function to update our data every UPDATE_INTERVAL)
timer = Timer(-1)
timer.init(period=int(UPDATE_INTERVAL * 1000), mode=Timer.PERIODIC, callback=lambda t: get_data())

while True:
    # adjust brightness with LUX + and -
    # LEDs take a couple of secs to update, so adjust in big (10%) steps
    if su.is_pressed(StellarUnicorn.SWITCH_BRIGHTNESS_UP):
        su.adjust_brightness(+0.1)
        su.update(graphics)
        print(f"Brightness set to {su.get_brightness()}")

    if su.is_pressed(StellarUnicorn.SWITCH_BRIGHTNESS_DOWN):
        su.adjust_brightness(-0.1)
        su.update(graphics)
        print(f"Brightness set to {su.get_brightness()}")

    # pause for a moment (important or the USB serial device will fail)
    time.sleep(0.001)
