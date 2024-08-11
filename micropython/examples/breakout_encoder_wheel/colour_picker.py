import time
from pimoroni_i2c import PimoroniI2C
from pimoroni import BREAKOUT_GARDEN_I2C_PINS  # or PICO_EXPLORER_I2C_PINS or HEADER_I2C_PINS
from breakout_encoder_wheel import BreakoutEncoderWheel, UP, DOWN, LEFT, RIGHT, CENTRE, NUM_LEDS

"""
Create a colour wheel on the Encoder Wheel's LED ring, and use all functions of the wheel to interact with it.

Rotate the wheel to select a Hue
Press the up direction to increase Brightness
Press the down direction to decrease Brightness
Press the left direction to decrease Saturation
Press the right direction to increase Saturation
Press the centre to hide the selection marker

Press Ctrl+C to stop the program.
"""

# Constants
BRIGHTNESS_STEP = 0.02      # How much to increase or decrease the brightness each update
SATURATION_STEP = 0.02      # How much to increase or decrease the saturation each update
UPDATES = 50                # How many times to update the LEDs per second
UPDATE_RATE_US = 1000000 // UPDATES

# Create a new BreakoutEncoderWheel
i2c = PimoroniI2C(**BREAKOUT_GARDEN_I2C_PINS)
wheel = BreakoutEncoderWheel(i2c)

# Variables
brightness = 1.0
saturation = 1.0
position = 0
changed = True
last_centre_pressed = False


# From CPython Lib/colorsys.py
def hsv_to_rgb(h, s, v):
    if s == 0.0:
        return v, v, v
    i = int(h * 6.0)
    f = (h * 6.0) - i
    p = v * (1.0 - s)
    q = v * (1.0 - s * f)
    t = v * (1.0 - s * (1.0 - f))
    i = i % 6
    if i == 0:
        return v, t, p
    if i == 1:
        return q, v, p
    if i == 2:
        return p, v, t
    if i == 3:
        return p, q, v
    if i == 4:
        return t, p, v
    if i == 5:
        return v, p, q


# Simple function to clamp a value between 0.0 and 1.0
def clamp01(value):
    return max(min(value, 1.0), 0.0)


# Sleep until a specific time in the future. Use this instead of time.sleep() to correct for
# inconsistent timings when dealing with complex operations or external communication
def sleep_until(end_time):
    time_to_sleep = time.ticks_diff(end_time, time.ticks_us())
    if time_to_sleep > 0:
        time.sleep_us(time_to_sleep)


while True:
    # Record the start time of this loop
    start_time = time.ticks_us()

    # If up is pressed, increase the brightness
    if wheel.pressed(UP):
        brightness += BRIGHTNESS_STEP
        changed = True  # Trigger a change

    # If down is pressed, decrease the brightness
    if wheel.pressed(DOWN):
        brightness -= BRIGHTNESS_STEP
        changed = True  # Trigger a change

    # If right is pressed, increase the saturation
    if wheel.pressed(RIGHT):
        saturation += SATURATION_STEP
        changed = True  # Trigger a change

    # If left is pressed, decrease the saturation
    if wheel.pressed(LEFT):
        saturation -= SATURATION_STEP
        changed = True  # Trigger a change

    # Limit the brightness and saturation between 0.0 and 1.0
    brightness = clamp01(brightness)
    saturation = clamp01(saturation)

    # Check if the encoder has been turned
    if wheel.delta() != 0:
        # Update the position based on the count change
        position = wheel.step()
        changed = True  # Trigger a change

    # If centre is pressed, trigger a change
    centre_pressed = wheel.pressed(CENTRE)
    if centre_pressed != last_centre_pressed:
        changed = True
    last_centre_pressed = centre_pressed

    # Was a change triggered?
    if changed:
        # Print the colour at the current hue, saturation, and brightness
        r, g, b = [int(c * 255) for c in hsv_to_rgb(position / NUM_LEDS, saturation, brightness)]
        print("Colour Code = #", '{:02x}'.format(r), '{:02x}'.format(g), '{:02x}'.format(b), sep="")

        # Set the LED at the current position to either the actual colour,
        # or an inverted version to show a "selection marker"
        if centre_pressed:
            wheel.set_rgb(position, r, g, b)
        else:
            wheel.set_rgb(position, 255 - r, 255 - g, 255 - b)

        # Set the LEDs below the current position
        for i in range(0, position):
            wheel.set_hsv(i, i / NUM_LEDS, saturation, brightness)

        # Set the LEDs after the current position
        for i in range(position + 1, NUM_LEDS):
            wheel.set_hsv(i, i / NUM_LEDS, saturation, brightness)
        wheel.show()
        changed = False

    # Sleep until the next update, accounting for how long the above operations took to perform
    sleep_until(time.ticks_add(start_time, UPDATE_RATE_US))
