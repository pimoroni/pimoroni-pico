import plasma
from plasma import plasma2040
import time
from random import random, uniform

"""
A basic fire effect.
"""

# Set how many LEDs you have
NUM_LEDS = 50

# WS2812 / NeoPixel™ LEDs
led_strip = plasma.WS2812(NUM_LEDS, 0, 0, plasma2040.DAT)

# Start updating the LED strip
led_strip.start()

while True:
    # fire effect! Random red/orange hue, full saturation, random brightness
    for i in range(NUM_LEDS):
        led_strip.set_hsv(i, uniform(0.0, 50 / 360), 1.0, random())
    time.sleep(0.1)
