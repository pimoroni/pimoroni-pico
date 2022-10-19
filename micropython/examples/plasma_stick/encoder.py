from pimoroni_i2c import PimoroniI2C
from breakout_encoder import BreakoutEncoder
import plasma
from plasma import plasma_stick

"""
Change the colour of your LEDs easily by hooking up an RGB Encoder Breakout!
"""

# set how many LEDs you have
NUM_LEDS = 50

# make this number bigger for more precise colour adjustments
STEPS_PER_REV = 24

i2c = PimoroniI2C(plasma_stick.SDA, plasma_stick.SCL)
enc = BreakoutEncoder(i2c)

enc.set_brightness(1.0)
# enc.set_direction(BreakoutEncoder.DIRECTION_CCW)     # Uncomment this to flip the direction


def hsv_to_rgb(h, s, v):
    # From CPython Lib/colorsys.py
    if s == 0.0:
        return v, v, v
    i = int(h * 6.0)
    f = (h * 6.0) - i
    p = v * (1.0 - s)
    q = v * (1.0 - s * f)
    t = v * (1.0 - s * (1.0 - f))
    i = i % 6
    if i == 0:
        return v, t, p
    if i == 1:
        return q, v, p
    if i == 2:
        return p, v, t
    if i == 3:
        return p, q, v
    if i == 4:
        return t, p, v
    if i == 5:
        return v, p, q


def count_changed(count):
    print("Count: ", count, sep="")
    h = ((count % STEPS_PER_REV) * 360.0) / STEPS_PER_REV     # Convert the count to a colour hue
    r, g, b = [int(255 * c) for c in hsv_to_rgb(h / 360.0, 1.0, 1.0)]  # rainbow magic
    # set the encoder LED colour
    enc.set_led(r, g, b)
    # set the led strip to match
    for i in range(NUM_LEDS):
        led_strip.set_rgb(i, r, g, b)


# WS2812 / NeoPixel™ LEDs
led_strip = plasma.WS2812(NUM_LEDS, 0, 0, plasma_stick.DAT, color_order=plasma.COLOR_ORDER_RGB)

# Start updating the LED strip
led_strip.start()

count = 0

count_changed(count)

enc.clear_interrupt_flag()

while True:
    if enc.get_interrupt_flag():
        count = enc.read()
        enc.clear_interrupt_flag()

        while count < 0:
            count += STEPS_PER_REV

        count_changed(count)
