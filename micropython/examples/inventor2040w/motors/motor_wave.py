import time
import math
from inventor import Inventor2040W, NUM_LEDS, NUM_MOTORS

"""
An example of applying a wave pattern to Inventor 2040 W's motors and LEDs.

Press "User" to exit the program.
"""

# Constants
SPEED = 5             # The speed that the LEDs and motors will cycle at
BRIGHTNESS = 0.4      # The brightness of the LEDs
UPDATES = 50          # How many times to update LEDs and motors per second
SPEED_EXTENT = 1.0    # How far from zero to drive the motors
HALF_LEDS = NUM_LEDS / 2

# Create a new Inventor2040W
board = Inventor2040W()

offset = 0.0

# Make waves until the user button is pressed
while not board.switch_pressed():

    offset += SPEED / 1000.0

    # Update the LED bars
    for i in range(HALF_LEDS):
        hue = i / (NUM_LEDS * 4)
        board.leds.set_hsv(i, hue + offset, 1.0, BRIGHTNESS)
        board.leds.set_hsv(NUM_LEDS - i - 1, hue + offset, 1.0, BRIGHTNESS)

    # Update both motors
    for i in range(NUM_MOTORS):
        angle = (i + offset) * math.pi
        board.motors[i].speed(math.sin(angle) * SPEED_EXTENT)

    time.sleep(1.0 / UPDATES)

# Stop both motors
for m in board.motors:
    m.disable()

# Turn off the LEDs
board.leds.clear()
