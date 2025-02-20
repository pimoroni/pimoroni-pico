import time
from inventor import Inventor, NUM_LEDS

"""
Displays a rotating rainbow pattern on Inventor 2040/2350 W's onboard LED bars.

Press "User" to exit the program.
"""

# Constants
SPEED = 5           # The speed that the LEDs will cycle at
BRIGHTNESS = 0.4    # The brightness of the LEDs
UPDATES = 50        # How many times the LEDs will be updated per second

# Create a new Inventor object
board = Inventor()

# Variables
offset = 0.0

# Wrap the code in a try block, to catch any exceptions (including KeyboardInterrupt)
try:
    # Make rainbows until the user button is pressed
    while not board.switch_pressed():

        offset += SPEED / 1000.0

        # Update all the LEDs
        for i in range(NUM_LEDS):
            hue = float(i) / NUM_LEDS
            board.leds.set_hsv(i, hue + offset, 1.0, BRIGHTNESS)

        time.sleep(1.0 / UPDATES)

# Put the board back into a safe state, regardless of how the program may have ended
finally:
    # Turn off the LED bars
    board.leds.clear()
