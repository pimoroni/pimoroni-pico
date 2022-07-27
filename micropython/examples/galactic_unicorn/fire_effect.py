import time
import random
from picographics import PicoGraphics, DISPLAY_GALACTIC_UNICORN
from galactic import GalacticUnicorn
from ulab import numpy as np

graphics = PicoGraphics(DISPLAY_GALACTIC_UNICORN)
gu = GalacticUnicorn()

width = 53
height = 15

heat = [0.0] * 1000
#heat = np.full((width + 5, height + 5), 0.0)

@micropython.native
def set(x, y, v):
    global heat
    heat[x + y * width] = v
    #heat[x, y] = v

@micropython.native
def get(x, y):
    global heat
    x = x if x >= 0 else 0
    x = x if x < width else width - 1
    #try:
    #return heat[x, y]
    return heat[x + y * width]
    #except IndexError:
    #    pass

@micropython.native
def select_colour(x, y):
    #value = heat[x, y]
    value = heat[x + y * width]
    if value > 0.5:
        return 255, 255, 180
    elif value > 0.4:
        return 220, 160, 0
    elif value > 0.3:
        return 180, 30, 0
    elif value > 0.22:
        return 20, 20, 20
    else:
        return 0, 0, 0


@micropython.native
def calc_average(x, y):
    #if x > 0:
    #    return (heat[x, y] + heat[x, y + 2] + heat[x, y + 1] + heat[x - 1, y + 1] + heat[x + 1, y + 1]) / 5.0
    return (heat[x + (y * width)] + heat[x + ((y + 2) * width)] + heat[x + ((y + 1) * width)] + heat[(x - 1 if x >= 0 else 0) + ((y + 1) * width)] + heat[(x + 1) + ((y + 1) * width)]) / 5.0
    #else:
        #return (heat[x, y] + heat[x, y + 2] + heat[x, y + 1] + heat[x - 0, y + 1] + heat[x + 1, y + 1]) / 5.0
        #return (heat[x + (y * width)] + heat[x + ((y + 2) * width)] + heat[x + ((y + 1) * width)] + heat[(x) + ((y + 1) * width)] + heat[(x + 1) + ((y + 1) * width)]) / 5.0


gu.set_brightness(0.3)

# heat[x-1:x+2, y:y+3]
#weights = np.array([[0.0, 0.0, 0.0],
                    #[0.0, 0.0, 0.0],
                    #[0.0, 0.0, 0.0]])

landscape = True

while True:
    start = time.ticks_ms()
    if gu.is_pressed(21):
        gu.adjust_brightness(+0.01)
    if gu.is_pressed(26):
        gu.adjust_brightness(-0.01)

    if gu.is_pressed(0):
        landscape = True
        width = 53
        height = 15
        for i in range(0, len(heat)):
            heat[i] = 0.0

    if gu.is_pressed(1):
        landscape = False
        width = 11
        height = 55
        for i in range(0, len(heat)):
            heat[i] = 0.0

    #for y, row in enumerate(heat):
        #for x, val in enumerate(row):
         #   print(x, y, row, val)

    for y in range(0, height):
        for x in range(0, width):
            r, g, b = select_colour(x, y)

            if landscape:
                gu.set_pixel(x, y, r, g, b)
                #graphics.pixel(x, y)
            else:
                gu.set_pixel(y, x, r, g, b)
                #graphics.pixel(y, x)

            # update this pixel by averaging the below pixels
            #average = (get(x, y) + get(x, y + 2) + get(x, y + 1) + get(x - 1, y + 1) + get(x + 1, y + 1)) / 5.0
            average = calc_average(x, y)

            # damping factor to ensure flame tapers out towards the top of the displays
            average *= 0.95 if landscape else 0.99

            # update the heat map with our newly averaged value
            set(x, y, average)

    #gu.update(graphics)

    # clear the bottom row and then add a new fire seed to it
    for x in range(0, width):
        set(x, height - 1, 0.0)

    # add a new random heat source
    source_count = 5 if landscape else 1

    for c in range(0, source_count):
        px = random.randint(0, width - 4) + 2
        set(px    , height - 2, 1.0)
        set(px + 1, height - 2, 1.0)
        set(px - 1, height - 2, 1.0)
        set(px    , height - 1, 1.0)
        set(px + 1, height - 1, 1.0)
        set(px - 1, height - 1, 1.0)

    end = time.ticks_ms()
    print("Time:", end - start)
    #time.sleep(0.02)