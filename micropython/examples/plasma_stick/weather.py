import WIFI_CONFIG
from network_manager import NetworkManager
import uasyncio
import urequests
import time
import plasma
from plasma import plasma_stick
# Random functions! randrange is for picking integers from a range, and uniform is for floats.
from random import randrange, uniform
from machine import Timer, Pin

"""
Weather in a bottle!
This Plasma Stick example connects to Open Meteo to access the current weather conditions.
It then does some cool weather appropriate stuff with LEDs.
Find out more about the Open Meteo API at https://open-meteo.com
Based on original code by AxWax <3 https://github.com/axwax/Open-Meteo-Inky-Pack
"""

# Set how many LEDs you have
NUM_LEDS = 50

# Set your latitude/longitude here (find yours by right clicking in Google Maps!)
LAT = 53.38609085276884
LNG = -1.4239983439328177
TIMEZONE = "auto"  # determines time zone from lat/long

URL = "http://api.open-meteo.com/v1/forecast?latitude=" + str(LAT) + "&longitude=" + str(LNG) + "&current_weather=true&timezone=" + TIMEZONE
UPDATE_INTERVAL = 900  # refresh interval in secs. Be nice to free APIs!

# Weather codes from https://open-meteo.com/en/docs#:~:text=WMO%20Weather%20interpretation%20codes%20(WW)
WEATHERCODES = {
    0: 'clear sky',
    1: 'mostly clear',
    2: 'partly cloudy',
    3: 'cloudy',
    45: 'fog and depositing rime',
    48: 'fog',
    51: 'light drizzle',
    53: 'moderate drizzle',
    55: 'dense drizzle',
    56: 'light freezing drizzle',
    57: 'dense freezing drizzle',
    61: 'slight rain',
    63: 'moderate rain',
    65: 'heavy rain',
    66: 'light freezing rain',
    67: 'heavy freezing rain',
    71: 'slight snow',
    73: 'moderate snow',
    75: 'heavy snow',
    77: 'snow grains',
    80: 'slight rain showers',
    81: 'moderate rain showers',
    82: 'violent rain showers',
    85: 'slight snow showers',
    86: 'heavy snow showers',
    95: 'thunderstorm',
    96: 'thunderstorm with slight hail',
    99: 'thunderstorm with heavy hail'
}


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


def get_data():
    global weathercode
    print(f"Requesting URL: {URL}")
    r = urequests.get(URL)
    # open the json data
    j = r.json()
    print("Data obtained!")

    # parse relevant data from JSON
    current = j["current_weather"]
    temperature = current["temperature"]
    weathercode = current["weathercode"]
    datetime_arr = current["time"].split("T")

    print(f"""Temperature = {temperature}°C
Conditions = {WEATHERCODES[weathercode]}
Last Open-Meteo update: {datetime_arr[0]}, {datetime_arr[1]}
    """)

    r.close()

    # flash the onboard LED after getting data
    pico_led.value(True)
    time.sleep(0.2)
    pico_led.value(False)


# the rest of our functions are for animations!
def display_current():
    # paint our current LED colours to the strip
    for i in range(NUM_LEDS):
        led_strip.set_rgb(i, current_leds[i][0], current_leds[i][1], current_leds[i][2])


def move_to_target():
    # nudge our current colours closer to the target colours
    for i in range(NUM_LEDS):
        for c in range(3):  # 3 times, for R, G & B channels
            if current_leds[i][c] < target_leds[i][c]:
                current_leds[i][c] = min(current_leds[i][c] + ANIMATION_SPEED, target_leds[i][c])  # increase current, up to a maximum of target
            elif current_leds[i][c] > target_leds[i][c]:
                current_leds[i][c] = max(current_leds[i][c] - ANIMATION_SPEED, target_leds[i][c])  # reduce current, down to a minimum of target


def clear():
    if weathercode == 0:  # clear
        # nice sunny yellow
        for i in range(NUM_LEDS):
            target_leds[i] = [randrange(220, 260), randrange(220, 260), randrange(60, 100)]
    if weathercode == 1:  # mostly clear
        # sky blues
        for i in range(NUM_LEDS):
            target_leds[i] = [randrange(0, 40), randrange(150, 190), randrange(180, 220)]


def clouds():
    # base colours:
    if weathercode == 2:
        cloud_colour = [165, 168, 138]  # partly cloudy
    if weathercode == 3:
        cloud_colour = [93, 94, 83]  # cloudy
    if weathercode in (45, 48):
        cloud_colour = [186, 185, 182]  # foggy

    # add highlights and lowlights
    for i in range(NUM_LEDS):
        if uniform(0, 1) < 0.001:  # highlight
            target_leds[i] = [x + 20 for x in cloud_colour]
        elif uniform(0, 1) < 0.001:  # lowlight
            target_leds[i] = [x - 20 for x in cloud_colour]
        elif uniform(0, 1) < 0.005:  # normal
            target_leds[i] = cloud_colour


def storm():
    # heavy rain, with lightning!
    raindrop_chance = 0.01

    for i in range(NUM_LEDS):
        if raindrop_chance > uniform(0, 1):
            # paint a raindrop (use current rather than target, for an abrupt change to the drop colour)
            current_leds[i] = [randrange(0, 50), randrange(20, 100), randrange(50, 255)]
        else:
            # paint backdrop
            target_leds[i] = [0, 15, 60]

    lightning_chance = 0.001
    if lightning_chance > uniform(0, 1):
        for i in range(NUM_LEDS):
            current_leds[i] = [255, 255, 255]


def rain():
    # splodgy blues
    # first, work out how many raindrops:
    if weathercode in (51, 56, 61, 66, 80):  # light rain
        raindrop_chance = 0.001
    elif weathercode in (53, 63, 81):  # moderate rain
        raindrop_chance = 0.005
    else:
        # heavy rain
        raindrop_chance = 0.01

    for i in range(NUM_LEDS):
        if raindrop_chance > uniform(0, 1):
            # paint a raindrop (use current rather than target, for an abrupt change to the drop colour)
            current_leds[i] = [randrange(0, 50), randrange(20, 100), randrange(50, 255)]
        else:
            # paint backdrop
            target_leds[i] = [0, 15, 60]


def snow():
    # splodgy whites
    # first, work out how many snowflakes:
    if weathercode in (71, 85):  # light snow
        snowflake_chance = 0.001
    elif weathercode in (73, 77):  # moderate snow
        snowflake_chance = 0.005
    else:
        # heavy snow
        snowflake_chance = 0.01

    for i in range(NUM_LEDS):
        if snowflake_chance > uniform(0, 1):
            # paint a snowflake (use current rather than target, for an abrupt change to the drop colour)
            current_leds[i] = [227, 227, 227]
        else:
            # paint backdrop
            target_leds[i] = [54, 54, 54]


# some variables we'll use for animations
ANIMATION_SPEED = 1  # higher number gets from current to target colour faster

# Create an list of [r, g, b] values that will hold current LED colours, for display
current_leds = [[0] * 3 for i in range(NUM_LEDS)]
# Create an list of [r, g, b] values that will hold target LED colours, to move towards
target_leds = [[0] * 3 for i in range(NUM_LEDS)]

# set up the Pico W's onboard LED
pico_led = Pin('LED', Pin.OUT)

# set up the WS2812 / NeoPixel™ LEDs
led_strip = plasma.WS2812(NUM_LEDS, 0, 0, plasma_stick.DAT, color_order=plasma.COLOR_ORDER_RGB)

# start updating the LED strip
led_strip.start()

# set up wifi
network_manager = NetworkManager(WIFI_CONFIG.COUNTRY, status_handler=status_handler)
uasyncio.get_event_loop().run_until_complete(network_manager.client(WIFI_CONFIG.SSID, WIFI_CONFIG.PSK))

# get the first lot of data
get_data()

# start timer (the timer will update our data every UPDATE_INTERVAL)
timer = Timer(-1)
timer.init(period=UPDATE_INTERVAL * 1000, mode=Timer.PERIODIC, callback=lambda t: get_data())

while True:
    # do some fancy stuff with the LEDs based on the weather code
    if 0 <= weathercode <= 1:
        clear()
    elif 2 <= weathercode <= 48:
        clouds()
    elif 51 <= weathercode <= 67 or 80 <= weathercode <= 82:
        rain()
    elif 71 <= weathercode <= 77 or 85 <= weathercode <= 86:
        snow()
    elif 95 <= weathercode <= 99:
        storm()
    else:
        print("Unknown weather code :(")

    move_to_target()   # nudge our current colours closer to the target colours
    display_current()  # display current colours to strip
