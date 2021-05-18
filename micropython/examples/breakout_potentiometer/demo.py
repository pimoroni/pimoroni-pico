import time
from pimoroni_i2c import PimoroniI2C
from breakout_potentiometer import BreakoutPotentiometer

PINS_BREAKOUT_GARDEN = {"sda": 4, "scl": 5}
PINS_PICO_EXPLORER = {"sda": 20, "scl": 21}

i2c = PimoroniI2C(**PINS_BREAKOUT_GARDEN)
pot = BreakoutPotentiometer(i2c)

pot.set_brightness(1.0)
# pot.set_direction(BreakoutPotentiometer.DIRECTION_CCW)     # Uncomment this to flip the direction


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


val = 0

while True:
    val = pot.read()
    h = val * 240.0     # Colour range from red to blue

    r, g, b = [int(255 * c) for c in hsv_to_rgb(h / 360.0, 1.0, 1.0)]  # rainbow magic

    print("Percent: ", int(val * 100), "%", sep="")
    pot.set_led(r, g, b)

    time.sleep(0.02)
