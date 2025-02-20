import time
from inventor import Inventor, NUM_MOTORS  # , MOTOR_A, MOTOR_B
# from pimoroni import REVERSED_DIR

"""
Demonstrates how to read the speeds of Inventor 2040/2350 W's two encoders.

Press "User" to exit the program.
"""

# Wheel friendly names
NAMES = ["LEFT", "RIGHT"]

# Constants
GEAR_RATIO = 50                         # The gear ratio of the motor
SPEED = 1.0                             # The speed to drive the motors at
SLEEP = 0.1                             # The time to sleep between each capture

# Create a new Inventor object
board = Inventor(motor_gear_ratio=GEAR_RATIO)

# Uncomment the below lines (and the top imports) to
# reverse the counting direction of an encoder
# encoders[MOTOR_A].direction(REVERSED_DIR)
# encoders[MOTOR_B].direction(REVERSED_DIR)

# Set both motors driving
for motor in board.motors:
    motor.speed(SPEED)

# Variables for storing encoder captures
captures = [None] * NUM_MOTORS

# Read the encoders until the user button is pressed
while not board.switch_pressed():

    # Capture the state of all the encoders since the last capture, SLEEP seconds ago
    for i in range(NUM_MOTORS):
        captures[i] = board.encoders[i].capture()

    # Print out the speeds from each encoder
    for i in range(NUM_MOTORS):
        print(NAMES[i], "=", captures[i].revolutions_per_second, end=", ")
    print()

    time.sleep(SLEEP)
