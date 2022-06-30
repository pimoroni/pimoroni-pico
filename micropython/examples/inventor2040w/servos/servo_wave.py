import time
import math
from inventor import Inventor2040W, NUM_SERVOS, LED_SERVO_1

"""
An example of applying a wave pattern to a group of servos and the LEDs.

Press "User" to exit the program.
"""

SPEED = 5             # The speed that the LEDs will cycle at
BRIGHTNESS = 0.4      # The brightness of the LEDs
UPDATES = 50          # How many times to update LEDs and Servos per second
SERVO_EXTENT = 80.0   # How far from zero to move the servos

# Create a new Inventor2040W
board = Inventor2040W()

offset = 0.0

# Make waves until the user button is pressed
while not board.switch_pressed():

    offset += SPEED / 1000.0

    # Update all the Servos
    for i in range(NUM_SERVOS):
        angle = ((i / NUM_SERVOS) + offset) * math.pi
        board.servos[i].value(math.sin(angle) * SERVO_EXTENT)

        # Read back the servo's angle and use that to set a hue on the neighbouring LED
        hue = ((board.servos[i].value() / SERVO_EXTENT) + 1) * 0.333
        board.leds.set_hsv(i + LED_SERVO_1, hue, 1.0, BRIGHTNESS)

    time.sleep(1.0 / UPDATES)

# Stop all the servos
for s in board.servos:
    s.disable()

# Turn off the LEDs
board.leds.clear()
