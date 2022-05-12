import gc
from encoder import Encoder
# from encoder import REVERSED_DIR

"""
An example of how to read a mechanical rotary encoder, only when a change has occurred.
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


# Print out the initial count, step, and turn (they should all be zero)
print("Count =", enc.count(), end=", ")
print("Step =", enc.step(), end=", ")
print("Turn =", enc.turn())

# Loop forever
while True:
    if enc.delta() != 0:
        # Print out the new count, step, and turn
        print("Count =", enc.count(), end=", ")
        print("Step =", enc.step(), end=", ")
        print("Turn =", enc.turn())
