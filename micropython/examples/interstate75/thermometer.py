import time
import machine
from breakout_bme68x import BreakoutBME68X, STATUS_HEATER_STABLE
from interstate75 import Interstate75

"""
Interstate75 Temp DEMO
This demo uses a BME680 or BME688 attached to the QWST connector to
measure temperature, pressure, and humidity and display it on the Interstate75 display.
The internal temperature sensor can be used in place of the BME68x breakout -
just change use_bme68x_breakout to False

This example is designed for use on 2 chained 64x64 panels.
"""

# Settings
lower_temp_bound = 15
upper_temp_bound = 30
use_bme68x_breakout = True

sensor_temp = machine.ADC(4)
conversion_factor = 3.3 / (65535)  # used for calculating a temperature from the raw sensor reading

i75 = Interstate75(display=Interstate75.DISPLAY_INTERSTATE75_128X64)
graphics = i75.display

BLACK = graphics.create_pen(0, 0, 0)
TEMP_COLOUR = graphics.create_pen(255, 255, 255)

if use_bme68x_breakout:
    bmp = BreakoutBME68X(i75.i2c)

graphics.set_pen(BLACK)
graphics.clear()
graphics.set_font("bitmap14_outline")

while True:
    # Clear display
    graphics.set_pen(BLACK)
    graphics.clear()

    graphics.set_pen(TEMP_COLOUR)
    graphics.text("Interstate75 Temp demo", 2, 2, scale=0.1)

    if use_bme68x_breakout:
        temperature, pressure, humidity, gas, status, _, _ = bmp.read()
        graphics.text("Temp: {:0.2f}c".format(temperature), 2, 20, scale=0.2)
        graphics.text("Press: {:0.2f}Pa".format(pressure), 2, 35, scale=0.2)
        graphics.text("Humid: {:0.2f}%".format(humidity), 2, 50, scale=0.2)

        heater = "Stable" if status & STATUS_HEATER_STABLE else "Unstable"
        print("{:0.2f}c, {:0.2f}Pa, {:0.2f}%, {:0.2f} Ohms, Heater: {}".format(
            temperature, pressure, humidity, gas, heater))

    else:
        reading = sensor_temp.read_u16() * conversion_factor
        temperature = 27 - (reading - 0.706) / 0.001721
        graphics.text("Temperature", 25, 15, scale=0.2)
        graphics.text("{:0.2f}c".format(temperature), 25, 30, scale=2)

    if temperature < lower_temp_bound:
        r = 0
        b = 255
    elif temperature > upper_temp_bound:
        r = 255
        b = 0
    else:
        r = int((temperature - lower_temp_bound) / (upper_temp_bound - lower_temp_bound) * 255)
        b = int(255 - ((temperature - lower_temp_bound) / (upper_temp_bound - lower_temp_bound) * 255))

    TEMP_COLOUR = graphics.create_pen(r, 0, b)
    i75.update(graphics)

    time.sleep(0.2)
