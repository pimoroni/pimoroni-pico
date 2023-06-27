import time
from stellar import StellarUnicorn
from picographics import PicoGraphics, DISPLAY_STELLAR_UNICORN as DISPLAY
import WIFI_CONFIG
from network_manager import NetworkManager
import uasyncio
import urequests
import jpegdec

"""
This example connects to Open Meteo to access the current weather conditions.
It then displays an appropriate weather icon on Stellar Unicorn.

Find out more about the Open Meteo API at https://open-meteo.com
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


# create galactic object and graphics surface for drawing
su = StellarUnicorn()
display = PicoGraphics(DISPLAY)

WIDTH = StellarUnicorn.WIDTH
HEIGHT = StellarUnicorn.HEIGHT

RED = display.create_pen(255, 0, 0)

jpeg = jpegdec.JPEG(display)

su.set_brightness(1.0)

# set up wifi
try:
    network_manager = NetworkManager(WIFI_CONFIG.COUNTRY, status_handler=status_handler)
    uasyncio.get_event_loop().run_until_complete(network_manager.client(WIFI_CONFIG.SSID, WIFI_CONFIG.PSK))
except Exception as e:
    print(f'Wifi connection failed! {e}')

while True:
    get_data()
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

    else:
        display.set_pen(RED)
        display.text("ERR", 0, 0, WIDTH, 1)

    su.update(display)
    time.sleep(UPDATE_INTERVAL * 60)
