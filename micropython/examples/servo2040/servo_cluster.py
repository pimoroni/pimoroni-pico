import time
import math
from servo import ServoCluster, servo2040

# NOTE: ServoCluster lets you control up to 30 servos at once. This is
# done using the RP2040's PIO system. As such it is experimental and may
# have edge-cases that will need to be fixed. This is particularly true
# when attempting to run a program multiple times.
# If you do encounter issues, try resetting your board.

# Create a servo cluster for pins 0 to 3, using PIO 0 and State Machine 0
START_PIN = servo2040.SERVO_1
END_PIN = servo2040.SERVO_4
servos = ServoCluster(pio=0, sm=0, pins=list(range(START_PIN, END_PIN + 1)))

# Enable all servos (this puts them at the middle)
servos.enable_all()
time.sleep(2)

# Go to min
servos.all_to_min()
time.sleep(2)

# Go to max
servos.all_to_max()
time.sleep(2)

# Go back to mid
servos.all_to_mid()
time.sleep(2)

SWEEPS = 3              # How many sweeps of the servo to perform
STEPS = 10              # The number of discrete sweep steps
STEPS_INTERVAL = 0.5    # The time in seconds between each step of the sequence
SWEEP_EXTENT = 90.0     # How far from zero to move the servos when sweeping

# Do a sine sweep
for j in range(SWEEPS):
    for i in range(360):
        value = math.sin(math.radians(i)) * SWEEP_EXTENT
        servos.all_to_value(value)
        time.sleep(0.02)

# Do a stepped sweep
for j in range(SWEEPS):
    for i in range(0, STEPS):
        servos.all_to_percent(i, 0, STEPS, 0.0 - SWEEP_EXTENT, SWEEP_EXTENT)
        time.sleep(STEPS_INTERVAL)
    for i in range(0, STEPS):
        servos.all_to_percent(i, STEPS, 0, 0.0 - SWEEP_EXTENT, SWEEP_EXTENT)
        time.sleep(STEPS_INTERVAL)

# Disable the servos
servos.disable_all()
