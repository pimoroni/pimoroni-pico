import time
import random
from picounicorn import PicoUnicorn
from picographics import PicoGraphics, DISPLAY_UNICORN_PACK

'''
A pretty, procedural fire effect. Based on fire_effect.py from bigger Unicorns!
'''

picounicorn = PicoUnicorn()
graphics = PicoGraphics(display=DISPLAY_UNICORN_PACK)

fire_colours = [graphics.create_pen(0, 0, 0),
                graphics.create_pen(20, 20, 20),
                graphics.create_pen(180, 30, 0),
                graphics.create_pen(220, 160, 0),
                graphics.create_pen(255, 255, 180)]


@micropython.native  # noqa: F821
def update():
    # take local references as it's quicker than accessing the global
    # and we access it a lot in this method
    _heat = heat

    # clear the bottom row and then add a new fire seed to it
    for x in range(width):
        _heat[x][height - 1] = 0.0
        _heat[x][height - 2] = 0.0

    for c in range(fire_spawns):
        x = random.randint(0, width - 4) + 2
        _heat[x + 0][height - 1] = 1.0
        _heat[x + 1][height - 1] = 1.0
        _heat[x - 1][height - 1] = 1.0
        _heat[x + 0][height - 2] = 1.0
        _heat[x + 1][height - 2] = 1.0
        _heat[x - 1][height - 2] = 1.0

    factor = damping_factor / 5.0
    for y in range(0, height - 2):
        for x in range(1, width - 1):
            _heat[x][y] += _heat[x][y + 1] + _heat[x][y + 2] + _heat[x - 1][y + 1] + _heat[x + 1][y + 1]
            _heat[x][y] *= factor


@micropython.native  # noqa: F821
def draw():
    # take local references as it's quicker than accessing the global
    # and we access it a lot in this method
    _graphics = graphics
    _heat = heat
    _set_pen = graphics.set_pen
    _pixel = graphics.pixel
    _fire_colours = fire_colours

    for y in range(picounicorn.get_height()):
        for x in range(picounicorn.get_width()):
            value = _heat[y + 1][x]
            if value < 0.15:
                _set_pen(_fire_colours[0])
            elif value < 0.25:
                _set_pen(_fire_colours[1])
            elif value < 0.35:
                _set_pen(_fire_colours[2])
            elif value < 0.45:
                _set_pen(_fire_colours[3])
            else:
                _set_pen(_fire_colours[4])
            _pixel(x, y)

    picounicorn.update(_graphics)


width = picounicorn.get_height() + 2
height = picounicorn.get_width() + 4
heat = [[0.0 for y in range(height)] for x in range(width)]
fire_spawns = 1
damping_factor = 0.97

while True:

    start = time.ticks_ms()

    update()
    draw()

    print("total took: {} ms".format(time.ticks_ms() - start))

    # And sleep, so we update ~ 60fps
    time.sleep(1.0 / 60)
