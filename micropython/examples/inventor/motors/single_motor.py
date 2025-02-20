import time
import math
from inventor import Inventor, MOTOR_A

"""
Demonstrates how to control a motor on Inventor 2040/2350 W.
"""

# Constants
SWEEPS = 2              # How many speed sweeps of the motor to perform
STEPS = 10              # The number of discrete sweep steps
STEPS_INTERVAL = 0.5    # The time in seconds between each step of the sequence
SPEED_EXTENT = 1.0      # How far from zero to drive the motor when sweeping

# Create a new Inventor object and access a motor
board = Inventor()
m = board.motors[MOTOR_A]

# Wrap the code in a try block, to catch any exceptions (including KeyboardInterrupt)
try:
    # Enable the motor
    m.enable()
    time.sleep(2)

    # Drive at full positive
    m.full_positive()
    time.sleep(2)

    # Stop moving
    m.stop()
    time.sleep(2)

    # Drive at full negative
    m.full_negative()
    time.sleep(2)

    # Coast to a gradual stop
    m.coast()
    time.sleep(2)

    # Do a sine speed sweep
    for j in range(SWEEPS):
        for i in range(360):
            m.speed(math.sin(math.radians(i)) * SPEED_EXTENT)
            time.sleep(0.02)

    # Do a stepped speed sweep
    for j in range(SWEEPS):
        for i in range(0, STEPS):
            m.to_percent(i, 0, STEPS, 0.0 - SPEED_EXTENT, SPEED_EXTENT)
            time.sleep(STEPS_INTERVAL)
        for i in range(0, STEPS):
            m.to_percent(i, STEPS, 0, 0.0 - SPEED_EXTENT, SPEED_EXTENT)
            time.sleep(STEPS_INTERVAL)

# Put the board back into a safe state, regardless of how the program may have ended
finally:
    # Disable the motor
    m.disable()
