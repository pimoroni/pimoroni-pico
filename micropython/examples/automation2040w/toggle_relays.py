import time
from automation import Automation2040W, SWITCH_A

# Uncomment for Automation2040W Mini and comment out above import
# from automation import Automation2040WMini, SWITCH_A

"""
Demonstrates how to toggle the actuation state of each of Automation 2040 W's relays.

Press "A" to exit the program.
"""

TIME_PER_TOGGLE = 0.5               # How much time to wait between each toggle (in seconds)
RELAY_NAMES = ("R1", "R2", "R3")    # The friendly names to give each relay

# Create a new Automation2040W
board = Automation2040W()
# Uncomment for Automation2040W Mini
# board = Automation2040WMini()

# Enable the LED of the switch used to exit the loop
board.switch_led(SWITCH_A, 50)  # Half Brightness

toggle = True
index = 0

# Toggle the relays until the user switch is pressed
while not board.switch_pressed(SWITCH_A):

    # Toggle a relay
    if board.NUM_RELAYS == 1:
        board.relay(toggle)

        # Print the state of the relay
        print(RELAY_NAMES[0], " = ", board.relay(), sep="", end=", ")
    else:
        board.relay(index, toggle)

        # Print the state of all relays
        for i in range(board.NUM_RELAYS):
            print(RELAY_NAMES[i], " = ", board.relay(i), sep="", end=", ")

    # Print a new line
    print()

    index += 1                  # Move on to the next relay
    if index >= board.NUM_RELAYS:
        index = 0               # Go back to the first relay
        toggle = not toggle     # Invert the toggle value

    time.sleep(TIME_PER_TOGGLE)

# Put the board back into a safe state
board.reset()
