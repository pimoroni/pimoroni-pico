import time
from machine import Pin
from inventor import Inventor, GPIOS, NUM_GPIOS

"""
Shows how to initialise and read the 6 GPIO headers of Inventor 2040/2350 W.

Press "User" to exit the program.
"""

# Constants
BRIGHTNESS = 0.4      # The brightness of the LEDs
GPIO_NAMES = ("GP0", "GP1", "GP2", "A0", "A1", "A2")
INTERVAL = 0.1

# Create a new Inventor object
board = Inventor()

# Create an input pin object for each GPIO
inputs = [Pin(i, Pin.IN, Pin.PULL_DOWN) for i in GPIOS]


# Wrap the code in a try block, to catch any exceptions (including KeyboardInterrupt)
try:
    # Read the GPIOs until the user button is pressed
    while not board.switch_pressed():

        # Read each GPIO in turn and print its value
        for i in range(NUM_GPIOS):
            value = inputs[i].value()
            print(GPIO_NAMES[i], " = ", inputs[i].value(), sep="", end=", ")

            # Set the neighbouring LED to a colour based on
            # the input, with Green for high and Blue for low
            if value:
                board.leds.set_hsv(i, 0.333, 1.0, BRIGHTNESS)
            else:
                board.leds.set_hsv(i, 0.666, 1.0, BRIGHTNESS)

        # Print a new line
        print()

        time.sleep(INTERVAL)

# Put the board back into a safe state, regardless of how the program may have ended
finally:
    # Turn off the LED bars
    board.leds.clear()
