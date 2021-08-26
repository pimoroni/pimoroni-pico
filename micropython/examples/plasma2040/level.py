import plasma
from plasma import plasma2040
import time
import math
import random

# Import helpers for RGB LEDs and Buttons
from pimoroni import RGBLED, Button

# Import msa301 and I2C helper
from breakout_msa301 import BreakoutMSA301
from pimoroni_i2c import PimoroniI2C

# A simple balancing game, where you use the MSA301 accelerometer to line up a band with a goal on the strip.
# This can either be done using:
# - Angle mode: Where position on the strip directly matches the accelerometer's angle
# - Velocity mode: Where tilting the accelerometer changes the speed the band moves at
# When the goal position is reached, a new position is randomly selected

# Press "A" to change the game mode.
# Press "B" to start or stop the game mode.
# Press "Boot" to invert the direction of the accelerometer tilt

# Set how many LEDs you have
NUM_LEDS = 30

# How many times the LEDs will be updated per second
UPDATES = 60

# The sensitivity of the accelerometer input
ANGLE_SENSITIVITY = 0.05
VELOCITY_SENSITIVITY = 0.2 / UPDATES

# The band colour hues to show in Angle mode
ANGLE_MODE_GOAL_HUE = 0.333
ANGLE_MODE_EDGE_HUE = 0.0

# The band colour hues to show in Velocity mode
VELOCITY_MODE_GOAL_HUE = 0.667
VELOCITY_MODE_EDGE_HUE = 1.0

# The width and colour settings for the band
BAND_PIXEL_WIDTH = 2.0
BAND_SATURATION = 1.0
BAND_IN_GOAL_SATURATION = 0.5
BAND_BRIGHTNESS = 1.0

# The width and colour settings for the goal
# Goal should be wider than the band by a small amount
GOAL_PIXEL_WIDTH = BAND_PIXEL_WIDTH + 2.0
GOAL_BRIGHTNESS = 0.1

# The percentage of the new angle (between 0.0 and 1.0) to apply to the last angle
# Has the effect of smoothing out the reading, at the cost of making it slower to react
SMOOTHING_FACTOR = 0.1


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
msa = BreakoutMSA301(i2c)


ANGLE, VELOCITY = range(2)


# Maps a value from one range to another
def map(x, in_min, in_max, out_min, out_max):
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min


# Shows a band and goal with the given widths at the positions on the strip
def colour_band(centre_position, width, goal_position, goal_width, hue):
    if centre_position >= 0.0 and width > 0.0 and goal_width > 0.0:
        band_pixels_start = centre_position - (width / 2)
        band_pixels_end = centre_position + (width / 2)

        goal_pixels_start = goal_position - (goal_width / 2)
        goal_pixels_end = goal_position + (goal_width / 2)

        # Go through each led in the strip
        for i in range(NUM_LEDS):
            # Set saturation and brightness values for if the led is inside or outside of the goal
            saturation = BAND_SATURATION
            brightness = 0.0
            if i >= goal_pixels_start and i < goal_pixels_end:
                saturation = BAND_IN_GOAL_SATURATION
                brightness = GOAL_BRIGHTNESS

            i2 = i + 1
            if i2 <= band_pixels_end:
                if i2 <= band_pixels_start:
                    # Outside of the band
                    led_strip.set_hsv(i, hue, 0.0, brightness)
                elif i <= band_pixels_start:
                    # Transition into the band
                    val = map(band_pixels_start, float(i), float(i2), BAND_BRIGHTNESS, brightness)
                    sat = map(band_pixels_start, float(i), float(i2), BAND_SATURATION, saturation)
                    led_strip.set_hsv(i, hue, sat, val)
                else:
                    # Inside the band
                    led_strip.set_hsv(i, hue, 1.0, 1.0)

            elif i <= band_pixels_end:
                # Transition out of the band
                val = map(band_pixels_end, float(i), float(i2), brightness, BAND_BRIGHTNESS)
                sat = map(band_pixels_end, float(i), float(i2), saturation, BAND_SATURATION)
                led_strip.set_hsv(i, hue, sat, val)
            else:
                # Outside of the band
                led_strip.set_hsv(i, hue, 0.0, brightness)


mode = ANGLE

goal_position = 0.0
measured_angle = 0.0
invert = False
game_mode = False

# Start updating the LED strip
led_strip.start()

while True:
    # Read the x and y axes of the accelerometer
    x = msa.get_x_axis()
    y = msa.get_y_axis()

    # Convert those values to an angle in degrees, and invert if selected
    new_measured_angle = (math.atan2(x, -y) * 180.0) / math.pi
    if invert:
        new_measured_angle = -new_measured_angle
    print("Angle:", new_measured_angle, "deg")

    # Smooth out the measured angle
    measured_angle = ((new_measured_angle - measured_angle) * SMOOTHING_FACTOR) + measured_angle

    if mode == ANGLE:
        # Apply the measured angle directly to the band position, clamping it between -1 and +1
        band_position = measured_angle * ANGLE_SENSITIVITY
        band_position = min(1.0, max(-1.0, band_position))

        # Convert the difference between the band and goal positions into a colour hue
        position_diff = min(abs(band_position - goal_position), 1.0)
        hue = map(position_diff, 0.0, 1.0, ANGLE_MODE_GOAL_HUE, ANGLE_MODE_EDGE_HUE)

    elif mode == VELOCITY:
        # Apply the measured angle as a velocity to the band position, clamping it between -1 and +1
        band_position += measured_angle * VELOCITY_SENSITIVITY
        band_position = min(1.0, max(-1.0, band_position))

        # Convert the difference between the band and goal positions into a colour hue
        position_diff = min(abs(band_position - goal_position), 1.0)
        hue = map(position_diff, 0.0, 1.0, VELOCITY_MODE_GOAL_HUE, VELOCITY_MODE_EDGE_HUE)

    # Convert the band and goal positions to positions on the LED strip
    strip_band_position = map(band_position, -1.0, 1.0, 0.0, float(NUM_LEDS))
    strip_goal_position = map(goal_position, -1.0, 1.0, 0.0, float(NUM_LEDS))

    # Draw the band and goal
    colour_band(strip_band_position, BAND_PIXEL_WIDTH, strip_goal_position, GOAL_PIXEL_WIDTH, hue)

    sw_pressed = user_sw.read()
    a_pressed = button_a.read()
    b_pressed = button_b.read()

    if b_pressed:
        game_mode = not game_mode

    if sw_pressed:
        invert = not invert

    if mode == ANGLE:
        if game_mode:
            led.set_rgb(255, 255, 0)
        else:
            led.set_rgb(0, 255, 0)
        if a_pressed:
            mode = VELOCITY

    elif mode == VELOCITY:
        if game_mode:
            led.set_rgb(255, 0, 255)
        else:
            led.set_rgb(0, 0, 255)
        if a_pressed:
            mode = ANGLE

    if game_mode:
        # Check if the band is within the goal, and if so, set a new goal
        above_lower = strip_band_position >= strip_goal_position - (GOAL_PIXEL_WIDTH - BAND_PIXEL_WIDTH) / 2
        below_upper = strip_band_position <= strip_goal_position + (GOAL_PIXEL_WIDTH - BAND_PIXEL_WIDTH) / 2
        if above_lower and below_upper:
            goal_position = random.uniform(-1.0, 1.0)

    time.sleep(1.0 / UPDATES)
