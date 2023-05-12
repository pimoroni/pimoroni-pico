import time
from pimoroni_i2c import PimoroniI2C
from pimoroni import BREAKOUT_GARDEN_I2C_PINS  # or PICO_EXPLORER_I2C_PINS or HEADER_I2C_PINS
from breakout_encoder_wheel import BreakoutEncoderWheel, NUM_LEDS

"""
Displays a rotating rainbow pattern on Encoder Wheel's LED ring.

Press Ctrl+C to stop the program.
"""

# Constants
SPEED = 5           # The speed that the LEDs will cycle at
BRIGHTNESS = 1.0    # The brightness of the LEDs
UPDATES = 50        # How many times the LEDs will be updated per second
UPDATE_RATE_US = 1000000 // UPDATES

# Create a new BreakoutEncoderWheel
i2c = PimoroniI2C(**BREAKOUT_GARDEN_I2C_PINS)
wheel = BreakoutEncoderWheel(i2c)

# Variables
offset = 0.0


# Sleep until a specific time in the future. Use this instead of time.sleep() to correct for
# inconsistent timings when dealing with complex operations or external communication
def sleep_until(end_time):
    time_to_sleep = time.ticks_diff(end_time, time.ticks_us())
    if time_to_sleep > 0:
        time.sleep_us(time_to_sleep)


# Make rainbows
while True:

    # Record the start time of this loop
    start_time = time.ticks_us()

    offset += SPEED / 1000.0

    # Update all the LEDs
    for i in range(NUM_LEDS):
        hue = float(i) / NUM_LEDS
        wheel.set_hsv(i, hue + offset, 1.0, BRIGHTNESS)
    wheel.show()

    # Sleep until the next update, accounting for how long the above operations took to perform
    sleep_until(time.ticks_add(start_time, UPDATE_RATE_US))
