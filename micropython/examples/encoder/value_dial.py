import gc
from encoder import Encoder
# from encoder import REVERSED_DIR

"""
A demonstration of a rotary encoder being used to control a value.
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

# The min and max value
MIN_VALUE = 0
MAX_VALUE = 100

value = 50

# Print out the initial value
print("Value =", value)

# Loop forever
while True:
    delta = enc.delta()
    if delta != 0:
        if delta > 0:
            value = min(value + 1, MAX_VALUE)
        else:
            value = max(value - 1, MIN_VALUE)

        # Print out the new value
        print("Value =", value)
