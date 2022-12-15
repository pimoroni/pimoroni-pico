'''
raw_set_pixel.py
This example shows how to set the pixels on the display individually without having to use pico graphics.
This method can be used to save on memory usage.
'''

import hub75
import random
import time

HEIGHT = 32
WIDTH = 32
MAX_PIXELS = 64

h75 = hub75.Hub75(WIDTH, HEIGHT, stb_invert=False)


def rand_pixel():
    x = random.randint(0, WIDTH)
    y = random.randint(0, HEIGHT)
    return x, y


def rand_color():
    r = random.randint(0, 255)
    g = random.randint(0, 255)
    b = random.randint(0, 255)
    return r, g, b


h75.start()
counter = 0

while 1:
    x, y = rand_pixel()
    r, g, b = rand_color()
    print('Setting Pixel x: {0} y: {1}'.format(x, y))
    h75.set_pixel(x, y, r, g, b)
    time.sleep(0.2)
    counter += 1
    if counter > MAX_PIXELS:
        counter = 0
        h75.clear()
