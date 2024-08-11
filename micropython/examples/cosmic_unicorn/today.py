import time
import network
import ntptime
import machine

from cosmic import CosmicUnicorn
from picographics import PicoGraphics, DISPLAY_COSMIC_UNICORN as DISPLAY

cu = CosmicUnicorn()
graphics = PicoGraphics(DISPLAY)

# Default Brightness
cu.set_brightness(0.4)

# You will need to create or update the file secrets.py with your network credentials using Thonny
# in order for the example to update using the NTP.

# secrets.py should contain:
# WIFI_SSID = ""
# WIFI_PASSWORD = ""

try:
    from secrets import WIFI_SSID, WIFI_PASSWORD
except ImportError:
    print("Create secrets.py with your WiFi credentials")


WIDTH = CosmicUnicorn.WIDTH
HEIGHT = CosmicUnicorn.HEIGHT

rtc = machine.RTC()

DAYS = ["Mon", "Tue", "Wed", "Thur", "Fri", "Sat", "Sun"]

# Enable the Wireless
wlan = network.WLAN(network.STA_IF)
wlan.active(True)


def network_connect(SSID, PSK):

    # Number of attempts to make before timeout
    max_wait = 5

    # Sets the Wireless LED pulsing and attempts to connect to your local network.
    print("connecting...")
    wlan.config(pm=0xa11140)  # Turn WiFi power saving off for some slow APs
    wlan.connect(SSID, PSK)

    while max_wait > 0:
        if wlan.status() < 0 or wlan.status() >= 3:
            break
        max_wait -= 1
        print('waiting for connection...')
        time.sleep(1)

    # Handle connection error. Switches the Warn LED on.
    if wlan.status() != 3:
        print("Unable to connect. Attempting connection again")


# Function to sync the Pico RTC using NTP
def sync_time():

    try:
        network_connect(WIFI_SSID, WIFI_PASSWORD)
    except NameError:
        print("Create secrets.py with your WiFi credentials")

    if wlan.status() < 0 or wlan.status() >= 3:
        try:
            ntptime.settime()
        except OSError:
            print("Unable to sync with NTP server. Check network and try again.")


def init():

    sync_time()


def draw():

    # Pens
    RED = graphics.create_pen(120, 0, 0)
    WHITE = graphics.create_pen(255, 255, 255)

    current_t = rtc.datetime()

    # Set the pen to Red and clear the screen.
    graphics.set_pen(WHITE)
    graphics.clear()

    # Measures the length of the text to help us with centring later.
    day_length = graphics.measure_text(DAYS[current_t[3]], 1)
    date_length = graphics.measure_text(str(current_t[2]), 3)

    graphics.set_font("bitmap6")
    graphics.set_pen(RED)
    graphics.rectangle(0, 0, WIDTH, 7)
    graphics.set_pen(WHITE)
    graphics.text(DAYS[current_t[3]], (WIDTH // 2) - (day_length // 2) - 1, 0, 32, 1)

    graphics.set_pen(RED)
    graphics.set_font("bitmap8")
    graphics.text(str(current_t[2]), (WIDTH // 2) - (date_length // 2) + 1, 9, 32, 3)

    graphics.set_pen(graphics.create_pen(0, 0, 0))

    cu.update(graphics)


init()

while 1:

    # Adjust Brightness +/-
    if cu.is_pressed(CosmicUnicorn.SWITCH_BRIGHTNESS_UP):
        cu.adjust_brightness(+0.01)

    if cu.is_pressed(CosmicUnicorn.SWITCH_BRIGHTNESS_DOWN):
        cu.adjust_brightness(-0.01)

    # Connect to the network and sync with the NTP server
    if cu.is_pressed(CosmicUnicorn.SWITCH_C):
        sync_time()

    draw()
