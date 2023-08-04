import time
import math
from picographics import PicoGraphics, DISPLAY_TUFTY_2040, PEN_RGB332

display = PicoGraphics(DISPLAY_TUFTY_2040, pen_type=PEN_RGB332)
display.set_backlight(1.0)

WIDTH, HEIGHT = display.get_bounds()

BLACK = display.create_pen(0, 0, 0)


def scaled_sine(start, finish, speed):
    s = math.sin(time.ticks_ms() / speed)
    s += 1    # -1 to +1 to 0 to 2
    s /= 2.0  # 0 to 2 to 0 to 1
    s *= finish - start
    s += start
    return s


def regular_polygon(o_x, o_y, sides, radius, rotation):
    angle = math.radians(360 / sides)
    rotation = math.radians(rotation)

    points = []

    for side in range(sides):
        current_angle = side * angle + rotation
        x = math.cos(current_angle) * radius
        y = math.sin(current_angle) * radius
        points.append((int(x) + o_x, int(y) + o_y))

    return points


while True:
    sides = int(scaled_sine(3, 10, 500))
    rotation = time.ticks_ms() / 10
    display.set_pen(BLACK)
    display.clear()

    for y in range(HEIGHT):
        p = display.create_pen_hsv(y / HEIGHT, 1.0, 1.0)
        display.set_pen(p)
        display.line(0, y, WIDTH, y)

    display.set_pen(BLACK)

    points_a = regular_polygon(int(WIDTH / 2), int(HEIGHT / 2), sides, 100, rotation)
    points_b = regular_polygon(int(WIDTH / 2), int(HEIGHT / 2), sides, 50, -rotation)

    display.pretty_polygon(points_a, points_b)

    display.update()

    time.sleep(1.0 / 60)
