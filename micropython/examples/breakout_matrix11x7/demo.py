import time
from breakout_matrix11x7 import BreakoutMatrix11x7

on_brightness = 64

matrix = BreakoutMatrix11x7()

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