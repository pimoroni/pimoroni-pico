import time
import math
from motor import Motor, motor2040

"""
Demonstrates how to create a Motor object and control it.
"""

# Create a motor on pins 4 and 5
m = Motor(motor2040.MOTOR_1)

# Enable the motor (this puts it at the middle)
m.enable()
time.sleep(2)

# Go to full positive
m.full_positive()
time.sleep(2)

# Go to full negative
m.full_negative()
time.sleep(2)

# Stop moving
m.stop()
time.sleep(2)


SWEEPS = 3              # How many sweeps of the motor to perform
STEPS = 10              # The number of discrete sweep steps
STEPS_INTERVAL = 0.5    # The time in seconds between each step of the sequence

# Do a sine sweep
for j in range(SWEEPS):
    for i in range(360):
        m.speed(math.sin(math.radians(i)))
        time.sleep(0.02)

# Do a stepped sweep
for j in range(SWEEPS):
    for i in range(0, STEPS):
        m.to_percent(i, 0, STEPS)
        time.sleep(STEPS_INTERVAL)
    for i in range(0, STEPS):
        m.to_percent(i, STEPS, 0)
        time.sleep(STEPS_INTERVAL)

# Disable the motor
m.disable()
