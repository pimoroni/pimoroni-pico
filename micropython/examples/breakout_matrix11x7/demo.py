import time
from pimoroni_i2c import PimoroniI2C
from breakout_matrix11x7 import BreakoutMatrix11x7

on_brightness = 64

PINS_BREAKOUT_GARDEN = {"sda": 4, "scl": 5}
PINS_PICO_EXPLORER = {"sda": 20, "scl": 21}

i2c = PimoroniI2C(**PINS_BREAKOUT_GARDEN)
matrix = BreakoutMatrix11x7(i2c)

x = 0
y = 0
light = True

while True:
    if light:
        matrix.set_pixel(x, y, on_brightness)
    else:
        matrix.set_pixel(x, y, 0)
    matrix.update()

    x += 1
    if x >= matrix.WIDTH:
        x = 0
        y += 1
        if y >= matrix.HEIGHT:
            y = 0
            light = not light
            time.sleep(0.5)

    time.sleep(0.01)
