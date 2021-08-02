"""BME688 / BME680 demo

This demo will work for both the BME680 and BME688.
"""
import time
from breakout_bme68x import BreakoutBME68X, STATUS_HEATER_STABLE
from pimoroni_i2c import PimoroniI2C

PINS_BREAKOUT_GARDEN = {"sda": 4, "scl": 5}
PINS_PICO_EXPLORER = {"sda": 20, "scl": 21}

i2c = PimoroniI2C(**PINS_PICO_EXPLORER)
bmp = BreakoutBME68X(i2c)

while True:
    temperature, pressure, humidity, gas, status, _, _ = bmp.read()
    heater = "Stable" if status & STATUS_HEATER_STABLE else "Unstable"
    print("{:0.2f}c, {:0.2f}Pa, {:0.2f}%, {:0.2f} Ohms, Heater: {}".format(
        temperature, pressure, humidity, gas, heater))
    time.sleep(1.0)
