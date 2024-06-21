import time
from tiny_fx import TinyFX
from machine import Pin
from pimoroni import Analog

"""
Reads an analog sensor connected to TinyFX and show it across the outputs.

Press "Boot" to exit the program.
"""

# Constants
MAX_VOLTAGE = 3.3           # The maximum voltage, in volts, to show on the meter
MIN_VOLTAGE = 0             # The minimum voltage, in volts, to show on the meter
TIME_BETWEEN = 0.01         # The time between each voltage measurement

# Variables
fx = TinyFX()               # Create a new TinyFX object
sensor = Analog(Pin(26))    # Create an Analog object to read the sensor

# Wrap the code in a try block, to catch any exceptions (including KeyboardInterrupt)
try:
    while not fx.boot_pressed():
        # Read the voltage from the attached sensor
        voltage = sensor.read_voltage()

        # Print out the voltage
        print("S =", round(voltage, 4))

        # Convert the voltage to a percentage between the maximum and minimum voltage we want to show
        percent = (voltage - MIN_VOLTAGE) / (MAX_VOLTAGE - MIN_VOLTAGE)

        # Scale the percent up by the number of outputs
        out_percent = percent * len(fx.outputs)

        # Update all the outputs to show the voltage level
        for i, out in enumerate(fx.outputs):
            out.brightness(out_percent - i)

        fx.rgb.set_hsv(percent * 0.666, 1, 1)

        time.sleep(TIME_BETWEEN)

finally:
    # Turn off all outputs
    fx.clear()
