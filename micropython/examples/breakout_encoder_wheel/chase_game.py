import random
from pimoroni_i2c import PimoroniI2C
from pimoroni import BREAKOUT_GARDEN_I2C_PINS  # or PICO_EXPLORER_I2C_PINS or HEADER_I2C_PINS
from breakout_encoder_wheel import BreakoutEncoderWheel, NUM_LEDS

"""
A simple alignment game. Use Encoder Wheel's rotary dial to align the coloured band
to the white goal. The closer to the goal, the greener your coloured band will be.
When you reach the goal, the goal will move to a new random position.

Press Ctrl+C to stop the program.
"""

i2c = PimoroniI2C(**BREAKOUT_GARDEN_I2C_PINS)
wheel = BreakoutEncoderWheel(i2c)

# The band colour hues to show in Angle mode
GOAL_HUE = 0.333
FAR_HUE = 0.0

# The width and colour settings for the band
BAND_WIDTH = 5.0
BAND_SATURATION = 1.0
BAND_IN_GOAL_SATURATION = 0.5
BAND_BRIGHTNESS = 1.0

# The width and colour settings for the goal
# Goal should be wider than the band by a small amount
GOAL_MARGIN = 1
GOAL_WIDTH = BAND_WIDTH + (2 * GOAL_MARGIN)
GOAL_BRIGHTNESS = 0.4


# Maps a value from one range to another
def map(x, in_min, in_max, out_min, out_max):
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min


# Shows a band and goal with the given widths at the positions on the strip
def colour_band(centre_position, width, goal_position, goal_width, hue):
    if centre_position >= 0.0 and width > 0.0 and goal_width > 0.0:
        band_start = centre_position - (width / 2)
        band_end = centre_position + (width / 2)
        band_centre = centre_position

        goal_start = goal_position - (goal_width / 2)
        goal_end = goal_position + (goal_width / 2)

        # Go through each led in the strip
        for i in range(NUM_LEDS):
            # Set saturation and brightness values for if the led is inside or outside of the goal
            saturation = BAND_SATURATION
            brightness = 0.0

            if i >= goal_start and i < goal_end:
                saturation = BAND_IN_GOAL_SATURATION
                brightness = GOAL_BRIGHTNESS
            if goal_end >= NUM_LEDS and i + NUM_LEDS < goal_end:
                saturation = BAND_IN_GOAL_SATURATION
                brightness = GOAL_BRIGHTNESS
            if goal_start < 0 and i - NUM_LEDS >= goal_start:
                saturation = BAND_IN_GOAL_SATURATION
                brightness = GOAL_BRIGHTNESS

            if i >= band_start and i < band_end:
                # Inside the band
                if i < band_centre:
                    # Transition into the band
                    val = map(i, band_centre, band_start, BAND_BRIGHTNESS, brightness)
                    sat = map(i, band_centre, band_start, BAND_SATURATION, saturation)
                else:
                    val = map(i, band_centre, band_end, BAND_BRIGHTNESS, brightness)
                    sat = map(i, band_centre, band_end, BAND_SATURATION, saturation)
                wheel.set_hsv(i, hue, sat, val)

            elif band_end >= NUM_LEDS and i + NUM_LEDS < band_end and i < band_centre:
                val = map(i + NUM_LEDS, band_centre, band_end, BAND_BRIGHTNESS, brightness)
                sat = map(i + NUM_LEDS, band_centre, band_end, BAND_SATURATION, saturation)
                wheel.set_hsv(i, hue, sat, val)

            elif band_start < 0 and i - NUM_LEDS >= band_start and i >= band_centre:
                val = map(i - NUM_LEDS, band_centre, band_start, BAND_BRIGHTNESS, brightness)
                sat = map(i - NUM_LEDS, band_centre, band_start, BAND_SATURATION, saturation)
                wheel.set_hsv(i, hue, sat, val)
            else:
                # Outside of the band
                wheel.set_hsv(i, hue, 0.0, brightness)
        wheel.show()


goal_position = 0.0
band_position = 0

while True:

    band_position = wheel.step()

    # Convert the difference between the band and goal positions into a colour hue
    if band_position > goal_position:
        diff1 = band_position - goal_position
        diff2 = (goal_position + NUM_LEDS) - band_position
    else:
        diff1 = goal_position - band_position
        diff2 = (band_position + NUM_LEDS) - goal_position

    position_diff = min(diff1, diff2)
    hue = map(position_diff, 0, NUM_LEDS // 2, GOAL_HUE, FAR_HUE)

    # Convert the band and goal positions to positions on the LED strip
    strip_band_position = map(band_position, 0, NUM_LEDS, 0.0, float(NUM_LEDS))
    strip_goal_position = map(goal_position, 0, NUM_LEDS, 0.0, float(NUM_LEDS))

    # Draw the band and goal
    colour_band(strip_band_position, BAND_WIDTH, strip_goal_position, GOAL_WIDTH, hue)

    # Check if the band is within the goal, and if so, set a new goal
    if band_position >= goal_position - GOAL_MARGIN and band_position <= goal_position + GOAL_MARGIN:
        goal_position = random.randint(0, NUM_LEDS - 1)
    if band_position >= NUM_LEDS and band_position + NUM_LEDS < goal_position + GOAL_MARGIN:
        goal_position = random.randint(0, NUM_LEDS - 1)
    if goal_position - GOAL_MARGIN < 0 and band_position - NUM_LEDS >= goal_position + GOAL_MARGIN:
        goal_position = random.randint(0, NUM_LEDS - 1)
