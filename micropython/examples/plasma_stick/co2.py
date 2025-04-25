import plasma
from plasma import plasma_stick
import breakout_scd41 as scd
from pimoroni_i2c import PimoroniI2C

"""
Reads CO2 level from an SCD41 breakout...
... and changes the LED strip an appropriate colour.
https://shop.pimoroni.com/products/scd41-co2-sensor-breakout
"""

# set how many LEDs you have
NUM_LEDS = 50

BRIGHTNESS = 1.0

# the range of readings to map to colours
# https://www.kane.co.uk/knowledge-centre/what-are-safe-levels-of-co-and-co2-in-rooms
MIN = 400
MAX = 2000

# pick what bits of the colour wheel to use (from 0-360°)
# https://www.cssscript.com/demo/hsv-hsl-color-wheel-picker-reinvented/
HUE_START = 100  # green
HUE_END = 0  # red

# WS2812 / NeoPixel™ LEDs
led_strip = plasma.WS2812(NUM_LEDS, 0, 0, plasma_stick.DAT, color_order=plasma.COLOR_ORDER_RGB)

# Start updating the LED strip
led_strip.start()

# set up I2C
i2c = PimoroniI2C(plasma_stick.SDA, plasma_stick.SCL)

# set up SCD41 breakout
scd.init(i2c)
scd.start()

print("Waiting for SCD41 to be ready")

while True:
    if scd.ready():
        co2, temperature, humidity = scd.measure()
        print(f"""
CO2:         {co2} ppm
Temperature: {temperature:.2f} °C
Humidity:    {humidity:.2f} %""")

        # calculates a colour
        hue = max(0, HUE_START + ((co2 - MIN) * (HUE_END - HUE_START) / (MAX - MIN)))

        # set the leds
        for i in range(NUM_LEDS):
            led_strip.set_hsv(i, hue / 360, 1.0, BRIGHTNESS)
