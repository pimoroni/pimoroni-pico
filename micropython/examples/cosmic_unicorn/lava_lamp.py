import time
import random
import math
from cosmic import CosmicUnicorn
from picographics import PicoGraphics, DISPLAY_COSMIC_UNICORN as DISPLAY

'''
A 70s-tastic, procedural rainbow lava lamp.

You can adjust the brightness with LUX + and -.
'''

cu = CosmicUnicorn()
graphics = PicoGraphics(DISPLAY)

blob_count = 10


class Blob():
    def __init__(self):
        self.x = float(random.randint(0, width - 1))
        self.y = float(random.randint(0, height - 1))
        self.r = (float(random.randint(0, 40)) / 10.0) + 5.0
        self.dx = (float(random.randint(0, 2)) / 10.0) - 0.1
        self.dy = (float(random.randint(0, 2)) / 10.0) - 0.05  # positive bias


@micropython.native  # noqa: F821
def setup_portrait():
    global width, height, liquid, blobs
    width = CosmicUnicorn.HEIGHT
    height = CosmicUnicorn.WIDTH
    liquid = [[0.0 for y in range(height)] for x in range(width)]
    blobs = [Blob() for i in range(blob_count)]


hue = 0.0


@micropython.native  # noqa: F821
def from_hsv(h, s, v):
    i = math.floor(h * 6.0)
    f = h * 6.0 - i
    v *= 255.0
    p = v * (1.0 - s)
    q = v * (1.0 - f * s)
    t = v * (1.0 - (1.0 - f) * s)

    i = int(i) % 6
    if i == 0:
        return graphics.create_pen(int(v), int(t), int(p))
    if i == 1:
        return graphics.create_pen(int(q), int(v), int(p))
    if i == 2:
        return graphics.create_pen(int(p), int(v), int(t))
    if i == 3:
        return graphics.create_pen(int(p), int(q), int(v))
    if i == 4:
        return graphics.create_pen(int(t), int(p), int(v))
    if i == 5:
        return graphics.create_pen(int(v), int(p), int(q))


@micropython.native  # noqa: F821
def update_liquid():
    for y in range(height):
        for x in range(width):
            liquid[x][y] = 0.0

    for blob in blobs:
        r_sq = blob.r * blob.r
        blob_y_range = range(max(math.floor(blob.y - blob.r), 0),
                             min(math.ceil(blob.y + blob.r), height))
        blob_x_range = range(max(math.floor(blob.x - blob.r), 0),
                             min(math.ceil(blob.x + blob.r), width))

        for y in blob_y_range:
            for x in blob_x_range:
                x_diff = x - blob.x
                y_diff = y - blob.y
                d_sq = x_diff * x_diff + y_diff * y_diff
                if d_sq <= r_sq:
                    liquid[x][y] += 1.0 - (d_sq / r_sq)


@micropython.native  # noqa: F821
def move_blobs():
    for blob in blobs:
        blob.x += blob.dx
        blob.y += blob.dy

        if blob.x < 0.0 or blob.x >= float(width):
            blob.dx = 0.0 - blob.dx

        if blob.y < 0.0 or blob.y >= float(height):
            blob.dy = 0.0 - blob.dy


@micropython.native  # noqa: F821
def draw_portrait():
    global hue
    hue += 0.001

    dark = from_hsv(hue, 1.0, 0.3)
    mid = from_hsv(hue, 1.0, 0.6)
    bright = from_hsv(hue, 1.0, 1.0)

    for y in range(height):
        for x in range(width):
            v = liquid[x][y]

            # select a colour for this pixel based on how much
            # "blobfluence" there is at this position in the liquid
            if v >= 1.5:
                graphics.set_pen(bright)
            elif v >= 1.25:
                graphics.set_pen(mid)
            elif v >= 1.0:
                graphics.set_pen(dark)
            else:
                graphics.set_pen(0)
            graphics.pixel(y, x)

    cu.update(graphics)


setup_portrait()

cu.set_brightness(0.5)

while True:

    if cu.is_pressed(CosmicUnicorn.SWITCH_BRIGHTNESS_UP):
        cu.adjust_brightness(+0.01)

    if cu.is_pressed(CosmicUnicorn.SWITCH_BRIGHTNESS_DOWN):
        cu.adjust_brightness(-0.01)

    if cu.is_pressed(CosmicUnicorn.SWITCH_A):
        setup_portrait()

    start = time.ticks_ms()

    update_liquid()
    move_blobs()
    draw_portrait()

    # pause for a moment (important or the USB serial device will fail)
    time.sleep(0.001)

    print("total took: {} ms".format(time.ticks_ms() - start))
