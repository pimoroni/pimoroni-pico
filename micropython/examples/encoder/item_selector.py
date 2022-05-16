import gc
from encoder import Encoder
# from encoder import REVERSED_DIR

"""
A demonstration of a rotary encoder being used to
select items based on its physical position.

This requires that the encoder is positioned in the same
direction at the start of every program run (e.g. upwards).
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


# A list of items, up to the encoder's counts_per_rev
ITEMS = ["Red", "Orange", "Yellow", "Green", "Blue", "Indigo", "Violet", "Black", "White"]

last_step = -1

# Loop forever
while True:
    step = enc.step()
    if step != last_step:
        if step < len(ITEMS):
            print(step, "/", int(enc.counts_per_rev()) - 1, ": ", ITEMS[step], sep="")
        else:
            print(step, "/", int(enc.counts_per_rev()) - 1, ": ", "Undefined", sep="")
        last_step = step
