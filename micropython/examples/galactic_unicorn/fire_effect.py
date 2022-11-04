import time
import random
from galactic import GalacticUnicorn
from picographics import PicoGraphics, DISPLAY_GALACTIC_UNICORN as DISPLAY

'''
A pretty, procedural fire effect.

Switch between landscape fire and vertical fire using the A and B buttons!
You can adjust the brightness with LUX + and -.
'''

gu = GalacticUnicorn()
graphics = PicoGraphics(DISPLAY)

fire_colours = [graphics.create_pen(0, 0, 0),
                graphics.create_pen(20, 20, 20),
                graphics.create_pen(180, 30, 0),
                graphics.create_pen(220, 160, 0),
                graphics.create_pen(255, 255, 180)]


@micropython.native  # noqa: F821
def setup_landscape():
    global width, height, heat, fire_spawns, damping_factor
    width = GalacticUnicorn.WIDTH + 2
    height = GalacticUnicorn.HEIGHT + 4
    heat = [[0.0 for y in range(height)] for x in range(width)]
    fire_spawns = 5
    damping_factor = 0.97


@micropython.native  # noqa: F821
def setup_portrait():
    global width, height, heat, fire_spawns, damping_factor
    width = GalacticUnicorn.HEIGHT + 2
    height = GalacticUnicorn.WIDTH + 4
    heat = [[0.0 for y in range(height)] for x in range(width)]
    fire_spawns = 2
    damping_factor = 0.99


@micropython.native  # noqa: F821
def update():
    # clear the bottom row and then add a new fire seed to it
    for x in range(width):
        heat[x][height - 1] = 0.0
        heat[x][height - 2] = 0.0

    for c in range(fire_spawns):
        x = random.randint(0, width - 4) + 2
        heat[x + 0][height - 1] = 1.0
        heat[x + 1][height - 1] = 1.0
        heat[x - 1][height - 1] = 1.0
        heat[x + 0][height - 2] = 1.0
        heat[x + 1][height - 2] = 1.0
        heat[x - 1][height - 2] = 1.0

    for y in range(0, height - 2):
        for x in range(1, width - 1):
            # update this pixel by averaging the below pixels
            average = (
                heat[x][y] + heat[x][y + 1] + heat[x][y + 2] + heat[x - 1][y + 1] + heat[x + 1][y + 1]
            ) / 5.0

            # damping factor to ensure flame tapers out towards the top of the displays
            average *= damping_factor

            # update the heat map with our newly averaged value
            heat[x][y] = average


@micropython.native  # noqa: F821
def draw_landscape():
    for y in range(GalacticUnicorn.HEIGHT):
        for x in range(GalacticUnicorn.WIDTH):
            value = heat[x + 1][y]
            if value < 0.15:
                graphics.set_pen(fire_colours[0])
            elif value < 0.25:
                graphics.set_pen(fire_colours[1])
            elif value < 0.35:
                graphics.set_pen(fire_colours[2])
            elif value < 0.45:
                graphics.set_pen(fire_colours[3])
            else:
                graphics.set_pen(fire_colours[4])
            graphics.pixel(x, y)

    gu.update(graphics)


@micropython.native  # noqa: F821
def draw_portrait():
    for y in range(GalacticUnicorn.WIDTH):
        for x in range(GalacticUnicorn.HEIGHT):
            value = heat[x + 1][y]
            if value < 0.15:
                graphics.set_pen(fire_colours[0])
            elif value < 0.25:
                graphics.set_pen(fire_colours[1])
            elif value < 0.35:
                graphics.set_pen(fire_colours[2])
            elif value < 0.45:
                graphics.set_pen(fire_colours[3])
            else:
                graphics.set_pen(fire_colours[4])
            graphics.pixel(y, x)

    gu.update(graphics)


landscape = True
setup_landscape()

gu.set_brightness(0.5)

while True:

    if gu.is_pressed(GalacticUnicorn.SWITCH_BRIGHTNESS_UP):
        gu.adjust_brightness(+0.01)

    if gu.is_pressed(GalacticUnicorn.SWITCH_BRIGHTNESS_DOWN):
        gu.adjust_brightness(-0.01)

    if gu.is_pressed(GalacticUnicorn.SWITCH_A):
        landscape = True
        setup_landscape()

    if gu.is_pressed(GalacticUnicorn.SWITCH_B):
        landscape = False
        setup_portrait()

    start = time.ticks_ms()

    update()
    if landscape:
        draw_landscape()
    else:
        draw_portrait()

    # pause for a moment (important or the USB serial device will fail)
    time.sleep(0.001)

    print("total took: {} ms".format(time.ticks_ms() - start))
