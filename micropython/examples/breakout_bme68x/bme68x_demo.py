"""BME688 / BME680 demo

This demo will work for both the BME680 and BME688.
"""
import time
from breakout_bme68x import BreakoutBME68X
from pimoroni_i2c import PimoroniI2C

PINS_BREAKOUT_GARDEN = {"sda": 4, "scl": 5}
PINS_PICO_EXPLORER = {"sda": 20, "scl": 21}

i2c = PimoroniI2C(**PINS_PICO_EXPLORER)
bmp = BreakoutBME68X(i2c, address=0x77)

while True:
    reading = bmp.read()
    print(reading)
    time.sleep(1.0)
