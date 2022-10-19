import plasma
from plasma import plasma_stick
import time

"""
Make some rainbows!
"""

# Set how many LEDs you have
NUM_LEDS = 50

# The SPEED that the LEDs cycle at (1 - 255)
SPEED = 20

# How many times the LEDs will be updated per second
UPDATES = 60

# WS2812 / NeoPixel™ LEDs
led_strip = plasma.WS2812(NUM_LEDS, 0, 0, plasma_stick.DAT, color_order=plasma.COLOR_ORDER_RGB)

# Start updating the LED strip
led_strip.start()

offset = 0.0

# Make rainbows
while True:

    SPEED = min(255, max(1, SPEED))
    offset += float(SPEED) / 2000.0

    for i in range(NUM_LEDS):
        hue = float(i) / NUM_LEDS
        led_strip.set_hsv(i, hue + offset, 1.0, 1.0)

    time.sleep(1.0 / UPDATES)
