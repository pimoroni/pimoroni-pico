import plasma
from plasma import plasma_stick
from random import uniform

"""
A festive sparkly effect. Play around with BACKGROUND_COLOUR and SPARKLE_COLOUR for different effects!
"""

# Set how many LEDs you have
NUM_LEDS = 50

# How many sparkles? [bigger number = more sparkles]
SPARKLE_INTENSITY = 0.005

# Change your colours here! RGB colour picker: https://g.co/kgs/k2Egjk
BACKGROUND_COLOUR = [50, 50, 0]
SPARKLE_COLOUR = [255, 255, 0]

# how quickly current colour changes to target colour [1 - 255]
FADE_UP_SPEED = 2
FADE_DOWN_SPEED = 2


def display_current():
    # paint our current LED colours to the strip
    for i in range(NUM_LEDS):
        led_strip.set_rgb(i, current_leds[i][0], current_leds[i][1], current_leds[i][2])


def move_to_target():
    # nudge our current colours closer to the target colours
    for i in range(NUM_LEDS):
        for c in range(3):  # 3 times, for R, G & B channels
            if current_leds[i][c] < target_leds[i][c]:
                current_leds[i][c] = min(current_leds[i][c] + FADE_UP_SPEED, target_leds[i][c])  # increase current, up to a maximum of target
            elif current_leds[i][c] > target_leds[i][c]:
                current_leds[i][c] = max(current_leds[i][c] - FADE_DOWN_SPEED, target_leds[i][c])  # reduce current, down to a minimum of target


# Create a list of [r, g, b] values that will hold current LED colours, for display
current_leds = [[0] * 3 for i in range(NUM_LEDS)]
# Create a list of [r, g, b] values that will hold target LED colours, to move towards
target_leds = [[0] * 3 for i in range(NUM_LEDS)]

# set up the WS2812 / NeoPixel™ LEDs
led_strip = plasma.WS2812(NUM_LEDS, 0, 0, plasma_stick.DAT, color_order=plasma.COLOR_ORDER_RGB)

# start updating the LED strip
led_strip.start()

while True:
    for i in range(NUM_LEDS):
        # randomly add sparkles
        if SPARKLE_INTENSITY > uniform(0, 1):
            # set a target to start a sparkle
            target_leds[i] = SPARKLE_COLOUR
        # for any sparkles that have achieved max sparkliness, reset them to background
        if current_leds[i] == target_leds[i]:
            target_leds[i] = BACKGROUND_COLOUR
    move_to_target()   # nudge our current colours closer to the target colours
    display_current()  # display current colours to strip
