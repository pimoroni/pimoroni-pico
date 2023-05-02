import time

from pimoroni_i2c import PimoroniI2C
from breakout_encoder_wheel import BreakoutEncoderWheel, NUM_LEDS

"""
Displays a rotating rainbow pattern on Encoder Wheel's LED ring.

Press Ctrl+C to stop the program.
"""

PINS_BREAKOUT_GARDEN = {"sda": 4, "scl": 5}
PINS_PICO_EXPLORER = {"sda": 20, "scl": 21}

# Constants
SPEED = 5           # The speed that the LEDs will cycle at
BRIGHTNESS = 1.0    # The brightness of the LEDs
UPDATES = 50        # How many times the LEDs will be updated per second
UPDATE_RATE = 1 / UPDATES

# Create a new BreakoutEncoderWheel
i2c = PimoroniI2C(**PINS_BREAKOUT_GARDEN)
wheel = BreakoutEncoderWheel(i2c)

# Variables
offset = 0.0


# Sleep until a specific time in the future. Use this instead of time.sleep() to correct for
# inconsistent timings when dealing with complex operations or external communication
def sleep_until(end_time):
    time_to_sleep = end_time - (time.ticks_ms() / 1000)
    if time_to_sleep > 0.0:
        time.sleep(time_to_sleep)


# Make rainbows
while True:

    # Record the start time of this loop
    start_time = time.ticks_ms() / 1000

    offset += SPEED / 1000.0

    # Update all the LEDs
    for i in range(NUM_LEDS):
        hue = float(i) / NUM_LEDS
        wheel.set_hsv(i, hue + offset, 1.0, BRIGHTNESS)
    wheel.show()

    # Sleep until the next update, accounting for how long the above operations took to perform
    sleep_until(start_time + UPDATE_RATE)
