import time
from pimoroni_i2c import PimoroniI2C
from breakout_mics6814 import BreakoutMICS6814

OUTPUT_FREQUENCY = 0.5

PINS_BREAKOUT_GARDEN = {"sda": 4, "scl": 5}
PINS_PICO_EXPLORER = {"sda": 20, "scl": 21}

i2c = PimoroniI2C(**PINS_BREAKOUT_GARDEN)
gas = BreakoutMICS6814(i2c)

gas.set_brightness(1.0)


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


h = 0

last_output = time.time()

while True:
    oxd = gas.read_oxidising()
    red = gas.read_reducing()
    nh3 = gas.read_nh3()

    r, g, b = [int(255 * c) for c in hsv_to_rgb(h / 360.0, 1.0, 1.0)]  # rainbow magic

    if time.time() - last_output > OUTPUT_FREQUENCY:
        print("OX: {} Red: {} NH3: {}".format(oxd, red, nh3))
        last_output = time.time()

    gas.set_led(r, g, b)

    h += 1

    time.sleep(0.02)
