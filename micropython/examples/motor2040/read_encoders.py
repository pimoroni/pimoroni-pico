import gc
import time
from motor import motor2040
from encoder import Encoder, MMME_CPR
from pimoroni import Button  # , REVERSED_DIR

"""
Demonstrates how to read the angles of Motor 2040's four encoders.

Press "Boot" to exit the program.
"""

GEAR_RATIO = 50                         # The gear ratio of the motor
COUNTS_PER_REV = MMME_CPR * GEAR_RATIO  # The counts per revolution of the motor's output shaft


# Free up hardware resources ahead of creating a new Encoder
gc.collect()

# Create a list of encoders
ENCODER_PINS = [motor2040.ENCODER_A, motor2040.ENCODER_B, motor2040.ENCODER_C, motor2040.ENCODER_D]
ENCODER_NAMES = ["A", "B", "C", "D"]
NUM_ENCODERS = len(ENCODER_PINS)
encoders = [Encoder(0, i, ENCODER_PINS[i], counts_per_rev=COUNTS_PER_REV, count_microsteps=True) for i in range(NUM_ENCODERS)]

# Uncomment the below lines (and the top import) to
# reverse the counting direction of an encoder
# encoders[0].direction(REVERSED_DIR)
# encoders[1].direction(REVERSED_DIR)
# encoders[2].direction(REVERSED_DIR)
# encoders[3].direction(REVERSED_DIR)

# Create the user button
user_sw = Button(motor2040.USER_SW)

# Read the encoders until the user button is pressed
while not user_sw.raw():

    # Print out the angle of each encoder
    for i in range(NUM_ENCODERS):
        print(ENCODER_NAMES[i], "=", encoders[i].degrees(), end=", ")
    print()

    time.sleep(0.1)
