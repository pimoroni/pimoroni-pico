import time
import math
from inventor import Inventor2040W, SERVO_1

"""
Demonstrates how to control a single servo on Inventor 2040 W.
"""

# Create a new Inventor2040W
board = Inventor2040W()

# Access the servo from Inventor and enable it (this puts it at the middle)
s = board.servos[SERVO_1]
s.enable()
time.sleep(2)

# Go to min
s.to_min()
time.sleep(2)

# Go to max
s.to_max()
time.sleep(2)

# Go back to mid
s.to_mid()
time.sleep(2)


SWEEPS = 3              # How many sweeps of the servo to perform
STEPS = 10              # The number of discrete sweep steps
STEPS_INTERVAL = 0.5    # The time in seconds between each step of the sequence
SWEEP_EXTENT = 90.0     # How far from zero to move the servo when sweeping

# Do a sine sweep
for j in range(SWEEPS):
    for i in range(360):
        s.value(math.sin(math.radians(i)) * SWEEP_EXTENT)
        time.sleep(0.02)

# Do a stepped sweep
for j in range(SWEEPS):
    for i in range(0, STEPS):
        s.to_percent(i, 0, STEPS, 0.0 - SWEEP_EXTENT, SWEEP_EXTENT)
        time.sleep(STEPS_INTERVAL)
    for i in range(0, STEPS):
        s.to_percent(i, STEPS, 0, 0.0 - SWEEP_EXTENT, SWEEP_EXTENT)
        time.sleep(STEPS_INTERVAL)

# Disable the servo
s.disable()
