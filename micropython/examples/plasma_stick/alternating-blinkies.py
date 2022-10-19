import plasma
from plasma import plasma_stick
import time

"""
This super simple example sets up two alternating colours, great for festive lights!
"""

# Set how many LEDs you have
NUM_LEDS = 50

# Pick two hues from the colour wheel (from 0-360°, try https://www.cssscript.com/demo/hsv-hsl-color-wheel-picker-reinvented/ )
HUE_1 = 40
HUE_2 = 285

# Set up brightness (between 0 and 1)
BRIGHTNESS = 0.5

# Set up speed (wait time between colour changes, in seconds)
SPEED = 1

# WS2812 / NeoPixel™ LEDs
led_strip = plasma.WS2812(NUM_LEDS, 0, 0, plasma_stick.DAT, color_order=plasma.COLOR_ORDER_RGB)

# Start updating the LED strip
led_strip.start()

while True:
    for i in range(NUM_LEDS):
        # the if statements below use a modulo operation to identify the even and odd numbered LEDs
        if (i % 2) == 0:
            led_strip.set_hsv(i, HUE_1 / 360, 1.0, BRIGHTNESS)
        else:
            led_strip.set_hsv(i, HUE_2 / 360, 1.0, BRIGHTNESS)
    time.sleep(SPEED)

    for i in range(NUM_LEDS):
        if (i % 2) == 0:
            led_strip.set_hsv(i, HUE_2 / 360, 1.0, BRIGHTNESS)
        else:
            led_strip.set_hsv(i, HUE_1 / 360, 1.0, BRIGHTNESS)
    time.sleep(SPEED)
