import plasma
import time

NUM_LEDS = 30

# WS2812 / NeoPixel™ LEDs
led_strip = plasma.WS2812(NUM_LEDS, 0, 0, 15)
# APA102 / DotStar™ LEDs
# led_strip = plasma.APA102(NUM_LEDS, 0, 0, 15, 14)

# Start updating the LED strip
led_strip.start()

# Make rainbows
while True:
    t = time.ticks_ms() / 1000.0 / 5.0
    for i in range(NUM_LEDS):
        led_strip.set_hsv(i, t + (i / NUM_LEDS))
    time.sleep(1.0 / 60)
