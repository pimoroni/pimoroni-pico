import time
import random
import picodisplay as display  #Comment this line out to use PicoDisplay2
#import picodisplay2 as display  #Uncomment this line to use PicoDisplay2

width = display.get_width()
height = display.get_height()

display_buffer = bytearray(width * height * 2)  # 2-bytes per pixel (RGB565)
display.init(display_buffer)

display.set_backlight(1.0)


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
            random.randint(r, r + (width - 2 * r)),
            random.randint(r, r + (height - 2 * r)),
            r,
            (14 - r) / 2,
            (14 - r) / 2,
            display.create_pen(random.randint(0, 255), random.randint(0, 255), random.randint(0, 255)),
        )
    )

while True:
    display.set_pen(40, 40, 40)
    display.clear()

    for ball in balls:
        ball.x += ball.dx
        ball.y += ball.dy

        xmax = width - ball.r
        xmin = ball.r
        ymax = height - ball.r
        ymin = ball.r

        if ball.x < xmin or ball.x > xmax:
            ball.dx *= -1

        if ball.y < ymin or ball.y > ymax:
            ball.dy *= -1

        display.set_pen(ball.pen)
        display.circle(int(ball.x), int(ball.y), int(ball.r))

    display.update()
    time.sleep(0.01)
