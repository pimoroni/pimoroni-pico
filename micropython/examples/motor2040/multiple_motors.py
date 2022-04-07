import time
import math
from motor import Motor, motor2040

"""
Demonstrates how to create multiple Motor objects and control them together.
"""

# Create a list of motors
MOTOR_PINS = [ motor2040.MOTOR_1, motor2040.MOTOR_2, motor2040.MOTOR_3, motor2040.MOTOR_4]
motors = [Motor(pins) for pins in MOTOR_PINS]

# Enable all motors (this puts them at the middle)
for m in motors:
    m.enable()
time.sleep(2)

# Go to min
for m in motors:
    m.full_positive()
time.sleep(2)

# Go to max
for m in motors:
    m.full_negative()
time.sleep(2)

# Go back to mid
for m in motors:
    m.stop()
time.sleep(2)

SWEEPS = 3              # How many sweeps of the motor to perform
STEPS = 10              # The number of discrete sweep steps
STEPS_INTERVAL = 0.5    # The time in seconds between each step of the sequence
SPEED_EXTENT = 1.0      # How far from zero to move the motor when sweeping

# Do a sine sweep
for j in range(SWEEPS):
    for i in range(360):
        speed = math.sin(math.radians(i)) * SPEED_EXTENT
        for s in motors:
            s.speed(speed)
        time.sleep(0.02)

# Do a stepped sweep
for j in range(SWEEPS):
    for i in range(0, STEPS):
        for m in motors:
            m.to_percent(i, 0, STEPS, 0.0 - SPEED_EXTENT, SPEED_EXTENT)
        time.sleep(STEPS_INTERVAL)
    for i in range(0, STEPS):
        for m in motors:
            m.to_percent(i, STEPS, 0, 0.0 - SPEED_EXTENT, SPEED_EXTENT)
        time.sleep(STEPS_INTERVAL)

# Disable the motors
for m in motors:
    m.disable()
