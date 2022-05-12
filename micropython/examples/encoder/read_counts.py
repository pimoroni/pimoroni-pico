import gc
import time
from encoder import Encoder
# from encoder import REVERSED_DIR

"""
An example of how to read a mechanical rotary encoder.
"""

# Free up hardware resources ahead of creating a new Encoder
gc.collect()

# Create an encoder on the 3 ADC pins, using PIO 0 and State Machine 0
PIN_A = 26    # The A channel pin
PIN_B = 28    # The B channel pin
PIN_C = 27    # The common pin
enc = Encoder(0, 0, (PIN_A, PIN_B), PIN_C)

# Uncomment the below line (and the top import) to reverse the counting direction
# enc.direction(REVERSED_DIR)


# Loop forever
while True:

    # Print out the count, delta, step, and turn
    print("Count =", enc.count(), end=", ")
    print("Delta =", enc.delta(), end=", ")
    print("Step =", enc.step(), end=", ")
    print("Turn =", enc.turn())

    time.sleep(0.1)
