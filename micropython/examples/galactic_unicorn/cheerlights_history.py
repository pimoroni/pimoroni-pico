# This Galactic Unicorn example updates a pixel every five(ish) minutes
# to display the most recent #cheerlights colour. Discover the most popular
# colours over time, or use it as an avant garde (but colourful) 53 hour clock!
# Find out more about the Cheerlights API at https://cheerlights.com/
#
# To run this example you'll need WIFI_CONFIG.py and network_manager.py from
# the pimoroni-pico micropython/examples/common folder

import WIFI_CONFIG
from network_manager import NetworkManager
import uasyncio
import urequests
import time
from machine import Timer, Pin
from galactic import GalacticUnicorn
from picographics import PicoGraphics, DISPLAY_GALACTIC_UNICORN as DISPLAY

URL = 'http://api.thingspeak.com/channels/1417/field/2/last.json'

UPDATE_INTERVAL = 327  # refresh interval in secs. Be nice to free APIs!
# this esoteric number is used so that a column of LEDs equates (approximately) to an hour


def status_handler(mode, status, ip):
    # reports wifi connection status
    print(mode, status, ip)
    print('Connecting to wifi...')
    if status is not None:
        if status:
            print('Wifi connection successful!')
        else:
            print('Wifi connection failed!')


def hex_to_rgb(hex):
    # converts a hex colour code into RGB
    h = hex.lstrip('#')
    r, g, b = (int(h[i:i + 2], 16) for i in (0, 2, 4))
    return r, g, b


def get_data():
    # open the json file
    print(f'Requesting URL: {URL}')
    r = urequests.get(URL)
    # open the json data
    j = r.json()
    print('Data obtained!')
    r.close()

    # flash the onboard LED after getting data
    pico_led.value(True)
    time.sleep(0.2)
    pico_led.value(False)

    # extract hex colour from the json data
    hex = j['field2']

    # add the new hex colour to the end of the array
    colour_array.append(hex)
    print(f'Colour added to array: {hex}')
    # remove the oldest colour in the array
    colour_array.pop(0)
    update_leds()


def update_leds():
    # light up the LEDs
    # this step takes a second, it's doing a lot of hex_to_rgb calculations!
    print("Updating LEDs...")
    i = 0
    for x in range(width):
        for y in range(height):
            r = hex_to_rgb(colour_array[i])[0]
            g = hex_to_rgb(colour_array[i])[1]
            b = hex_to_rgb(colour_array[i])[2]
            current_colour = graphics.create_pen(r, g, b)
            graphics.set_pen(current_colour)
            graphics.pixel(x, y)
            i = i + 1
    gu.update(graphics)
    print("LEDs updated!")


gu = GalacticUnicorn()
graphics = PicoGraphics(DISPLAY)

width = GalacticUnicorn.WIDTH
height = GalacticUnicorn.HEIGHT

gu.set_brightness(0.5)

# set up the Pico W's onboard LED
pico_led = Pin('LED', Pin.OUT)

current_colour = graphics.create_pen(0, 0, 0)

# set up an list to store the colours
colour_array = ["#000000"] * 583

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
timer.init(period=UPDATE_INTERVAL * 1000, mode=Timer.PERIODIC, callback=lambda t: get_data())

while True:
    # adjust brightness with LUX + and -
    # LEDs take a couple of secs to update, so adjust in big (10%) steps
    if gu.is_pressed(GalacticUnicorn.SWITCH_BRIGHTNESS_UP):
        gu.adjust_brightness(+0.1)
        update_leds()
        print(f"Brightness set to {gu.get_brightness()}")

    if gu.is_pressed(GalacticUnicorn.SWITCH_BRIGHTNESS_DOWN):
        gu.adjust_brightness(-0.1)
        update_leds()
        print(f"Brightness set to {gu.get_brightness()}")

    # pause for a moment (important or the USB serial device will fail)
    time.sleep(0.001)
