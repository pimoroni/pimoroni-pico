import plasma
from plasma import plasma_stick
from math import sin

"""
Simple pulsing effect generated using a sine wave.
"""

# Set how many LEDs you have
NUM_LEDS = 50

# we're using HSV colours in this example - find more at https://colorpicker.me/
# to convert a hue that's in degrees, divide it by 360
COLOUR = 0.5

# set up the WS2812 / NeoPixel™ LEDs
led_strip = plasma.WS2812(NUM_LEDS, 0, 0, plasma_stick.DAT, color_order=plasma.COLOR_ORDER_RGB)

# start updating the LED strip
led_strip.start()

offset = 0

while True:
    # use a sine wave to set the brightness
    for i in range(NUM_LEDS):
        led_strip.set_hsv(i, COLOUR, 1.0, sin(offset))
    offset += 0.002

#     # our sine wave goes between -1.0 and 1.0 - this means the LEDs will be off half the time
#     # this formula forces the brightness to be between 0.0 and 1.0
#     for i in range(NUM_LEDS):
#         led_strip.set_hsv(i, COLOUR, 1.0, (1 + sin(offset)) / 2)
#     offset += 0.002

#     # adjust the saturation instead of the brightness/value
#     for i in range(NUM_LEDS):
#         led_strip.set_hsv(i, COLOUR, (1 + sin(offset)) / 2, 0.8)
#     offset += 0.002
