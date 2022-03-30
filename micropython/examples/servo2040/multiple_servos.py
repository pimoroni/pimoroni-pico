import time
import math
from servo import Servo, servo2040

"""
Demonstrates how to create multiple Servo objects and control them together.
"""

# Create a list of servos for pins 0 to 3. Up to 16 servos can be created
START_PIN = servo2040.SERVO_1
END_PIN = servo2040.SERVO_4
servos = [Servo(i) for i in range(START_PIN, END_PIN + 1)]

# Enable all servos (this puts them at the middle)
for s in servos:
    s.enable()
time.sleep(2)

# Go to min
for s in servos:
    s.to_min()
time.sleep(2)

# Go to max
for s in servos:
    s.to_max()
time.sleep(2)

# Go back to mid
for s in servos:
    s.to_mid()
time.sleep(2)

SWEEPS = 3              # How many sweeps of the servo to perform
STEPS = 10              # The number of discrete sweep steps
STEPS_INTERVAL = 0.5    # The time in seconds between each step of the sequence
SWEEP_EXTENT = 90.0     # How far from zero to move the servo when sweeping

# Do a sine sweep
for j in range(SWEEPS):
    for i in range(360):
        value = math.sin(math.radians(i)) * SWEEP_EXTENT
        for s in servos:
            s.value(value)
        time.sleep(0.02)

# Do a stepped sweep
for j in range(SWEEPS):
    for i in range(0, STEPS):
        for s in servos:
            s.to_percent(i, 0, STEPS, 0.0 - SWEEP_EXTENT, SWEEP_EXTENT)
        time.sleep(STEPS_INTERVAL)
    for i in range(0, STEPS):
        for s in servos:
            s.to_percent(i, STEPS, 0, 0.0 - SWEEP_EXTENT, SWEEP_EXTENT)
        time.sleep(STEPS_INTERVAL)

# Disable the servos
for s in servos:
    s.disable()
