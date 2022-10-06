import time
from automation import Automation2040W, SWITCH_A, SWITCH_B

"""
An example of the user switches and LEDs on Automation 2040 W.

Press "A" and "B" together to exit the program.
"""

UPDATES = 10                     # How many times to update per second
SWITCH_NAMES = ("SW_A", "SW_B")  # The friendly names to give each user switch
LED_INCREMENT = 2                # How much to change a LED's brightess each update

# Create a new Automation2040W
board = Automation2040W()

# Enable the Conn LED
board.conn_led(True)  # Full Brightness

led_brightnesses = [0.0, 0.0]

# Interact with the switches and LEDs until both are pressed simultaneously
while not board.switch_pressed(SWITCH_A) or not board.switch_pressed(SWITCH_B):

    for i in range(board.NUM_SWITCHES):
        # Change the LED brightness based on switch's state
        if board.switch_pressed(i):
            print(SWITCH_NAMES[i], " = Pressed", sep="", end=", ")
            led_brightnesses[i] = min(led_brightnesses[i] + LED_INCREMENT, 100)
        else:
            print(SWITCH_NAMES[i], " = Released", sep="", end=", ")
            led_brightnesses[i] = max(led_brightnesses[i] - LED_INCREMENT, 0)

        # Apply the new brightnesses to the LEDs
        board.switch_led(i, led_brightnesses[i])

    # Print a new line
    print()

    time.sleep(1.0 / UPDATES)

# Put the board back into a safe state
board.reset()
