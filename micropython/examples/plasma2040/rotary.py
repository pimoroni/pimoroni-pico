import plasma
from plasma import plasma2040
import time
import math

# Import helpers for RGB LEDs and Buttons
from pimoroni import RGBLED, Button

# Import bme68x and I2C helper
from breakout_encoder import BreakoutEncoder
from pimoroni_i2c import PimoroniI2C

# Press "B" to enable cycling.
# Press "A" to change the encoder mode.
# Press "Boot" to reset the effects back to default.

# Set how many LEDs you have
NUM_LEDS = 30

# The speed that the LEDs will start cycling at
DEFAULT_SPEED = 20

# The hue (in degrees) that the LEDs will start at
DEFAULT_HUE = 0

# The angle (in degrees) from the hue, that the LEDs will end at
DEFAULT_ANGLE = 120

# The brightness (between 0 and 31) to set the LEDs to
DEFAULT_BRIGHTNESS = 16

# How many times the LEDs will be updated per second
UPDATES = 60


# Pick *one* LED type by uncommenting the relevant line below:

# APA102 / DotStar™ LEDs
led_strip = plasma.APA102(NUM_LEDS, 0, 0, plasma2040.DAT, plasma2040.CLK)

# WS2812 / NeoPixel™ LEDs
# led_strip = plasma.WS2812(NUM_LEDS, 0, 0, plasma2040.DAT)

user_sw = Button(plasma2040.USER_SW, repeat_time=0)
button_a = Button(plasma2040.BUTTON_A, repeat_time=0)
button_b = Button(plasma2040.BUTTON_B, repeat_time=0)
led = RGBLED(plasma2040.LED_R, plasma2040.LED_G, plasma2040.LED_B)

PINS_PLASMA2040 = {"sda": plasma2040.SDA, "scl": plasma2040.SCL}

i2c = PimoroniI2C(**PINS_PLASMA2040)
enc = BreakoutEncoder(i2c)
enc.set_brightness(1.0)


COLOUR, ANGLE, BRIGHTNESS, SPEED = range(4)


def wrap(v, mn, mx):
    if v <= mn:
        v += mx - mn

    if v > mx:
        v -= mx - mn

    return v


def colour_cycle(hue, t, angle):
    t /= 200.0

    for i in range(NUM_LEDS):
        percent_along = float(i) / NUM_LEDS
        offset = math.sin((percent_along + 0.5 + t) * math.pi) * angle
        h = wrap((hue + offset) / 360.0, 0.0, 1.0)
        led_strip.set_hsv(i, h, 1.0, 1.0)


def speed_gauge(v, vmax=100):
    light_pixels = NUM_LEDS * v / vmax

    for i in range(NUM_LEDS):
        if i < light_pixels:
            led_strip.set_rgb(i, 0, 255, 0)
        else:
            led_strip.set_rgb(i, 255, 0, 0)


def brightness_gauge(v, vmax=100):
    light_pixels = NUM_LEDS * v / vmax

    for i in range(NUM_LEDS):
        if i < light_pixels:
            led_strip.set_rgb(i, 64, 64, 64)
        else:
            led_strip.set_rgb(i, 0, 0, 0)


# Start updating the LED strip
led_strip.start()

enc.clear_interrupt_flag()

# Initialise the default values
speed = DEFAULT_SPEED
hue = DEFAULT_HUE
angle = DEFAULT_ANGLE
brightness = DEFAULT_BRIGHTNESS

cycle = True
mode = COLOUR
start_time = time.ticks_ms()

while True:
    t = time.ticks_ms() - start_time

    if enc.get_interrupt_flag():
        count = enc.read()
        enc.clear_interrupt_flag()
        enc.clear()

        cycle = False

        if mode == COLOUR:
            hue += count
            hue = min(359, max(0, hue))
            colour_cycle(hue, 0, angle)

        elif mode == ANGLE:
            angle += count
            angle = min(359, max(0, angle))
            colour_cycle(hue, 0, angle)

        elif mode == BRIGHTNESS:
            brightness += count
            brightness = min(31, max(0, brightness))
            led_strip.set_brightness(brightness)
            brightness_gauge(brightness, 31)

        elif mode == SPEED:
            speed += count
            speed = min(100, max(0, speed))
            speed_gauge(speed, 100)

    sw_pressed = user_sw.read()
    a_pressed = button_a.read()
    b_pressed = button_b.read()

    if sw_pressed:
        speed = DEFAULT_SPEED
        hue = DEFAULT_HUE
        angle = DEFAULT_ANGLE
        brightness = DEFAULT_BRIGHTNESS

    if b_pressed:
        if not cycle:
            start_time = time.ticks_ms()
        cycle = True
    speed = min(255, max(1, speed))

    if mode == COLOUR:
        led.set_rgb(255, 0, 0)
        if a_pressed:
            mode = ANGLE

    elif mode == ANGLE:
        led.set_rgb(255, 255, 0)
        if a_pressed:
            mode = BRIGHTNESS

    elif mode == BRIGHTNESS:
        led.set_rgb(0, 255, 0)
        if a_pressed:
            mode = SPEED

    elif mode == SPEED:
        led.set_rgb(0, 0, 255)
        if a_pressed:
            mode = COLOUR

    if cycle:
        colour_cycle(hue, float(t * speed) / 100.0, angle)

    mid_led = led_strip.get(int(NUM_LEDS / 2))
    enc.set_led(int(mid_led[0]), int(mid_led[1]), int(mid_led[2]))

    time.sleep(1.0 / UPDATES)
