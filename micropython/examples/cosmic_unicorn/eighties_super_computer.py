import time
import random
from galactic import GalacticUnicorn
from picographics import PicoGraphics, DISPLAY_GALACTIC_UNICORN as DISPLAY

'''
Random LEDs blink on and off mimicing the look of a movie
super computer doing its work in the eighties.

You can adjust the brightness with LUX + and -.
'''

gu = GalacticUnicorn()
graphics = PicoGraphics(DISPLAY)

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
                graphics.set_pen(graphics.create_pen(colour[0], colour[1], colour[2]))
            elif age[x][y] < lifetime[x][y] * 0.5:
                decay = (lifetime[x][y] * 0.5 - age[x][y]) * 5.0
                graphics.set_pen(graphics.create_pen(int(decay * colour[0]), int(decay * colour[1]), int(decay * colour[2])))
            else:
                graphics.set_pen(0)
            graphics.pixel(x, y)

    gu.update(graphics)


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

    # pause for a moment (important or the USB serial device will fail)
    time.sleep(0.001)

    print("total took: {} ms".format(time.ticks_ms() - start))
