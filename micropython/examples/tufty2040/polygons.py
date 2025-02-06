import math
import time

from picographics import DISPLAY_TUFTY_2040, PEN_RGB332, PicoGraphics
from picovector import PicoVector, Polygon, Transform

display = PicoGraphics(DISPLAY_TUFTY_2040, pen_type=PEN_RGB332)
display.set_backlight(1.0)

vector = PicoVector(display)
t = Transform()
vector.set_transform(t)

WIDTH, HEIGHT = display.get_bounds()

BLACK = display.create_pen(0, 0, 0)


def scaled_sine(start, finish, speed):
    s = math.sin(time.ticks_ms() / speed)
    s += 1  # -1 to +1 to 0 to 2
    s /= 2.0  # 0 to 2 to 0 to 1
    s *= finish - start
    s += start
    return s


while True:
    t.reset()
    sides = int(scaled_sine(3, 10, 500))
    rotation = time.ticks_ms() / 10
    display.set_pen(BLACK)
    display.clear()

    for y in range(HEIGHT):
        p = display.create_pen_hsv(y / HEIGHT, 1.0, 1.0)
        display.set_pen(p)
        display.line(0, y, WIDTH, y)

    display.set_pen(BLACK)

    poly = Polygon()
    poly.regular(int(WIDTH / 2), int(HEIGHT / 2), 100, sides, 100)
    poly.regular(int(WIDTH / 2), int(HEIGHT / 2), 50, sides, 50)

    t.rotate(rotation, (int(WIDTH / 2), int(HEIGHT / 2)))
    vector.draw(poly)

    display.update()

    time.sleep(1.0 / 60)
