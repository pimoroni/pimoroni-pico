import gc
import time
import random
from stellar import StellarUnicorn
from picographics import PicoGraphics, DISPLAY_STELLAR_UNICORN, PEN_P8
from ulab import numpy

"""
A randomly-seeded game-of-life cellular automata effect.
Experiment with the values below to change the effect.

Press "A" to manually re-seed.
"""

# MAXIMUM OVERKILL
# machine.freq(250_000_000)

INITIAL_LIFE = 128        # Number of live cells to seed
GENERATION_TIME_MS = 100   # MS between generations
SMOOTHED = True           # Enable for a more organic if somewhat unsettling feel
STALEMATE_DEPTH = 5       # How many generations of changes must match before reset

DECAY = 0.90              # Rate at which smoothing effect decays, higher number = more persistent, 1.0 = no decay
TENACITY = 32             # Rate at which smoothing effect increases

su = StellarUnicorn()
su.set_brightness(0.5)
graphics = PicoGraphics(DISPLAY_STELLAR_UNICORN, pen_type=PEN_P8)

changed_cells = []


for c in range(256):
    graphics.create_pen(c // 2, 0, c)


def update():
    global last_gen, changed_cells

    if SMOOTHED:
        duration[:] += life * TENACITY
        duration[:] *= DECAY

    if time.ticks_ms() - last_gen < GENERATION_TIME_MS:
        return

    last_gen = time.ticks_ms()

    # Rollin' rollin' rollin.
    _N = numpy.roll(life, -1, axis=0)
    _NW = numpy.roll(_N, -1, axis=1)
    _NE = numpy.roll(_N, 1, axis=1)
    _S = numpy.roll(life, 1, axis=0)
    _SW = numpy.roll(_S, -1, axis=1)
    _SE = numpy.roll(_S, 1, axis=1)
    _W = numpy.roll(life, -1, axis=1)
    _E = numpy.roll(life, 1, axis=1)

    # Compute the total neighbours for each cell
    neighbours[:] = _N + _NW + _NE + _S + _SW + _SE + _W + _E

    next_generation[:] = life[:]

    # Any cells with exactly three neighbours should always stay alive
    next_generation[:] += neighbours[:] == 3

    # Any alive cells with less than two neighbours should die
    next_generation[:] -= (neighbours[:] < 2) * life

    # Any alive cells with more than three neighbours should die
    next_generation[:] -= (neighbours[:] > 3) * life

    next_generation[:] = numpy.clip(next_generation, 0, 1)

    changed_cells.append(numpy.sum(life != next_generation))
    changed_cells = changed_cells[-STALEMATE_DEPTH:]

    life[:] = next_generation

    if changed_cells.count(changed_cells[0]) == STALEMATE_DEPTH:
        seed_life(INITIAL_LIFE // 2)


def draw():
    # Copy the effect to the framebuffer
    if SMOOTHED:
        memoryview(graphics)[:] = numpy.ndarray(numpy.clip(duration, 0, 255), dtype=numpy.uint8).tobytes()
    else:
        memoryview(graphics)[:] = numpy.ndarray(life * 255, dtype=numpy.uint8).tobytes()
    su.update(graphics)


def seed_life(amount=INITIAL_LIFE):
    for _ in range(amount):
        x = random.randint(0, width - 1)
        y = random.randint(0, height - 1)
        life[y][x] = int(True)  # Avoid: TypeError: 'bool' object isn't iterable


width = StellarUnicorn.WIDTH
height = StellarUnicorn.HEIGHT
life = numpy.zeros((height, width), dtype=numpy.bool)
next_generation = numpy.zeros((height, width), dtype=numpy.bool)
neighbours = numpy.zeros((height, width), dtype=numpy.uint8)
duration = numpy.zeros((height, width))
last_gen = time.ticks_ms()

t_count = 0
t_total = 0

seed_life()


while True:
    if su.is_pressed(StellarUnicorn.SWITCH_BRIGHTNESS_UP):
        su.adjust_brightness(+0.01)

    if su.is_pressed(StellarUnicorn.SWITCH_BRIGHTNESS_DOWN):
        su.adjust_brightness(-0.01)

    if su.is_pressed(StellarUnicorn.SWITCH_A):
        life[:] = int(False)

    if su.is_pressed(StellarUnicorn.SWITCH_B):
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
