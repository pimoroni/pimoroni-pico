import time
from tiny_fx import TinyFX

"""
Shows the voltage level of TinyFX on its outputs.

Press "Boot" to exit the program.
"""

# Constants
MAX_VOLTAGE = 6       # The maximum voltage, in volts, to show on the meter
MIN_VOLTAGE = 4       # The minimum voltage, in volts, to show on the meter
SAMPLES = 50          # The number of voltage measurements to take per reading
TIME_BETWEEN = 0.01   # The time between each voltage measurement

# Variables
fx = TinyFX()    # Create a new TinyFX object

# Wrap the code in a try block, to catch any exceptions (including KeyboardInterrupt)
try:
    while not fx.boot_pressed():
        # Read the average voltage across a set of samples
        voltage = fx.voltage(SAMPLES)

        # Print out the voltage
        print("Voltage =", round(voltage, 4))

        # Convert the voltage to a percentage between the maximum and minimum voltage we want to show
        percent = (voltage - MIN_VOLTAGE) / (MAX_VOLTAGE - MIN_VOLTAGE)

        # Update all the outputs
        for i, out in enumerate(fx.outputs):
            # Calculate the voltage level the LED represents
            level = (i + 0.5) / len(fx.outputs)

            # If the percent is above the level, light the output, otherwise turn it off
            if percent >= level:
                out.on()
            else:
                out.off()
        time.sleep(TIME_BETWEEN)

finally:
    # Turn off all outputs
    fx.clear()
