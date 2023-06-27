import time
import math
import gc
from cosmic import CosmicUnicorn
from picographics import PicoGraphics, DISPLAY_COSMIC_UNICORN, PEN_P8
from ulab import numpy

"""
Wavy flags.
Try and change the flag design!
"""

# MAXIMUM OVERKILL
# machine.freq(250_000_000)

cu = CosmicUnicorn()
cu.set_brightness(0.5)
graphics = PicoGraphics(DISPLAY_COSMIC_UNICORN, pen_type=PEN_P8)


# Colours
BLACK = graphics.create_pen(0, 0, 0)
TEAL = graphics.create_pen(0, 255, 255)
PINK = graphics.create_pen(255, 0, 255)
WHITE = graphics.create_pen(255, 255, 255)
GREY = graphics.create_pen(128, 128, 128)

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
    memoryview(graphics)[:] = buffer[4:36, 0:32].tobytes()
    cu.update(graphics)


width = CosmicUnicorn.WIDTH
height = CosmicUnicorn.HEIGHT + 8
flag = numpy.zeros((height, width), dtype=numpy.uint8)
buffer = numpy.zeros((height, width), dtype=numpy.uint8)

flag_column = [BLACK for _ in range(height)]

y = 8
stripe_height = 3
for c in (RED, ORANGE, YELLOW, GREEN, BLUE, PURPLE):
    flag_column[y:y + stripe_height] = [c] * stripe_height
    y += stripe_height


# Copy our vertical flag size to the numpy array
for x in range(3, width - 2):
    flag[::, x] = flag_column

# Draw a flag pole
flag[::, 2] = [0] * 6 + [GREY] * (height - 6)
flag[::, 3] = [0] * 6 + [GREY] * (height - 6)


t_count = 0
t_total = 0


while True:
    gc.collect()

    if cu.is_pressed(CosmicUnicorn.SWITCH_BRIGHTNESS_UP):
        cu.adjust_brightness(+0.01)

    if cu.is_pressed(CosmicUnicorn.SWITCH_BRIGHTNESS_DOWN):
        cu.adjust_brightness(-0.01)

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
