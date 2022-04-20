import gc
import time
import math
from motor import MotorCluster, motor2040

"""
Demonstrates how to create a MotorCluster object to control multiple motors at once.

NOTE: MotorCluster uses the RP2040's PIO system, and as
such may have problems when running code multiple times.
If you encounter issues, try resetting your board.
"""

# Free up hardware resources ahead of creating a new MotorCluster
gc.collect()

# Create a motor cluster, using PIO 0 and State Machine 0
MOTOR_PINS = [motor2040.MOTOR_A, motor2040.MOTOR_B, motor2040.MOTOR_C, motor2040.MOTOR_D]
motors = MotorCluster(pio=0, sm=0, pins=MOTOR_PINS)

# Enable all motors
motors.enable_all()
time.sleep(2)

# Drive at full positive
motors.all_full_positive()
time.sleep(2)

# Stop all moving
motors.stop_all()
time.sleep(2)

# Drive at full negative
motors.all_full_negative()
time.sleep(2)

# Coast all to a gradual stop
motors.coast_all()
time.sleep(2)


SWEEPS = 2              # How many sweeps of the motors to perform
STEPS = 10              # The number of discrete sweep steps
STEPS_INTERVAL = 0.5    # The time in seconds between each step of the sequence
SPEED_EXTENT = 1.0      # How far from zero to drive the motors when sweeping

# Do a sine speed sweep
for j in range(SWEEPS):
    for i in range(360):
        speed = math.sin(math.radians(i)) * SPEED_EXTENT
        motors.all_to_speed(speed)
        time.sleep(0.02)

# Do a stepped speed sweep
for j in range(SWEEPS):
    for i in range(0, STEPS):
        motors.all_to_percent(i, 0, STEPS, 0.0 - SPEED_EXTENT, SPEED_EXTENT)
        time.sleep(STEPS_INTERVAL)
    for i in range(0, STEPS):
        motors.all_to_percent(i, STEPS, 0, 0.0 - SPEED_EXTENT, SPEED_EXTENT)
        time.sleep(STEPS_INTERVAL)

# Disable the motors
motors.disable_all()
