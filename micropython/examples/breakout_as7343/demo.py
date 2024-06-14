# Prints data from the AS7343 sensor to the console
# Open up the plotter window in Thonny to see the values change in real-time!

from breakout_as7343 import BreakoutAS7343
from pimoroni_i2c import PimoroniI2C
from pimoroni import BREAKOUT_GARDEN_I2C_PINS  # or PICO_EXPLORER_I2C_PINS or HEADER_I2C_PINS
import time

i2c = PimoroniI2C(**BREAKOUT_GARDEN_I2C_PINS)
as7343 = BreakoutAS7343(i2c)

as7343.set_channels(18)
as7343.set_gain(1024)
as7343.set_measurement_time(33)  # Roughly 30fps at 16ms/measurement
as7343.set_integration_time(27800)

as7343.set_illumination_current(4)
as7343.set_illumination_led(True)

while True:
    reading = as7343.read()

    print(
        "FZ: {0}, FY: {1}, FXL: {2}, NIR: {3}, F2: {4}, F3: {5}, F4: {6}, F6: {7}, F1: {8}, F5: {9}, F7: {10}, F8: {11}".format(
            *reading
        )
    )
    time.sleep(0.01)
