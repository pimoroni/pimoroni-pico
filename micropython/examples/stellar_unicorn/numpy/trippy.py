import gc
import time
import random
from stellar import StellarUnicorn
from picographics import PicoGraphics, DISPLAY_STELLAR_UNICORN, PEN_P8
from ulab import numpy

"""
A random, trippy effect.
Experiment with the damping, number of spawns, intensity and offset to change the effect.
"""

# MAXIMUM OVERKILL
# machine.freq(250_000_000)

su = StellarUnicorn()
su.set_brightness(0.5)
graphics = PicoGraphics(DISPLAY_STELLAR_UNICORN, pen_type=PEN_P8)


DAMPING_FACTOR = 0.8
NUMBER_OF_DROPS = 2
INTENSITY = 10
OFFSET = 0.0  # Try 0.5

# Fill palette with a rainbow sweep
PALETTE_ENTRIES = 255
for x in range(PALETTE_ENTRIES):
    _ = graphics.create_pen_hsv(float(x) / PALETTE_ENTRIES + OFFSET, 1.0, 1.0)


def update():
    trippy[:] *= DAMPING_FACTOR

    # Spawn random drops
    for _ in range(NUMBER_OF_DROPS):
        x = random.randint(0, width - 1)
        y = random.randint(0, height - 1)
        trippy[y][x] = random.randint(0, INTENSITY)

    a = numpy.roll(trippy, 1, axis=0)
    b = numpy.roll(trippy, -1, axis=0)
    d = numpy.roll(trippy, 1, axis=1)
    e = numpy.roll(trippy, -1, axis=1)

    # Average over 5 adjacent pixels and apply damping
    trippy[:] += a + b + d + e
    trippy[:] /= 5.0


def draw():
    # Copy the effect to the framebuffer
    memoryview(graphics)[:] = numpy.ndarray(numpy.clip(trippy, 0, 1) * (PALETTE_ENTRIES - 1), dtype=numpy.uint8).tobytes()
    su.update(graphics)


width = StellarUnicorn.WIDTH
height = StellarUnicorn.HEIGHT
trippy = numpy.zeros((height, width))

t_count = 0
t_total = 0


while True:
    if su.is_pressed(StellarUnicorn.SWITCH_BRIGHTNESS_UP):
        su.adjust_brightness(+0.01)

    if su.is_pressed(StellarUnicorn.SWITCH_BRIGHTNESS_DOWN):
        su.adjust_brightness(-0.01)

    tstart = time.ticks_ms()
    gc.collect()
    update()
    draw()
    tfinish = time.ticks_ms()

    total = tfinish - tstart
    t_total += total
    t_count += 1

    if t_count == 60:
        per_frame_avg = t_total / t_count
        print(f"60 frames in {t_total}ms, avg {per_frame_avg:.02f}ms per frame, {1000/per_frame_avg:.02f} FPS")
        t_count = 0
        t_total = 0

    # pause for a moment (important or the USB serial device will fail)
    # try to pace at 60fps or 30fps
    if total > 1000 / 30:
        time.sleep(0.0001)
    elif total > 1000 / 60:
        t = 1000 / 30 - total
        time.sleep(t / 1000)
    else:
        t = 1000 / 60 - total
        time.sleep(t / 1000)
