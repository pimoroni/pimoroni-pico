import math
import time

from breakout_ioexpander import BreakoutIOExpander
from pimoroni_i2c import PimoroniI2C
from breakout_encoder_wheel import BreakoutEncoderWheel, CENTRE, GPIOS, NUM_GPIOS

"""
Output a sine wave PWM sequence on the Encoder Wheel's side GPIO pins.

Press the centre button or Ctrl+C to stop the program.
"""

PINS_BREAKOUT_GARDEN = {"sda": 4, "scl": 5}
PINS_PICO_EXPLORER = {"sda": 20, "scl": 21}

SPEED = 5             # The speed that the PWM will cycle at
UPDATES = 50          # How many times to update LEDs and Servos per second
UPDATE_RATE = 1 / UPDATES
FREQUENCY = 1000      # The frequency to run the PWM at

# Create a new BreakoutEncoderWheel
i2c = PimoroniI2C(**PINS_BREAKOUT_GARDEN)
wheel = BreakoutEncoderWheel(i2c)

# Set the PWM frequency for the GPIOs
period = wheel.gpio_pwm_frequency(FREQUENCY)

# Set the GPIO pins to PWM outputs
for g in GPIOS:
    wheel.gpio_pin_mode(g, BreakoutIOExpander.PIN_PWM)

offset = 0.0


# Sleep until a specific time in the future. Use this instead of time.sleep() to correct for
# inconsistent timings when dealing with complex operations or external communication
def sleep_until(end_time):
    time_to_sleep = end_time - (time.ticks_ms() / 1000)
    if time_to_sleep > 0.0:
        time.sleep(time_to_sleep)


# Make PWM waves until the centre button is pressed
while not wheel.pressed(CENTRE):

    # Record the start time of this loop
    start_time = time.ticks_ms() / 1000

    offset += SPEED / 1000.0

    # Update all the PWMs
    for i in range(NUM_GPIOS):
        angle = ((i / NUM_GPIOS) + offset) * math.pi
        duty = int(((math.sin(angle) / 2) + 0.5) * period)

        # Set the GPIO pin to the new duty cycle, but do not load it yet
        wheel.gpio_pin_value(GPIOS[i], duty, load=False)

    # Have all the PWMs load at once
    wheel.gpio_pwm_load()

    # Sleep until the next update, accounting for how long the above operations took to perform
    sleep_until(start_time + UPDATE_RATE)

# Turn off the PWM outputs
for g in GPIOS:
    wheel.gpio_pin_value(g, 0)
