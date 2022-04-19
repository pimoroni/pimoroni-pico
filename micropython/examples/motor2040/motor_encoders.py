import gc
import time
from pimoroni import Button
from encoder import Encoder
# from encoder import REVERSED
from motor import motor2040

"""
Demonstrates how to read the counts of multiple motor encoders.

Press "Boot" to exit the program.
"""

# Free up hardware resources ahead of creating a new Encoder
gc.collect()

# Create an encoder on the 3 ADC pins, using PIO 0 and State Machine 0
enc_a = Encoder(0, 0, motor2040.ENCODER_A, count_microsteps=True)
enc_b = Encoder(0, 1, motor2040.ENCODER_B, count_microsteps=True)
enc_c = Encoder(0, 2, motor2040.ENCODER_C, count_microsteps=True)
enc_d = Encoder(0, 3, motor2040.ENCODER_D, count_microsteps=True)

# Uncomment the below line (and the top import) to reverse the counting direction
# enc_a.direction(REVERSED)
# enc_b.direction(REVERSED)
# enc_c.direction(REVERSED)
# enc_d.direction(REVERSED)

# Create the user button
user_sw = Button(motor2040.USER_SW)

# Read the encoders until the user button is pressed
while user_sw.raw() is not True:
    print("A =", enc_a.count(), end=", ")
    print("B =", enc_b.count(), end=", ")
    print("C =", enc_c.count(), end=", ")
    print("D =", enc_d.count())
        
    time.sleep(0.1)

