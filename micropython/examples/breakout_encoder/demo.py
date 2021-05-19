from pimoroni_i2c import PimoroniI2C
from breakout_encoder import BreakoutEncoder

PINS_BREAKOUT_GARDEN = {"sda": 4, "scl": 5}
PINS_PICO_EXPLORER = {"sda": 20, "scl": 21}

steps_per_rev = 24

i2c = PimoroniI2C(**PINS_BREAKOUT_GARDEN)
enc = BreakoutEncoder(i2c)

enc.set_brightness(1.0)
# enc.set_direction(BreakoutEncoder.DIRECTION_CCW)     # Uncomment this to flip the direction


# From CPython Lib/colorsys.py
def hsv_to_rgb(h, s, v):
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
    h = ((count % steps_per_rev) * 360.0) / steps_per_rev     # Convert the count to a colour hue
    r, g, b = [int(255 * c) for c in hsv_to_rgb(h / 360.0, 1.0, 1.0)]  # rainbow magic
    enc.set_led(r, g, b)


count = 0

count_changed(count)
enc.clear_interrupt_flag()

while True:
    if enc.get_interrupt_flag():
        count = enc.read()
        enc.clear_interrupt_flag()

        while count < 0:
            count += steps_per_rev

        count_changed(count)
