import time
import random
from picographics import PicoGraphics, DISPLAY_ENVIRO_PLUS
from pimoroni import RGBLED

display = PicoGraphics(display=DISPLAY_ENVIRO_PLUS)
display.set_backlight(1.0)

led = RGBLED(6, 7, 10)
led.set_rgb(255, 255, 255)

WIDTH, HEIGHT = display.get_bounds()

# From CPython Lib/colorsys.py


def hsv_to_rgb(h, s, v):
    if s == 0.0:
        return v, v, v
    i = int(h * 6.0)
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


class Ball:
    def __init__(self, x, y, r, dx, dy, pen):
        self.x = x
        self.y = y
        self.r = r
        self.dx = dx
        self.dy = dy
        self.pen = pen


# initialise shapes
balls = []
for i in range(0, 100):
    r = random.randint(0, 10) + 3
    balls.append(
        Ball(
            random.randint(r, r + (WIDTH - 2 * r)),
            random.randint(r, r + (HEIGHT - 2 * r)),
            r,
            (14 - r) / 2,
            (14 - r) / 2,
            display.create_pen(random.randint(0, 255), random.randint(0, 255), random.randint(0, 255)),
        )
    )


hue = 0
while True:
    hue += 1
    r, g, b = [int(255 * c) for c in hsv_to_rgb(hue / 360.0, 1.0, 1.0)]  # rainbow magic
    led.set_rgb(r, g, b)  # pretty colours Led
    BG = display.create_pen(r, g, b)
    display.set_pen(BG)
    display.clear()

    for ball in balls:
        ball.x += ball.dx
        ball.y += ball.dy

        xmax = WIDTH - ball.r
        xmin = ball.r
        ymax = HEIGHT - ball.r
        ymin = ball.r

        if ball.x < xmin or ball.x > xmax:
            ball.dx *= -1

        if ball.y < ymin or ball.y > ymax:
            ball.dy *= -1

        display.set_pen(ball.pen)
        display.circle(int(ball.x), int(ball.y), int(ball.r))

    display.update()

time.sleep(0.01)
