# This simple example randomises LED colours and brightness for a subtly sparkly effect.

import plasma
from plasma import plasma2040
import time
# Random functions! randrange is for picking integers from a range, and uniform is for floats.
from random import randrange, uniform

# Set how many LEDs you have
NUM_LEDS = 50

# Pick what bits of the colour wheel to use (from 0-360°, try https://www.cssscript.com/demo/hsv-hsl-color-wheel-picker-reinvented/ )
HUE_START = 180
HUE_END = 260

# Set up brightness minimum and maximum (between 0.0 and 1.0)
BRIGHTNESS_MIN = 0.2
BRIGHTNESS_MAX = 0.7

# Set up speed (wait time between updates, in seconds.)
SPEED = 0.3

# Pick *one* LED type by uncommenting the relevant line below:

# APA102 / DotStar™ LEDs
# led_strip = plasma.APA102(NUM_LEDS, 0, 0, plasma2040.DAT, plasma2040.CLK)

# WS2812 / NeoPixel™ LEDs
led_strip = plasma.WS2812(NUM_LEDS, 0, 0, plasma2040.DAT)

# Start updating the LED strip
led_strip.start()

# Light up all the leds random colours and brightnesses from the specified ranges...
for i in range(NUM_LEDS):
    led_strip.set_hsv(i, randrange(HUE_START, HUE_END) / 360, 1.0, uniform(BRIGHTNESS_MIN, BRIGHTNESS_MAX))

while True:
    # ...and then update one random pixel at a time to keep things fresh and sparkly.
    # Comment out the lines below if you want static lights.
    led_strip.set_hsv(randrange(0, NUM_LEDS), randrange(HUE_START, HUE_END) / 360, 1.0, uniform(BRIGHTNESS_MIN, BRIGHTNESS_MAX))
    time.sleep(SPEED)
