import time
from cosmic import CosmicUnicorn
from picographics import PicoGraphics, DISPLAY_COSMIC_UNICORN as DISPLAY
import WIFI_CONFIG
from network_manager import NetworkManager
import uasyncio as asyncio
import urequests
import jpegdec

# Set your latitude/longitude here (find yours by right clicking in Google Maps!)
LAT = 53.38609085276884
LNG = -1.4239983439328177
TIMEZONE = "auto"  # determines time zone from lat/long

URL = "http://api.open-meteo.com/v1/forecast?latitude=" + str(LAT) + "&longitude=" + str(LNG) + "&current_weather=true&timezone=" + TIMEZONE
WEATHER_TEXT = ''
user_icon = None


def get_data():
    global WEATHER_TEXT, temperature, weathercode
    print(f"Requesting URL: {URL}")
    r = urequests.get(URL)
    # open the json data
    j = r.json()
    print("Data obtained!")
    print(j)

    # parse relevant data from JSON
    current = j["current_weather"]
    temperature = current["temperature"]
    windspeed = current["windspeed"]
    winddirection = calculate_bearing(current["winddirection"])
    weathercode = current["weathercode"]
    date, now = current["time"].split("T")
    WEATHER_TEXT = f"Temp: {temperature}°C Wind Speed: {windspeed}kmph Wind Direction: {winddirection} As of: {date}, {now}"
    print(WEATHER_TEXT)
    r.close()


def calculate_bearing(d):
    # calculates a compass direction from the wind direction in degrees
    dirs = ['N', 'NNE', 'NE', 'ENE', 'E', 'ESE', 'SE', 'SSE', 'S', 'SSW', 'SW', 'WSW', 'W', 'WNW', 'NW', 'NNW']
    ix = round(d / (360. / len(dirs)))
    return dirs[ix % len(dirs)]


def status_handler(mode, status, ip):
    global MESSAGE
    print("Network: {}".format(WIFI_CONFIG.SSID))


# create galactic object and graphics surface for drawing
cu = CosmicUnicorn()
display = PicoGraphics(DISPLAY)

WIDTH = CosmicUnicorn.WIDTH
HEIGHT = CosmicUnicorn.HEIGHT

jpeg = jpegdec.JPEG(display)
TEXT_COLOUR = display.create_pen(200, 0, 200)
BLACK = display.create_pen(0, 0, 0)
WHITE = display.create_pen(255, 255, 255)


def run():
    # Setup wifi
    network_manager = NetworkManager(WIFI_CONFIG.COUNTRY, status_handler=status_handler)

    # Connect to Wifi network
    asyncio.get_event_loop().run_until_complete(network_manager.client(WIFI_CONFIG.SSID, WIFI_CONFIG.PSK))
    while (not network_manager.isconnected()):
        time.sleep(0.1)


cu.set_brightness(1)
run()  # Sets up Wifi connection


def outline_text(text, x, y):
    display.set_pen(BLACK)
    display.text(text, x - 1, y - 1, -1, 1)
    display.text(text, x, y - 1, -1, 1)
    display.text(text, x + 1, y - 1, -1, 1)
    display.text(text, x - 1, y, -1, 1)
    display.text(text, x + 1, y, -1, 1)
    display.text(text, x - 1, y + 1, -1, 1)
    display.text(text, x, y + 1, -1, 1)
    display.text(text, x + 1, y + 1, -1, 1)

    display.set_pen(WHITE)
    display.text(text, x, y, -1, 1)


def draw_page(cycle):
    global user_icon
    text_cycle = cycle % 1000
    cycle = cycle % 4
    # Clear the display
    display.set_pen(15)
    display.clear()

    # Draw the page header
    display.set_font("bitmap6")

    if temperature is not None:
        # Choose an appropriate icon based on the weather code
        # Weather codes from https://open-meteo.com/en/docs
        if user_icon is not None:
            icons = ["icons/snow{0}.jpg".format(cycle + 1), "icons/rain{0}.jpg".format(cycle + 1), "icons/cloud{0}.jpg".format(cycle + 1), "icons/sun{0}.jpg".format(cycle + 1), "icons/storm{0}.jpg".format(cycle + 1)]
            jpeg.open_file(icons[user_icon])
        else:
            if weathercode in [71, 73, 75, 77, 85, 86]:  # codes for snow
                jpeg.open_file("icons/snow{0}.jpg".format(cycle + 1))
            elif weathercode in [51, 53, 55, 56, 57, 61, 63, 65, 66, 67, 80, 81, 82]:  # codes for rain
                jpeg.open_file("icons/rain{0}.jpg".format(cycle + 1))
            elif weathercode in [1, 2, 3, 45, 48]:  # codes for cloud
                jpeg.open_file("icons/cloud{0}.jpg".format(cycle + 1))
            elif weathercode in [0]:  # codes for sun
                jpeg.open_file("icons/sun{0}.jpg".format(cycle + 1))
            elif weathercode in [95, 96, 99]:  # codes for storm
                jpeg.open_file("icons/storm{0}.jpg".format(cycle + 1))
        jpeg.decode(0, 0, jpegdec.JPEG_SCALE_FULL)
        display.set_pen(TEXT_COLOUR)
        outline_text(WEATHER_TEXT, 32 - text_cycle, 0)

    else:
        display.set_pen(0)
        display.set_pen(15)
        display.text("Unable to display weather! Check your network settings in WIFI_CONFIG.py", 5, 65, WIDTH, 1)

    cu.update(display)


while 1:

    get_data()
    for count in range(500):

        if cu.is_pressed(CosmicUnicorn.SWITCH_A):
            user_icon = 0
        if cu.is_pressed(CosmicUnicorn.SWITCH_B):
            user_icon = 1
        if cu.is_pressed(CosmicUnicorn.SWITCH_C):
            user_icon = 2
        if cu.is_pressed(CosmicUnicorn.SWITCH_D):
            user_icon = 3
        if cu.is_pressed(CosmicUnicorn.SWITCH_BRIGHTNESS_UP):
            user_icon = 4
        if cu.is_pressed(CosmicUnicorn.SWITCH_BRIGHTNESS_DOWN):
            user_icon = None
        draw_page(count)
        time.sleep(0.2)
