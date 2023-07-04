import time
from stellar import StellarUnicorn
from picographics import PicoGraphics, DISPLAY_STELLAR_UNICORN as DISPLAY
import WIFI_CONFIG
from network_manager import NetworkManager
import uasyncio
import urequests
import jpegdec
from random import choice

"""
This example connects to Open Meteo to access the current weather conditions.
It then displays an appropriate weather icon on Stellar Unicorn.

Find out more about the Open Meteo API at https://open-meteo.com

Buttons:
A - show / hide temperature
B - swap between Celsius and Fahrenheit
C - randomly select a weather icon
D - add rainbows
LUX + and - adjust brightness
"""

# Set your latitude/longitude here (find yours by right clicking in Google Maps!)
LAT = 53.38609085276884
LNG = -1.4239983439328177
TIMEZONE = "auto"  # determines time zone from lat/long

URL = "http://api.open-meteo.com/v1/forecast?latitude=" + str(LAT) + "&longitude=" + str(LNG) + "&current_weather=true&timezone=" + TIMEZONE

# how often to poll the API, in minutes
UPDATE_INTERVAL = 5


def get_data():
    global temperature, weathercode
    print(f"Requesting URL: {URL}")
    r = urequests.get(URL)
    # open the json data
    j = r.json()
    print("Data obtained!")

    # parse relevant data from JSON
    current = j["current_weather"]
    temperature = current["temperature"]
    windspeed = current["windspeed"]
    winddirection = calculate_bearing(current["winddirection"])
    weathercode = current["weathercode"]
    date, now = current["time"].split("T")
    print(f"Temp: {temperature}°C Wind Speed: {windspeed}kmph Wind Direction: {winddirection} As of: {date}, {now}")
    r.close()


def calculate_bearing(d):
    # calculates a compass direction from the wind direction in degrees
    dirs = ['N', 'NNE', 'NE', 'ENE', 'E', 'ESE', 'SE', 'SSE', 'S', 'SSW', 'SW', 'WSW', 'W', 'WNW', 'NW', 'NNW']
    ix = round(d / (360. / len(dirs)))
    return dirs[ix % len(dirs)]


def status_handler(mode, status, ip):
    # reports wifi connection status
    print(mode, status, ip)
    print('Connecting to wifi...')
    if status is not None:
        if status:
            print('Wifi connection successful!')
        else:
            print('Wifi connection failed!')


# create objects and picographics surface for drawing
su = StellarUnicorn()
display = PicoGraphics(DISPLAY)

jpeg = jpegdec.JPEG(display)

# some useful constants
WIDTH = StellarUnicorn.WIDTH
HEIGHT = StellarUnicorn.HEIGHT

BLACK = display.create_pen(0, 0, 0)
RED = display.create_pen(255, 0, 0)
ORANGE = display.create_pen(246, 138, 30)
YELLOW = display.create_pen(255, 216, 0)
GREEN = display.create_pen(0, 121, 64)
BLUE = display.create_pen(0, 0, 255)
INDIGO = display.create_pen(36, 64, 142)
VIOLET = display.create_pen(115, 41, 130)
WHITE = display.create_pen(255, 255, 255)

show_temperature = True
show_fahrenheit = False
show_rainbow = False

# timer variable to keep track of how often to poll the api
timer = UPDATE_INTERVAL * 60.0

su.set_brightness(0.8)

# set up wifi
try:
    network_manager = NetworkManager(WIFI_CONFIG.COUNTRY, status_handler=status_handler)
    uasyncio.get_event_loop().run_until_complete(network_manager.client(WIFI_CONFIG.SSID, WIFI_CONFIG.PSK))
except Exception as e:
    print(f'Wifi connection failed! {e}')

while True:
    # adjust brightness with LUX + and -
    if su.is_pressed(StellarUnicorn.SWITCH_BRIGHTNESS_UP):
        su.adjust_brightness(+0.05)
        print(f"Brightness set to {su.get_brightness()}")
    if su.is_pressed(StellarUnicorn.SWITCH_BRIGHTNESS_DOWN):
        su.adjust_brightness(-0.05)
        print(f"Brightness set to {su.get_brightness()}")

    if su.is_pressed(StellarUnicorn.SWITCH_A):
        show_temperature = not show_temperature
        print(f"Show temperature = {show_temperature}")
        # debounce
        time.sleep(0.1)

    if su.is_pressed(StellarUnicorn.SWITCH_B):
        show_fahrenheit = not show_fahrenheit
        print(f"Show fahrenheit = {show_fahrenheit}")
        # debounce
        time.sleep(0.1)

    # I hate this weather, give me another
    if su.is_pressed(StellarUnicorn.SWITCH_C):
        weathercode = choice([71, 51, 1, 0, 95])
        print("Weather icon randomised!")
        # debounce
        time.sleep(0.1)

    # brighten up boring weather with a rainbow
    if su.is_pressed(StellarUnicorn.SWITCH_D):
        show_rainbow = not show_rainbow
        print(f"Show rainbow = {show_rainbow}")
        # debounce
        time.sleep(0.1)

    # we only need to ping the api for data every UPDATE_INTERVAL
    if timer >= UPDATE_INTERVAL * 60:
        get_data()
        timer = 0.0

    if weathercode is not None:
        # Choose an appropriate icon based on the weather code
        # Weather codes from https://open-meteo.com/en/docs
        if weathercode in [71, 73, 75, 77, 85, 86]:  # codes for snow
            jpeg.open_file("icons/snow.jpg")
        elif weathercode in [51, 53, 55, 56, 57, 61, 63, 65, 66, 67, 80, 81, 82]:  # codes for rain
            jpeg.open_file("icons/rain.jpg")
        elif weathercode in [1, 2, 3, 45, 48]:  # codes for cloud
            jpeg.open_file("icons/cloud.jpg")
        elif weathercode in [0]:  # codes for sun
            jpeg.open_file("icons/sun.jpg")
        elif weathercode in [95, 96, 99]:  # codes for storm
            jpeg.open_file("icons/storm.jpg")
        jpeg.decode(0, 0, jpegdec.JPEG_SCALE_FULL)

        if show_rainbow is True:
            display.set_pen(VIOLET)
            display.circle(WIDTH - 1, HEIGHT - 1, 6)
            display.set_pen(BLUE)
            display.circle(WIDTH - 1, HEIGHT - 1, 5)
            display.set_pen(GREEN)
            display.circle(WIDTH - 1, HEIGHT - 1, 4)
            display.set_pen(YELLOW)
            display.circle(WIDTH - 1, HEIGHT - 1, 3)
            display.set_pen(ORANGE)
            display.circle(WIDTH - 1, HEIGHT - 1, 2)
            display.set_pen(RED)
            display.circle(WIDTH - 1, HEIGHT - 1, 1)

        # draw the temperature text
        if show_temperature is True:
            display.set_pen(RED)
            if show_fahrenheit is True:
                fahrenheit = (temperature * 9 / 5) + 32
                # measure the text so we can right align it
                text_width = display.measure_text(f"{fahrenheit:.0f}°", scale=1)
                display.text(f"{fahrenheit:.0f}°", WIDTH - text_width, 4, WIDTH, scale=1)
            else:
                # measure the text so we can right align it
                text_width = display.measure_text(f"{temperature:.0f}°", scale=1)
                display.text(f"{temperature:.0f}°", WIDTH - text_width, 4, WIDTH, scale=1)

    else:
        display.set_pen(RED)
        display.text("ERR", 0, 0, WIDTH, 1)

    su.update(display)
    timer += 0.1
    time.sleep(0.1)
