import plasma
import time

# Import helper for Analog
from pimoroni import Analog

NUM_LEDS = 30
UPDATES = 60

# WS2812 / NeoPixel™ LEDs
led_strip = plasma.WS2812(NUM_LEDS, 0, 0, 15)
# APA102 / DotStar™ LEDs
# led_strip = plasma.APA102(NUM_LEDS, 0, 0, 15, 14)

# Set up the ADC for reading current
sense = Analog(plasma.PIN_CURRENT_SENSE, plasma.ADC_GAIN, plasma.SHUNT_RESISTOR)

# Start updating the LED strip
led_strip.start()

count = 0
# Make rainbows
while True:
    t = time.ticks_ms() / 1000.0 / 5.0
    for i in range(NUM_LEDS):
        led_strip.set_hsv(i, t + (i / NUM_LEDS))

    count += 1
    if count >= UPDATES:
        # Display the current value once every second
        print("Current =", sense.read_current(), "A")
        count = 0

    time.sleep(1.0 / UPDATES)
