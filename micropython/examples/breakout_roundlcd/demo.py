import time
import math
from picographics import PicoGraphics, DISPLAY_ROUND_LCD_240X240

display = PicoGraphics(display=DISPLAY_ROUND_LCD_240X240)
display.set_backlight(1.0)

WIDTH, HEIGHT = display.get_bounds()
RADIUS = WIDTH // 2

BLACK = display.create_pen(0, 0, 0)


def hsv_to_rgb(h, s, v):
    if s == 0.0:
        return v, v, v
    i = int(h * 6.0)  # XXX assume int() truncates!
    f = (h * 6.0) - i
    p = v * (1.0 - s)
    q = v * (1.0 - s * f)
    t = v * (1.0 - s * (1.0 - f))
    i = i % 6
    if i == 0:
        return v, t, p
    if i == 1:
        return q, v, p
    if i == 2:
        return p, v, t
    if i == 3:
        return p, q, v
    if i == 4:
        return t, p, v
    if i == 5:
        return v, p, q


t = 0

while True:
    display.set_pen(BLACK)
    display.clear()

    angle = t % (math.pi * 2)

    prev_x = RADIUS
    prev_y = RADIUS

    steps = 30.0
    angle_step = 0.5

    for step in range(int(steps)):
        angle += angle_step

        distance = RADIUS / steps * step
        distance += step * 0.2

        r, g, b = [int(c * 255) for c in hsv_to_rgb((t / 10.0) + distance / 120.0, 1.0, 1.0)]

        x = RADIUS + int(distance * math.cos(angle))
        y = RADIUS + int(distance * math.sin(angle))

        radius = ((math.sin(t + angle) + 1) / 2.0) * 10
        dot_colour = display.create_pen(r, g, b)
        display.set_pen(dot_colour)
        display.circle(int(x), int(y), int(radius))

        prev_x = x
        prev_y = y

    display.update()
    time.sleep(0.02)
    t += 0.02
