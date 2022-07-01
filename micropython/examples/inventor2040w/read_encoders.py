import time
from inventor import Inventor2040W, NUM_MOTORS  # , MOTOR_A, MOTOR_B
# from pimoroni import REVERSED_DIR

"""
Demonstrates how to read the angles of Inventor 2040 W's two encoders.

Press "User" to exit the program.
"""

# Wheel friendly names
NAMES = ["LEFT", "RIGHT"]

# Constants
GEAR_RATIO = 50                         # The gear ratio of the motor

# Create a new Inventor2040W
board = Inventor2040W(motor_gear_ratio=GEAR_RATIO)

# Uncomment the below lines (and the top imports) to
# reverse the counting direction of an encoder
# encoders[MOTOR_A].direction(REVERSED_DIR)
# encoders[MOTOR_B].direction(REVERSED_DIR)


# Read the encoders until the user button is pressed
while not board.switch_pressed():

    # Print out the angle of each encoder
    for i in range(NUM_MOTORS):
        print(NAMES[i], "=", board.encoders[i].degrees(), end=", ")
    print()

    time.sleep(0.1)
