import time
from automation import Automation2040W, SWITCH_A

# Uncomment for Automation2040W Mini and comment out above import
# from automation import Automation2040WMini, SWITCH_A

"""
Shows how to read the 3 Input terminals of Automation 2040 W.

Press "A" to exit the program.
"""

UPDATES = 10                            # How many times to update per second
INPUT_NAMES = ("I1", "I2", "I3", "I4")  # The friendly names to give each digital input

# Create a new Automation2040W
board = Automation2040W()
# Uncomment for Automation2040W Mini
# board = Automation2040WMini()

# Enable the LED of the switch used to exit the loop
board.switch_led(SWITCH_A, 50)  # Half Brightness

# Read the inputs until the user switch is pressed
while not board.switch_pressed(SWITCH_A):

    # Read each input in turn and print its value
    for i in range(board.NUM_INPUTS):
        value = board.read_input(i)
        print(INPUT_NAMES[i], " = ", value, sep="", end=", ")

    # Print a new line
    print()

    time.sleep(1.0 / UPDATES)

# Put the board back into a safe state
board.reset()
