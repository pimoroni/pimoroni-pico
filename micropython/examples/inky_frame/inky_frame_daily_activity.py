import gc
import WIFI_CONFIG
from network_manager import NetworkManager
import uasyncio
import ujson
from urllib import urequest
# from picographics import PicoGraphics, DISPLAY_INKY_FRAME as DISPLAY      # 5.7"
# from picographics import PicoGraphics, DISPLAY_INKY_FRAME_4 as DISPLAY  # 4.0"
from picographics import PicoGraphics, DISPLAY_INKY_FRAME_7 as DISPLAY  # 7.3"
from machine import Pin
from pimoroni_i2c import PimoroniI2C
from pcf85063a import PCF85063A
import time


I2C_SDA_PIN = 4
I2C_SCL_PIN = 5
HOLD_VSYS_EN_PIN = 2

# set up and enable vsys hold so we don't go to sleep
hold_vsys_en_pin = Pin(HOLD_VSYS_EN_PIN, Pin.OUT)
hold_vsys_en_pin.value(True)

# intialise the pcf85063a real time clock chip
i2c = PimoroniI2C(I2C_SDA_PIN, I2C_SCL_PIN, 100000)
rtc = PCF85063A(i2c)

# Length of time between updates in Seconds.
# Frequent updates will reduce battery life!
UPDATE_INTERVAL = 60 * 1

# API URL
URL = "https://www.boredapi.com/api/activity"


def status_handler(mode, status, ip):
    print(mode, status, ip)


network_manager = NetworkManager(WIFI_CONFIG.COUNTRY, status_handler=status_handler)

gc.collect()
graphics = PicoGraphics(DISPLAY)
WIDTH, HEIGHT = graphics.get_bounds()
gc.collect()


def display_quote(text, ox, oy, scale, wordwrap):
    # Processing text is memory intensive
    # so we'll do it one char at a time as we draw to the screen
    line_height = 8 * scale
    html = False
    html_tag = ""
    word = ""
    space_width = graphics.measure_text(" ", scale=scale)
    x = ox
    y = oy
    for char in text:
        if char in "[]":
            continue
        if char == "<":
            html = True
            html_tag = ""
            continue
        if char == ">":
            html = False
            continue
        if html:
            if char in "/ ":
                continue
            html_tag += char
            continue
        if char in (" ", "\n") or html_tag == "br":
            w = graphics.measure_text(word, scale=scale)
            if x + w > wordwrap or char == "\n" or html_tag == "br":
                x = ox
                y += line_height

            graphics.text(word, x, y, scale=scale)
            word = ""
            html_tag = ""
            x += w + space_width
            continue

        word += char

    # Last word
    w = graphics.measure_text(word, scale=scale)
    if x + w > wordwrap:
        x = ox
        y += line_height

    graphics.text(word, x, y, scale=scale)


rtc.enable_timer_interrupt(True)

while True:
    # Connect to WiFi
    uasyncio.get_event_loop().run_until_complete(network_manager.client(WIFI_CONFIG.SSID, WIFI_CONFIG.PSK))

    # Clear the screen
    graphics.set_pen(1)
    graphics.clear()
    graphics.set_pen(0)

    # Grab the data
    socket = urequest.urlopen(URL)
    j = ujson.load(socket)
    socket.close()

    text = [j['activity'], j['type'], j['participants']]

    # Page lines!
    graphics.set_pen(3)
    graphics.line(0, 65, WIDTH, 65)
    for i in range(2, 13):
        graphics.line(0, i * 35, WIDTH, i * 35)

    # Page margin
    graphics.set_pen(4)
    graphics.line(50, 0, 50, HEIGHT)
    graphics.set_pen(0)

    # Main text
    graphics.set_font("cursive")
    graphics.set_pen(4)
    graphics.set_font("cursive")
    graphics.text("Activity Idea", 55, 30, WIDTH - 20, 2)
    graphics.set_pen(0)
    graphics.set_font("bitmap8")
    display_quote(text[0], 55, 170, 5, WIDTH - 20)

    graphics.set_pen(2)
    graphics.text("Activity Type: " + text[1], 55, HEIGHT - 45, WIDTH - 20, 2)
    graphics.text("Participants: " + str(text[2]), 400, HEIGHT - 45, WIDTH - 20, 2)

    graphics.update()

    # Time to have a little nap until the next update
    rtc.set_timer(UPDATE_INTERVAL)
    hold_vsys_en_pin.init(Pin.IN)
    time.sleep(UPDATE_INTERVAL)
