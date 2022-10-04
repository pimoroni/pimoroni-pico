import plasma
from plasma import plasma2040
from pimoroni import RGBLED
from pimoroni_i2c import PimoroniI2C
import machine
import time

"""
Reads the internal temperature sensor on the Pico and changes the LED strip an appropriate colour.
"""

# Set how many LEDs you have
NUM_LEDS = 50

BRIGHTNESS = 1.0

MIN = 15
MAX = 30

# What you want your MIN colour to be - a hue between 0 and 360 degrees.
# Green is 120!
START_HUE = 120

# WS2812 / NeoPixel™ LEDs
led_strip = plasma.WS2812(NUM_LEDS, 0, 0, plasma2040.DAT, rgbw=False)

# Start updating the LED strip
led_strip.start()

sensor_temp = machine.ADC(4)
conversion_factor = 3.3 / (65535)  # used for calculating a temperature from the raw sensor reading

while True:
        
    # the following two lines do some maths to convert the number from the temp sensor into celsius
    reading = sensor_temp.read_u16() * conversion_factor
    temperature = 27 - (reading - 0.706) / 0.001721
    print(f"""
    Temperature: {temperature:0.2f} * C
    """)
    
    # calculates a colour
    hue = max(0, START_HUE / 360 * (1 - (temperature - MIN) / MAX))
    for i in range(NUM_LEDS):
        led_strip.set_hsv(i, hue, 1.0, BRIGHTNESS)
    
    time.sleep(0.5)
