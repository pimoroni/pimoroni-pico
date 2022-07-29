import time
import random
from galactic import GalacticUnicorn

gu = GalacticUnicorn()

colour = (230, 150, 0)


@micropython.native  # noqa: F821
def setup():
    global width, height, lifetime, age
    width = GalacticUnicorn.WIDTH
    height = GalacticUnicorn.HEIGHT
    lifetime = [[0.0 for y in range(height)] for x in range(width)]
    age = [[0.0 for y in range(height)] for x in range(width)]
    for y in range(height):
        for x in range(width):
            lifetime[x][y] = 1.0 + random.uniform(0.0, 0.1)
            age[x][y] = random.uniform(0.0, 1.0) * lifetime[x][y]


@micropython.native  # noqa: F821
def draw():
    for y in range(height):
        for x in range(width):
            if age[x][y] < lifetime[x][y] * 0.3:
                gu.set_pixel(x, y, colour[0], colour[1], colour[2])
            elif age[x][y] < lifetime[x][y] * 0.5:
                decay = (lifetime[x][y] * 0.5 - age[x][y]) * 5.0
                gu.set_pixel(x, y, int(decay * colour[0]), int(decay * colour[1]), int(decay * colour[2]))


@micropython.native  # noqa: F821
def update():
    for y in range(height):
        for x in range(width):
            if age[x][y] >= lifetime[x][y]:
                age[x][y] = 0.0
                lifetime[x][y] = 1.0 + random.uniform(0.0, 0.1)

            age[x][y] += 0.025


setup()

gu.set_brightness(0.5)

while True:

    if gu.is_pressed(GalacticUnicorn.SWITCH_BRIGHTNESS_UP):
        gu.adjust_brightness(+0.01)

    if gu.is_pressed(GalacticUnicorn.SWITCH_BRIGHTNESS_DOWN):
        gu.adjust_brightness(-0.01)

    start = time.ticks_ms()

    draw()
    update()

    print("total took: {} ms".format(time.ticks_ms() - start))
