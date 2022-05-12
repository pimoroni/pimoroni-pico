import gc
import time
from encoder import Encoder
# from encoder import REVERSED_DIR

"""
An example of how to read the speed a mechanical rotary encoder is being turned at.
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
    capture = enc.capture()

    print("Count =", capture.count, end=", ")
    print("Angle =", capture.degrees, end=", ")
    print("Freq =", capture.frequency, end=", ")
    print("Speed =", capture.degrees_per_second)

    time.sleep(0.1)
