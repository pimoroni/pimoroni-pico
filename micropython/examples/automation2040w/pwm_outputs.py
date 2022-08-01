import time
from automation import Automation2040, SWITCH_A, NUM_OUTPUTS

"""
Demonstrates how to toggle each of Automation 2040's output terminals.

Press "A" to exit the program.
"""

TIME_PER_TOGGLE = 0.5               # How much time to wait between each toggle (in seconds)
OUTPUT_NAMES = ("O1", "O2", "O3")   # The friendly names to give each digital output

# Create a new Automation2040
board = Automation2040()

# Enable the LED of the switch used to exit the loop
board.switch_led(SWITCH_A, 50)  # Half Brightness

toggle = True
index = 0

# Toggle the outputs until the user switch is pressed
while not board.switch_pressed(SWITCH_A):

    # Toggle an output
    for output_percentage in range(101):

        if toggle is True:
            board.output(index, float(output_percentage))
        else:
            board.output(index, 100.0 - float(output_percentage))

        # Print the state of all outputs
        for i in range(NUM_OUTPUTS):
            print(OUTPUT_NAMES[i], " = ", board.output_percent(i), sep="", end=", ")

        # Print a new line
        print()
        time.sleep(0.01)

    index += 1                  # Move on to the next output
    if index >= NUM_OUTPUTS:
        index = 0               # Go back to the first output
        toggle = not toggle     # Invert the toggle value

    time.sleep(TIME_PER_TOGGLE)

# Put the board back into a safe state
board.reset()
