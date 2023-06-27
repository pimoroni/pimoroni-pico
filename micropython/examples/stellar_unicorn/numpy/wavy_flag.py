import time
import math
import gc
from stellar import StellarUnicorn
from picographics import PicoGraphics, DISPLAY_STELLAR_UNICORN, PEN_P8
from ulab import numpy

"""
Wavy flags.
Try and change the flag design!
"""

# MAXIMUM OVERKILL
# machine.freq(250_000_000)

su = StellarUnicorn()
su.set_brightness(0.5)
graphics = PicoGraphics(DISPLAY_STELLAR_UNICORN, pen_type=PEN_P8)


# Colours
BLACK = graphics.create_pen(0, 0, 0)
TEAL = graphics.create_pen(0, 255, 255)
PINK = graphics.create_pen(255, 0, 255)
WHITE = graphics.create_pen(255, 255, 255)

RED = graphics.create_pen(255, 0, 0)
ORANGE = graphics.create_pen(255, 128, 0)
YELLOW = graphics.create_pen(255, 255, 0)
GREEN = graphics.create_pen(0, 255, 0)
BLUE = graphics.create_pen(0, 0, 255)
PURPLE = graphics.create_pen(128, 0, 255)


def update():
    buffer[:] = flag[:]
    t = time.ticks_ms() / 200
    for x in range(width):
        o = t + ((x / width) * 1.5 * math.pi)
        r = int(math.sin(o) * ((x + 3) / width) * 3)
        buffer[::, x] = numpy.roll(buffer[::, x], r, axis=0)


def draw():
    # Copy the fire effect to the framebuffer
    # Clips the fire to 0.0 to 1.0
    # Multiplies it by the number of palette entries (-1) to turn it into a palette index
    # Converts to uint8_t datatype to match the framebuffer
    memoryview(graphics)[:] = buffer[4:20, 0:16].tobytes()
    su.update(graphics)


width = StellarUnicorn.WIDTH
height = StellarUnicorn.HEIGHT + 8
flag = numpy.zeros((height, width), dtype=numpy.uint8)
buffer = numpy.zeros((height, width), dtype=numpy.uint8)

flag_column = [BLACK for _ in range(height)]

# Trans
# flag_column[7:9] = [TEAL] * 2
# flag_column[9:11] = [PINK] * 2
# flag_column[11:13] = [WHITE] * 2
# flag_column[13:15] = [PINK] * 2
# flag_column[15:17] = [TEAL] * 2

# Rainbow
flag_column[6:8] = [RED] * 2
flag_column[8:10] = [ORANGE] * 2
flag_column[10:12] = [YELLOW] * 2
flag_column[12:14] = [GREEN] * 2
flag_column[14:16] = [BLUE] * 2
flag_column[16:18] = [PURPLE] * 2


# Copy our vertical flag size to the numpy array
for x in range(width):
    flag[::, x] = flag_column


t_count = 0
t_total = 0


while True:
    gc.collect()

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
