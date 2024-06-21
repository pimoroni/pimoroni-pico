import time
from tiny_fx import TinyFX

"""
Play a wave animation on TinyFX's outputs.

Press "Boot" to exit the program.
"""

# Constants
SLEEP = 0.1     # The time to sleep between each change

# Variables
fx = TinyFX()   # Create a new TinyFX object

# Wrap the code in a try block, to catch any exceptions (including KeyboardInterrupt)
try:
    while not fx.boot_pressed():
        for out in fx.outputs:
            out.on()
            time.sleep(SLEEP)

        for out in fx.outputs:
            out.off()
            time.sleep(SLEEP)

finally:
    # Turn off all outputs
    fx.clear()
