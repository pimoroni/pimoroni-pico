import plasma
from plasma import plasma_stick
import time
from breakout_bme280 import BreakoutBME280
from pimoroni_i2c import PimoroniI2C

"""
Reads the temperature from a BME280 breakout...
...and changes the LED strip an appropriate colour.
https://shop.pimoroni.com/products/bme280-breakout
"""

# Set how many LEDs you have
NUM_LEDS = 50

BRIGHTNESS = 1.0

# The range of readings that we want to map to colours
MIN = 10
MAX = 30

# pick what bits of the colour wheel to use (from 0-360°)
# https://www.cssscript.com/demo/hsv-hsl-color-wheel-picker-reinvented/
HUE_START = 230  # blue
HUE_END = 359  # red

# WS2812 / NeoPixel™ LEDs
led_strip = plasma.WS2812(NUM_LEDS, 0, 0, plasma_stick.DAT, color_order=plasma.COLOR_ORDER_RGB)

# Start updating the LED strip
led_strip.start()

# set up I2C
i2c = PimoroniI2C(plasma_stick.SDA, plasma_stick.SCL)

# set up BME280 breakout
bme = BreakoutBME280(i2c)

while True:
    temperature, pressure, humidity = bme.read()
    print(f"""
Temperature: {temperature:.2f} °C
Humidity: {humidity:.2f} %
Pressure: {pressure / 100:.2f} hPa
     """)

    # calculates a colour
    hue = HUE_START + ((temperature - MIN) * (HUE_END - HUE_START) / (MAX - MIN))

    # set the leds
    for i in range(NUM_LEDS):
        led_strip.set_hsv(i, hue / 360, 1.0, BRIGHTNESS)

    time.sleep(0.5)
