import time
from tiny_fx import TinyFX

"""
Play a rainbow animation on TinyFX's RGB output.

Press "Boot" to exit the program.
"""

# Constants
SPEED = 5           # The speed that the LED will cycle at
BRIGHTNESS = 1.0    # The brightness of the LED
UPDATES = 50        # How many times the LED will be updated per second

# Variables
fx = TinyFX()       # Create a new TinyFX object
hue = 0.0           # The hue that will animate over time

# Wrap the code in a try block, to catch any exceptions (including KeyboardInterrupt)
try:
    while not fx.boot_pressed():
        # Update the LED
        fx.rgb.set_hsv(hue, 1.0, BRIGHTNESS)

        hue += SPEED / 1000.0
        if hue >= 1.0:
            hue -= 1.0

        time.sleep(1.0 / UPDATES)

finally:
    # Turn off all outputs
    fx.clear()
