import gc
import time
from pimoroni import Button
from encoder import Encoder, MMME_CPR
# from encoder import REVERSED
from motor import motor2040

"""
Demonstrates how to read the counts of multiple motor encoders.

Press "Boot" to exit the program.
"""

# Free up hardware resources ahead of creating a new Encoder
gc.collect()

# The gear ratio of the motor the encoder is attached to.
GEAR_RATIO = 50
CPR = MMME_CPR * GEAR_RATIO


# Create a list of motors
ENCODER_PINS = [ motor2040.ENCODER_A, motor2040.ENCODER_B, motor2040.ENCODER_C, motor2040.ENCODER_D ]
ENCODER_NAMES = [ "A", "B", "C", "D" ]
NUM_ENCODERS = len(ENCODER_PINS)
encoders = [Encoder(0, i, ENCODER_PINS[i], counts_per_rev=CPR, count_microsteps=True) for i in range(NUM_ENCODERS)]

# Uncomment the below line (and the top import) to reverse the counting direction
# encoders[0].direction(REVERSED)
# encoders[1].direction(REVERSED)
# encoders[2].direction(REVERSED)
# encoders[3].direction(REVERSED)

# Create the user button
user_sw = Button(motor2040.USER_SW)

captures = [None] * NUM_ENCODERS

# Read the encoders until the user button is pressed
while user_sw.raw() is not True:
    # Capture the state of all encoders, at as close to the same moment as possible
    for i in range(NUM_ENCODERS):
        captures[i] = encoders[i].take_snapshot()
    
    # Print out the angle and speed of each encoder
    for i in range(NUM_ENCODERS):
        print(ENCODER_NAMES[i], "=", captures[i].degrees(), "|", captures[i].degrees_per_second(), end=", ")
    
    print()        
    time.sleep(0.1)
