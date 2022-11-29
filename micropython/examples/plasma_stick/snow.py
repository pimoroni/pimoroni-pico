import plasma
from plasma import plasma_stick
from random import uniform

"""
Snow in a bottle! Always winter, never Christmas!
Adjust SNOW_INTENSITY for more snow.
"""

# Set how many LEDs you have
NUM_LEDS = 50

# How much snow? [bigger number = more snowflakes]
SNOW_INTENSITY = 0.0002

# Change RGB colours here (RGB colour picker: https://g.co/kgs/k2Egjk )
BACKGROUND_COLOUR = [30, 50, 50]  # dim blue
SNOW_COLOUR = [240, 255, 255]  # bluish white

# how quickly current colour changes to target colour [1 - 255]
FADE_UP_SPEED = 255  # abrupt change for a snowflake
FADE_DOWN_SPEED = 1


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
        # randomly add snow
        if SNOW_INTENSITY > uniform(0, 1):
            # set a target to start a snowflake
            target_leds[i] = SNOW_COLOUR
        # slowly reset snowflake to background
        if current_leds[i] == target_leds[i]:
            target_leds[i] = BACKGROUND_COLOUR
    move_to_target()   # nudge our current colours closer to the target colours
    display_current()  # display current colours to strip
