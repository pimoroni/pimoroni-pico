import time
import plasma
from plasma import plasma_stick
from random import random, choice

"""
A Christmas tree, with fairy lights!
This will probably work better if your LEDs are in a vaguely tree shaped bottle :)
"""

# Set how many LEDs you have
NUM_LEDS = 50

# we're using HSV colours in this example - find more at https://colorpicker.me/
# to convert a hue that's in degrees, divide it by 360
TREE_COLOUR = [0.34, 1.0, 0.6]
LIGHT_RATIO = 8  # every nth pixel is a light, the rest are tree.
LIGHT_COLOURS = ((0.0, 1.0, 1.0),   # red
                 (0.1, 1.0, 1.0),   # orange
                 (0.6, 1.0, 1.0),   # blue
                 (0.85, 0.4, 1.0))  # pink
LIGHT_CHANGE_CHANCE = 0.5  # change to 0.0 if you want static lights

# set up the WS2812 / NeoPixel™ LEDs
led_strip = plasma.WS2812(NUM_LEDS, 0, 0, plasma_stick.DAT, color_order=plasma.COLOR_ORDER_RGB)

# start updating the LED strip
led_strip.start()

# initial setup
for i in range(NUM_LEDS):
    if i % LIGHT_RATIO == 0:  # add an appropriate number of lights
        led_strip.set_hsv(i, *choice(LIGHT_COLOURS))  # choice randomly chooses from a list
    else:  # GREEN
        led_strip.set_hsv(i, *TREE_COLOUR)

# animate
while True:
    for i in range(NUM_LEDS):
        if (i % LIGHT_RATIO == 0) and (random() < LIGHT_CHANGE_CHANCE):
            led_strip.set_hsv(i, *choice(LIGHT_COLOURS))
    time.sleep(0.5)
