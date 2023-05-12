import math
import time
from pimoroni_i2c import PimoroniI2C
from pimoroni import BREAKOUT_GARDEN_I2C_PINS  # or PICO_EXPLORER_I2C_PINS or HEADER_I2C_PINS
from breakout_encoder_wheel import BreakoutEncoderWheel, CENTRE, NUM_LEDS

"""
Display a circular stop-watch on the Encoder Wheel's LED ring.

Press the centre button to start the stopwatch, then again to pause and resume.

Press Ctrl+C to stop the program.
"""

# Constants
BRIGHTNESS = 1.0                    # The brightness of the LEDs when the stopwatch is running
UPDATES = 50                        # How many times the LEDs will be updated per second
MINUTE_UPDATES = UPDATES * 60       # How many times the LEDs will be updated per minute
HOUR_UPDATES = MINUTE_UPDATES * 60  # How many times the LEDs will be updated per hour
UPDATE_RATE_US = 1000000 // UPDATES

IDLE_PULSE_MIN = 0.2                # The brightness (between 0.0 and 1.0) that the idle pulse will go down to
IDLE_PULSE_MAX = 0.5                # The brightness (between 0.0 and 1.0) that the idle pulse will go up to
IDLE_PULSE_TIME = 2                 # The time (in seconds) to perform a complete idle pulse
UPDATES_PER_PULSE = IDLE_PULSE_TIME * UPDATES

IDLE, COUNTING, PAUSED = range(3)   # The state constants used for program flow

# Create a new BreakoutEncoderWheel
i2c = PimoroniI2C(**BREAKOUT_GARDEN_I2C_PINS)
wheel = BreakoutEncoderWheel(i2c)

# Variables
state = IDLE
idle_update = 0
second_update = 0
minute_update = 0
hour_update = 0
last_centre_pressed = False


# Simple function to clamp a value between a minimum and maximum
def clamp(n, smallest, largest):
    return max(smallest, min(n, largest))


# Sleep until a specific time in the future. Use this instead of time.sleep() to correct for
# inconsistent timings when dealing with complex operations or external communication
def sleep_until(end_time):
    time_to_sleep = time.ticks_diff(end_time, time.ticks_us())
    if time_to_sleep > 0:
        time.sleep_us(time_to_sleep)


# Record the current time
current_time = time.ticks_us()

# Run the update loop forever
while True:

    # Read whether or not the wheen centre has been pressed
    centre_pressed = wheel.pressed(CENTRE)
    if centre_pressed and centre_pressed != last_centre_pressed:
        if state == IDLE:           # If we're currently idle, switch to counting
            second_update = 0
            minute_update = 0
            hour_update = 0
            state = COUNTING

        elif state == COUNTING:     # If we're counting, switch to paused
            state = PAUSED

        elif state == PAUSED:       # If we're paused, switch back to counting
            state = COUNTING
    last_centre_pressed = centre_pressed

    # If we're idle, perform a pulsing animation to show the stopwatch is ready to go
    if state == IDLE:
        percent_along = min(idle_update / UPDATES_PER_PULSE, 1.0)
        brightness = ((math.cos(percent_along * math.pi * 2) + 1.0) / 2.0) * ((IDLE_PULSE_MAX - IDLE_PULSE_MIN)) + IDLE_PULSE_MIN
        # Update all the LEDs
        for i in range(NUM_LEDS):
            wheel.set_hsv(i, 0.0, 0.0, brightness)
        wheel.show()

        # Advance to the next update, wrapping around to zero if at the end
        idle_update += 1
        if idle_update >= UPDATES_PER_PULSE:
            idle_update = 0

    # If we're counting, perform the stopwatch animation
    elif state == COUNTING:
        # Calculate how many LED channels should light, as a proportion of a second, minute, and hour
        r_to_light = min(second_update / UPDATES, 1.0) * 24
        g_to_light = min(minute_update / MINUTE_UPDATES, 1.0) * 24
        b_to_light = min(hour_update / HOUR_UPDATES, 1.0) * 24

        # Set each LED, such that ones below the current time are fully lit, ones after
        # are off, and the one at the transition is at a percentage of the brightness
        for i in range(NUM_LEDS):
            r = int(clamp(r_to_light - i, 0.0, 1.0) * BRIGHTNESS * 255)
            g = int(clamp(g_to_light - i, 0.0, 1.0) * BRIGHTNESS * 255)
            b = int(clamp(b_to_light - i, 0.0, 1.0) * BRIGHTNESS * 255)
            wheel.set_rgb(i, r, g, b)
        wheel.show()

        # Advance the second updates count, wrapping around to zero if at the end
        second_update += 1
        if second_update >= UPDATES:
            second_update = 0

        # Advance the minute updates count, wrapping around to zero if at the end
        minute_update += 1
        if minute_update >= MINUTE_UPDATES:
            minute_update = 0

        # Advance the hour updates count, wrapping around to zero if at the end
        hour_update += 1
        if hour_update >= HOUR_UPDATES:
            hour_update = 0

    # Sleep until the next update, accounting for how long the above operations took to perform
    current_time = time.ticks_add(current_time, UPDATE_RATE_US)
    sleep_until(current_time)
