import plasma
from plasma import plasma_stick
import machine
import time

"""
Reads the internal temperature sensor on the Pico W...
... and changes the LED strip an appropriate colour.
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

sensor_temp = machine.ADC(4)
conversion_factor = 3.3 / (65535)  # used for calculating a temperature from the raw sensor reading

# The Pico's temperature sensor is not super accurate and readings jump around
# lets do some averaging to avoid annoying flashing
n = 0
temperature = 20  # a dummy value to fill the array
temperature_array = [temperature] * 10  # average over 10 readings (5 secs)

while True:
    # read the sensor
    # the following two lines do some maths to convert the number from the temp sensor into celsius
    reading = sensor_temp.read_u16() * conversion_factor
    temperature = 27 - (reading - 0.706) / 0.001721

    # add the most recent reading to the array
    if n >= len(temperature_array):
        n = 0
    temperature_array[n] = temperature
    n += 1

    temperature_average = sum(temperature_array) / len(temperature_array)
    print(f"""
    Average temperature: {temperature_average:.2f} °C
    """)

    # calculates a colour
    hue = HUE_START + ((temperature_average - MIN) * (HUE_END - HUE_START) / (MAX - MIN))

    # set the leds
    for i in range(NUM_LEDS):
        led_strip.set_hsv(i, hue / 360, 1.0, BRIGHTNESS)

    time.sleep(0.5)
