"""BME688 / BME680 demo

This demo will work for both the BME680 and BME688.
"""
import time
from picographics import PicoGraphics, DISPLAY_ENVIRO_PLUS
from pimoroni import RGBLED
from breakout_bme68x import BreakoutBME68X, STATUS_HEATER_STABLE
from pimoroni_i2c import PimoroniI2C

PINS_BREAKOUT_GARDEN = {"sda": 4, "scl": 5}
PINS_PICO_EXPLORER = {"sda": 20, "scl": 21}

i2c = PimoroniI2C(**PINS_BREAKOUT_GARDEN)
bmp = BreakoutBME68X(i2c, address=0x77)


display = PicoGraphics(display=DISPLAY_ENVIRO_PLUS)
display.set_backlight(1.0)

led = RGBLED(6, 7, 10, invert=True)  # setting pins for the RGB led
led.set_rgb(0, 0, 0)


# setup background
BG = display.create_pen(0, 0, 0)
TEXT = display.create_pen(255, 255, 255)
TEMP = display.create_pen(255, 0, 0)
PRESS = display.create_pen(255, 255, 0)
HUMID = display.create_pen(0, 255, 0)

display.set_pen(BG)
display.clear()


def draw_graph(temp_value, press_value, humid_value):
    scaled_temp = int(temp_value + 40)  # Range from -40 to +60 = 1-100
    scaled_press = int((press_value - 87000) / ((108400 - 87000) / 100))  # Range 108400 - 87000 = 1 -100
    scaled_humid = int(humid_value)  # Range 1 - 100
    display.set_pen(BG)
    display.clear()
    display.set_pen(TEMP)
    display.rectangle(0, 60, scaled_temp, 60)
    display.text("TEMP: {:0.2f}c".format(temp_value), scaled_temp + 5, 80, scale=2)
    display.set_pen(PRESS)
    display.text("PRESS: {:0.2f}Pa".format(press_value), scaled_press + 5, 140, scale=2)
    display.rectangle(0, 120, scaled_press, 60)
    display.set_pen(HUMID)
    display.text("HUMID: {:0.2f}%".format(humid_value), scaled_humid + 5, 200, scale=2)
    display.rectangle(0, 180, scaled_humid, 60)
    display.set_pen(TEXT)
    display.text("BME688 enviro Sensor", 5, 10, scale=2)
    display.update()


while True:
    temperature, pressure, humidity, gas, status, _, _ = bmp.read()
    draw_graph(temperature, pressure, humidity)
    heater = "Stable" if status & STATUS_HEATER_STABLE else "Unstable"
    print("{:0.2f}c, {:0.2f}Pa, {:0.2f}%, {:0.2f} Ohms, Heater: {}".format(
        temperature, pressure, humidity, gas, heater))
    time.sleep(1.0)
