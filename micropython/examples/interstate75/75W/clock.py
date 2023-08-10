# Clock example with NTP synchronization
#
# Create a secrets.py with your Wifi details to be able to get the time
# when the Interstate75W isn't connected to Thonny.
#
# secrets.py should contain:
# WIFI_SSID = "Your WiFi SSID"
# WIFI_PASSWORD = "Your WiFi password"

import time
import math
import machine
import network
import ntptime
from interstate75 import Interstate75, DISPLAY_INTERSTATE75_32X32

i75 = Interstate75(display=DISPLAY_INTERSTATE75_32X32)
graphics = i75.display

width = i75.width
height = i75.height

try:
    from secrets import WIFI_SSID, WIFI_PASSWORD
    wifi_available = True
except ImportError:
    print("Create secrets.py with your WiFi credentials to get time from NTP")
    wifi_available = False


# constants for controlling the background colour throughout the day
MIDDAY_HUE = 1.1
MIDNIGHT_HUE = 0.8
HUE_OFFSET = -0.1

MIDDAY_SATURATION = 1.0
MIDNIGHT_SATURATION = 1.0

MIDDAY_VALUE = 0.8
MIDNIGHT_VALUE = 0.3

# create the rtc object
rtc = machine.RTC()


# set up some pens to use later
WHITE = graphics.create_pen(255, 255, 255)
BLACK = graphics.create_pen(0, 0, 0)


@micropython.native  # noqa: F821
def from_hsv(h, s, v):
    i = math.floor(h * 6.0)
    f = h * 6.0 - i
    v *= 255.0
    p = v * (1.0 - s)
    q = v * (1.0 - f * s)
    t = v * (1.0 - (1.0 - f) * s)

    i = int(i) % 6
    if i == 0:
        return int(v), int(t), int(p)
    if i == 1:
        return int(q), int(v), int(p)
    if i == 2:
        return int(p), int(v), int(t)
    if i == 3:
        return int(p), int(q), int(v)
    if i == 4:
        return int(t), int(p), int(v)
    if i == 5:
        return int(v), int(p), int(q)


# function for drawing a gradient background
def gradient_background(start_hue, start_sat, start_val, end_hue, end_sat, end_val):
    half_width = width // 2
    for x in range(0, half_width):
        hue = ((end_hue - start_hue) * (x / half_width)) + start_hue
        sat = ((end_sat - start_sat) * (x / half_width)) + start_sat
        val = ((end_val - start_val) * (x / half_width)) + start_val
        colour = from_hsv(hue, sat, val)
        graphics.set_pen(graphics.create_pen(int(colour[0]), int(colour[1]), int(colour[2])))
        for y in range(0, height):
            graphics.pixel(x, y)
            graphics.pixel(width - x - 1, y)

    colour = from_hsv(end_hue, end_sat, end_val)
    graphics.set_pen(graphics.create_pen(int(colour[0]), int(colour[1]), int(colour[2])))
    for y in range(0, height):
        graphics.pixel(half_width, y)


# function for drawing outlined text
def outline_text(text, x, y):
    graphics.set_pen(BLACK)
    graphics.text(text, x - 1, y - 1, -1, 1)
    graphics.text(text, x, y - 1, -1, 1)
    graphics.text(text, x + 1, y - 1, -1, 1)
    graphics.text(text, x - 1, y, -1, 1)
    graphics.text(text, x + 1, y, -1, 1)
    graphics.text(text, x - 1, y + 1, -1, 1)
    graphics.text(text, x, y + 1, -1, 1)
    graphics.text(text, x + 1, y + 1, -1, 1)

    graphics.set_pen(WHITE)
    graphics.text(text, x, y, -1, 1)


# Connect to wifi and synchronize the RTC time from NTP
def sync_time():
    if not wifi_available:
        return

    # Start connection
    wlan = network.WLAN(network.STA_IF)
    wlan.active(True)
    wlan.config(pm=0xa11140)  # Turn WiFi power saving off for some slow APs
    wlan.connect(WIFI_SSID, WIFI_PASSWORD)

    # Wait for connect success or failure
    max_wait = 100
    while max_wait > 0:
        if wlan.status() < 0 or wlan.status() >= 3:
            break
        max_wait -= 1
        print('waiting for connection...')
        time.sleep(0.2)

        redraw_display_if_reqd()

    if wlan.status() == 3:
        print("Connected")

        try:
            ntptime.settime()
            print("Time set")
        except OSError:
            pass

    wlan.disconnect()
    wlan.active(False)


# NTP synchronizes the time to UTC, this allows you to adjust the displayed time

utc_offset = 0

year, month, day, wd, hour, minute, second, _ = rtc.datetime()

last_second = second


# Check whether the RTC time has changed and if so redraw the display
def redraw_display_if_reqd():
    global year, month, day, wd, hour, minute, second, last_second

    year, month, day, wd, hour, minute, second, _ = rtc.datetime()
    if second != last_second:
        hour = (hour + utc_offset) % 24
        time_through_day = (((hour * 60) + minute) * 60) + second
        percent_through_day = time_through_day / 86400
        percent_to_midday = 1.0 - ((math.cos(percent_through_day * math.pi * 2) + 1) / 2)
        print(percent_to_midday)

        hue = ((MIDDAY_HUE - MIDNIGHT_HUE) * percent_to_midday) + MIDNIGHT_HUE
        sat = ((MIDDAY_SATURATION - MIDNIGHT_SATURATION) * percent_to_midday) + MIDNIGHT_SATURATION
        val = ((MIDDAY_VALUE - MIDNIGHT_VALUE) * percent_to_midday) + MIDNIGHT_VALUE

        gradient_background(hue, sat, val,
                            hue + HUE_OFFSET, sat, val)

        clock = "{:02}:{:02}:{:02}".format(hour, minute, second)

        # set the font
        graphics.set_font("bitmap8")

        # calculate text position so that it is centred
        w = graphics.measure_text(clock, 1)
        x = int(width / 2 - w / 2 + 1)
        y = 11

        outline_text(clock, x, y)

        last_second = second


sync_time()

while True:

    redraw_display_if_reqd()

    # Update the display
    i75.update()
