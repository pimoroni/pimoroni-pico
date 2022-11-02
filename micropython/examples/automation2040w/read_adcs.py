import time
from automation import Automation2040W, SWITCH_A

"""
Shows how to read the 3 ADC terminals of Automation 2040 W.

Press "A" to exit the program.
"""

UPDATES = 10                    # How many times to update per second
ADC_NAMES = ("A1", "A2", "A3")  # The friendly names to give each ADC input

# Create a new Automation2040W
board = Automation2040W()

# Enable the LED of the switch used to exit the loop
board.switch_led(SWITCH_A, 50)  # Half Brightness

# Read the ADCs until the user switch is pressed
while not board.switch_pressed(SWITCH_A):

    # Read each ADC in turn and print its voltage
    for i in range(board.NUM_ADCS):
        voltage = board.read_adc(i)
        print(ADC_NAMES[i], " = ", round(voltage, 3), sep="", end=", ")

    # Print a new line
    print()

    time.sleep(1.0 / UPDATES)

# Put the board back into a safe state
board.reset()
