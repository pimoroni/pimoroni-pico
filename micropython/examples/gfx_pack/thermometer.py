import time
import machine
from gfx_pack import GfxPack
from breakout_bme68x import BreakoutBME68X, STATUS_HEATER_STABLE

"""
GFX temp DEMO

This demo uses a BME680 or BME688 attached to the QWST connector to
measure temperature, pressure, and humidity and display it on the GFX display,
or the internal temperature sensor can be used in place of the
BME68x breakout. Just change use_bme68x_breakout to False
"""

# Settings
lower_temp_bound = 15
upper_temp_bound = 30
use_bme68x_breakout = True

sensor_temp = machine.ADC(4)
conversion_factor = 3.3 / (65535)  # used for calculating a temperature from the raw sensor reading

gp = GfxPack()
gp.set_backlight(0, 0, 0)  # turn the RGB backlight off
display = gp.display
display.set_backlight(0.4)  # set the white to a low value

if use_bme68x_breakout:
    bmp = BreakoutBME68X(gp.i2c)

display.set_pen(0)
display.clear()
display.set_font("bitmap14_outline")

while True:
    # Clear display
    display.set_pen(0)
    display.clear()

    display.set_pen(15)
    display.text("GFXPack Temp demo", 0, 0, scale=0.1)

    if use_bme68x_breakout:
        temperature, pressure, humidity, gas, status, _, _ = bmp.read()
        display.text("Temp: {:0.2f}c".format(temperature), 0, 20, scale=0.2)
        display.text("Press: {:0.2f}Pa".format(pressure), 0, 35, scale=0.2)
        display.text("Humid: {:0.2f}%".format(humidity), 0, 50, scale=0.2)

        heater = "Stable" if status & STATUS_HEATER_STABLE else "Unstable"
        print("{:0.2f}c, {:0.2f}Pa, {:0.2f}%, {:0.2f} Ohms, Heater: {}".format(
            temperature, pressure, humidity, gas, heater))

    else:
        reading = sensor_temp.read_u16() * conversion_factor
        temperature = 27 - (reading - 0.706) / 0.001721
        display.text("Temperature", 25, 15, scale=0.2)
        display.text("{:0.2f}c".format(temperature), 25, 30, scale=2)

    if temperature < lower_temp_bound:
        r = 0
        b = 255
    elif temperature > upper_temp_bound:
        r = 255
        b = 0
    else:
        r = (temperature - lower_temp_bound) / (upper_temp_bound - lower_temp_bound) * 255
        b = 255 - ((temperature - lower_temp_bound) / (upper_temp_bound - lower_temp_bound) * 255)

    gp.set_backlight(r, 0, b)
    display.update()

    time.sleep(0.2)
