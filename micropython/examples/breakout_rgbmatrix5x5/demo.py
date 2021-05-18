import time
from pimoroni_i2c import PimoroniI2C
from breakout_rgbmatrix5x5 import BreakoutRGBMatrix5x5

PINS_BREAKOUT_GARDEN = {"sda": 4, "scl": 5}
PINS_PICO_EXPLORER = {"sda": 20, "scl": 21}

i2c = PimoroniI2C(**PINS_BREAKOUT_GARDEN)
matrix = BreakoutRGBMatrix5x5(i2c)

colors = []
colors.append((255, 0, 0))
colors.append((0, 255, 0))
colors.append((0, 0, 255))
colors.append((128, 128, 128))

x = 0
y = 0
col = 0

while True:
    matrix.set_pixel(x, y, colors[col][0], colors[col][1], colors[col][2])
    matrix.update()

    x += 1
    if x >= matrix.WIDTH:
        x = 0
        y += 1
        if y >= matrix.HEIGHT:
            y = 0
            col += 1
            if col >= len(colors):
                col = 0
            time.sleep(0.5)

    time.sleep(0.01)
