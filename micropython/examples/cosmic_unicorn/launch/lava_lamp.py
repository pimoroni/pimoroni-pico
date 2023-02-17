import gc
import time
import math
import random
from cosmic import CosmicUnicorn
from ulab import numpy

"""
A lava lamp effect, created by blurred, moving particles.
"""

# MAXIMUM OVERKILL
# machine.freq(250_000_000)

gu = CosmicUnicorn()
graphics = None
gu.set_brightness(0.5)

width = CosmicUnicorn.WIDTH
height = CosmicUnicorn.HEIGHT
lava = numpy.zeros((height, width))


class Blob():
    def __init__(self):
        self.x = float(random.randint(0, width - 1))
        self.y = float(random.randint(0, height - 1))
        self.r = (float(random.randint(0, 40)) / 10.0) + 5.0
        self.dx = (float(random.randint(0, 2)) / 20.0) - 0.05
        self.dy = (float(random.randint(0, 2)) / 20.0) - 0.025  # positive bias

    def move(self):
        self.x += self.dx
        self.y += self.dy

        if self.x < 0.0 or self.x >= float(width):
            self.x = max(0.0, self.x)
            self.x = min(float(width - 1), self.x)
            self.dx = -self.dx

        if self.y < 0.0 or self.y >= float(height):
            self.y = max(0.0, self.y)
            self.y = min(float(width - 1), self.y)
            self.dy = -self.dy

def init():
    blobs = [Blob() for _ in range(10)]


    # Fill palette with a steep falloff from bright red to dark blue
    PAL_COLS = 9
    for x in range(PAL_COLS):
        graphics.create_pen_hsv(0.5 + math.log(x + 1, PAL_COLS + 1) / 2.0, 1.0, math.log(x + 1, PAL_COLS + 1))


def update():
    # Update the blobs and draw them into the effect
    for blob in blobs:
        blob.move()
        lava[int(blob.y)][int(blob.x)] = blob.r

    # Propogate the blobs outwards
    a = numpy.roll(lava, 1, axis=0)
    b = numpy.roll(lava, -1, axis=0)
    d = numpy.roll(lava, 1, axis=1)
    e = numpy.roll(lava, -1, axis=1)

    # Average over 5 adjacent pixels and apply damping
    lava[:] += a + b + d + e
    lava[:] *= 0.97 / 5.0


def draw_plasma():
    # Copy the lava effect to the framebuffer
    # Clips to 0.0 - 1.0
    # Multiplies by palette entries (-1) to turn it into a palette index
    # Converts to uint8_t datatype to match the framebuffer
    memoryview(graphics)[:] = numpy.ndarray(numpy.clip(lava, 0.0, 1.0) * (PAL_COLS - 1), dtype=numpy.uint8).tobytes()
    gu.update(graphics)


t_count = 0
t_total = 0

def draw():


    gc.collect()
    update()
    draw_plasma()

