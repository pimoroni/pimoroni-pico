import gc
import time
import random
from galactic import GalacticUnicorn
from picographics import PicoGraphics, DISPLAY_GALACTIC_UNICORN, PEN_P8
from ulab import numpy

"""
A randomly-seeded game-of-life cellular automata effect.

The values below are dialled in to give a subtle pulse on stable patterns,
experiment with them to change the effect.

Press "A" to manually re-seed.

Press "B" to toggle smoothing.
"""

# MAXIMUM OVERKILL
# machine.freq(250_000_000)

INITIAL_LIFE = 500        # Number of live cells to seed
GENERATION_TIME_MS = 200  # MS between generations
MINIMUM_LIFE = 10         # Auto reseed when only this many alive cells remain
SMOOTHED = True           # Enable for a more organic if somewhat unsettling feel

DECAY = 0.92              # Rate at which smoothing effect decays, higher number = more persistent, 1.0 = no decay
TENACITY = 32             # Rate at which smoothing effect increases

gu = GalacticUnicorn()
gu.set_brightness(0.5)
graphics = PicoGraphics(DISPLAY_GALACTIC_UNICORN, pen_type=PEN_P8)

for c in range(256):
    graphics.create_pen(c // 2, 0, c)


def update():
    global last_gen

    if SMOOTHED:
        duration[:] += life * TENACITY
        duration[:] *= DECAY

    if time.ticks_ms() - last_gen < GENERATION_TIME_MS:
        return

    last_gen = time.ticks_ms()

    if numpy.sum(life) < MINIMUM_LIFE:
        seed_life()
        return

    _N = numpy.roll(life, -1, axis=0)
    _NW = numpy.roll(_N, -1, axis=1)
    _NE = numpy.roll(_N, 1, axis=1)
    _S = numpy.roll(life, 1, axis=0)
    _SW = numpy.roll(_S, -1, axis=1)
    _SE = numpy.roll(_S, 1, axis=1)
    _W = numpy.roll(life, -1, axis=1)
    _E = numpy.roll(life, 1, axis=1)

    neighbours[:] = _N + _NW + _NE + _S + _SW + _SE + _W + _E

    next_generation[:] = life[:]

    for x in range(width):
        for y in range(height):
            count = neighbours[y][x]

            if life[y][x]:  # Alive
                if count < 2 or count > 3:
                    next_generation[y][x] = int(False)
            elif count == 3:
                next_generation[y][x] = int(True)

    life[:] = next_generation


def draw():
    # Copy the effect to the framebuffer
    if SMOOTHED:
        memoryview(graphics)[:] = numpy.ndarray(numpy.clip(duration, 0, 255), dtype=numpy.uint8).tobytes()
    else:
        memoryview(graphics)[:] = numpy.ndarray(life * 255, dtype=numpy.uint8).tobytes()
    gu.update(graphics)


def seed_life():
    for _ in range(INITIAL_LIFE):
        x = random.randint(0, width - 1)
        y = random.randint(0, height - 1)
        life[y][x] = int(True)  # Avoid: TypeError: 'bool' object isn't iterable


width = GalacticUnicorn.WIDTH
height = GalacticUnicorn.HEIGHT
life = numpy.zeros((height, width), dtype=numpy.bool)
next_generation = numpy.zeros((height, width), dtype=numpy.bool)
neighbours = numpy.zeros((height, width), dtype=numpy.uint8)
duration = numpy.zeros((height, width))
last_gen = time.ticks_ms()

t_count = 0
t_total = 0

seed_life()


while True:
    if gu.is_pressed(GalacticUnicorn.SWITCH_BRIGHTNESS_UP):
        gu.adjust_brightness(+0.01)

    if gu.is_pressed(GalacticUnicorn.SWITCH_BRIGHTNESS_DOWN):
        gu.adjust_brightness(-0.01)

    if gu.is_pressed(GalacticUnicorn.SWITCH_A):
        life[:] = int(False)

    if gu.is_pressed(GalacticUnicorn.SWITCH_B):
        SMOOTHED = not SMOOTHED

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
