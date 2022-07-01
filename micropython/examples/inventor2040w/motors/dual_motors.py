import time
import math
from inventor import Inventor2040W

"""
Demonstrates how to control both motors on Inventor 2040 W.
"""

# Create a new Inventor2040W
board = Inventor2040W()

# Enable all motors
for m in board.motors:
    m.enable()
time.sleep(2)

# Drive at full positive
for m in board.motors:
    m.full_positive()
time.sleep(2)

# Stop moving
for m in board.motors:
    m.stop()
time.sleep(2)

# Drive at full negative
for m in board.motors:
    m.full_negative()
time.sleep(2)

# Coast to a gradual stop
for m in board.motors:
    m.coast()
time.sleep(2)


SWEEPS = 2              # How many speed sweeps of the motors to perform
STEPS = 10              # The number of discrete sweep steps
STEPS_INTERVAL = 0.5    # The time in seconds between each step of the sequence
SPEED_EXTENT = 1.0      # How far from zero to drive the motors when sweeping

# Do a sine speed sweep
for j in range(SWEEPS):
    for i in range(360):
        speed = math.sin(math.radians(i)) * SPEED_EXTENT
        for m in board.motors:
            m.speed(speed)
        time.sleep(0.02)

# Do a stepped speed sweep
for j in range(SWEEPS):
    for i in range(0, STEPS):
        for m in board.motors:
            m.to_percent(i, 0, STEPS, 0.0 - SPEED_EXTENT, SPEED_EXTENT)
        time.sleep(STEPS_INTERVAL)
    for i in range(0, STEPS):
        for m in board.motors:
            m.to_percent(i, STEPS, 0, 0.0 - SPEED_EXTENT, SPEED_EXTENT)
        time.sleep(STEPS_INTERVAL)

# Disable the motors
for m in board.motors:
    m.disable()
