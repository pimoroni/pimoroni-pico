import time
import math
from motor import Motor, motor2040

"""
Demonstrates how to create multiple Motor objects and control them together.
"""

# Create a list of motors
MOTOR_PINS = [motor2040.MOTOR_A, motor2040.MOTOR_B, motor2040.MOTOR_C, motor2040.MOTOR_D]
motors = [Motor(pins) for pins in MOTOR_PINS]

# Enable all motors
for m in motors:
    m.enable()
time.sleep(2)

# Drive at full positive
for m in motors:
    m.full_positive()
time.sleep(2)

# Stop moving
for m in motors:
    m.stop()
time.sleep(2)

# Drive at full negative
for m in motors:
    m.full_negative()
time.sleep(2)

# Coast to a gradual stop
for m in motors:
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
        for m in motors:
            m.speed(speed)
        time.sleep(0.02)

# Do a stepped speed sweep
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
