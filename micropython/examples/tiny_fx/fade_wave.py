import time
import math
from tiny_fx import TinyFX

"""
Play a wave animation on TinyFX's outputs, with fading.

Press "Boot" to exit the program.
"""

# Constants
SLEEP = 0.01     # The time to sleep between each update
SPEED = 0.005    # How much to advance the offset by each update

# Variables
fx = TinyFX()    # Create a new TinyFX object
offset = 0       # The offset used to animate the wave

# Wrap the code in a try block, to catch any exceptions (including KeyboardInterrupt)
try:
    while not fx.boot_pressed():
        for i, out in enumerate(fx.outputs):
            # Calculate a brightness for each output based on its position and the offset
            phase = (i / len(fx.outputs)) - offset
            out.brightness((math.sin(phase * math.pi * 2) + 1) / 2)

        # Advance the offset, wrapping if it exceeds 1.0
        offset += SPEED
        if offset >= 1.0:
            offset -= 1.0

        time.sleep(SLEEP)

finally:
    # Turn off all outputs
    fx.clear()
