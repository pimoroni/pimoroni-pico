import hub75
import time
import random
from picographics import PicoGraphics, DISPLAY_INTERSTATE75 as DISPLAY
WIDTH, HEIGHT = 32, 32

hub = hub75.Hub75(WIDTH, HEIGHT, panel_type=hub75.PANEL_GENERIC)
graphics = PicoGraphics(DISPLAY)
hub.start()

width = WIDTH
height = HEIGHT


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
for i in range(0, 10):
    r = random.randint(0, 3) + 3
    balls.append(
        Ball(
            random.randint(r, r + (WIDTH - 2 * r)),
            random.randint(r, r + (HEIGHT - 2 * r)),
            r,
            (7 - r) / 4,
            (7 - r) / 4,
            graphics.create_pen(random.randint(0, 255), random.randint(0, 255), random.randint(0, 255)),
        )
    )

BG = graphics.create_pen(0, 0, 0)

while True:
    graphics.set_pen(BG)
    graphics.clear()

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

        graphics.set_pen(ball.pen)
        graphics.circle(int(ball.x), int(ball.y), int(ball.r))

    hub.update(graphics)
    time.sleep(0.025)
