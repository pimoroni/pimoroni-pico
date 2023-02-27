import time
import gc
import random
from cosmic import CosmicUnicorn
from picographics import PicoGraphics, DISPLAY_COSMIC_UNICORN, PEN_P8
from ulab import numpy

"""
Classic fire effect.
Play with the number of spawns, heat, damping factor and colour palette to tweak it.
"""

# MAXIMUM OVERKILL
# machine.freq(250_000_000)

cu = CosmicUnicorn()
cu.set_brightness(0.5)
graphics = PicoGraphics(DISPLAY_COSMIC_UNICORN, pen_type=PEN_P8)

# Number of random fire spawns
FIRE_SPAWNS = 5

# Fire damping
DAMPING_FACTOR = 0.98

# TURN UP THE HEEEAAT
HEAT = 3.0

# Create the fire palette
"""
# Raging Gas Inferno
graphics.create_pen(0, 0, 0)
graphics.create_pen(0, 0, 0)
graphics.create_pen(20, 20, 20)
graphics.create_pen(50, 10, 0)
graphics.create_pen(180, 30, 0)
graphics.create_pen(220, 160, 0)
graphics.create_pen(255, 255, 180)
graphics.create_pen(255, 255, 220)
graphics.create_pen(90, 90, 255)
graphics.create_pen(255, 0, 255)
"""
# Original Colours
graphics.create_pen(0, 0, 0)
graphics.create_pen(20, 20, 20)
graphics.create_pen(180, 30, 0)
graphics.create_pen(220, 160, 0)
graphics.create_pen(255, 255, 180)

PALETTE_SIZE = 5  # Should match the number of colours defined above


def update():
    # Clear the bottom two rows (off screen)
    heat[height - 1][:] = 0.0
    heat[height - 2][:] = 0.0

    # Add random fire spawns
    for c in range(FIRE_SPAWNS):
        x = random.randint(0, width - 4) + 2
        heat[height - 1][x - 1:x + 1] = HEAT / 2.0
        heat[height - 2][x - 1:x + 1] = HEAT

    # Propagate the fire upwards
    a = numpy.roll(heat, -1, axis=0)  # y + 1, x
    b = numpy.roll(heat, -2, axis=0)  # y + 2, x
    c = numpy.roll(heat, -1, axis=0)  # y + 1
    d = numpy.roll(c, 1, axis=1)      # y + 1, x + 1
    e = numpy.roll(c, -1, axis=1)     # y + 1, x - 1

    # Average over 5 adjacent pixels and apply damping
    heat[:] += a + b + d + e
    heat[:] *= DAMPING_FACTOR / 5.0


def draw():
    # Copy the fire effect to the framebuffer
    # Clips the fire to 0.0 to 1.0
    # Multiplies it by the number of palette entries (-1) to turn it into a palette index
    # Converts to uint8_t datatype to match the framebuffer
    memoryview(graphics)[:] = numpy.ndarray(numpy.clip(heat[0:32, 0:32], 0, 1) * (PALETTE_SIZE - 1), dtype=numpy.uint8).tobytes()
    cu.update(graphics)


width = CosmicUnicorn.WIDTH
height = CosmicUnicorn.HEIGHT + 4
heat = numpy.zeros((height, width))

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
