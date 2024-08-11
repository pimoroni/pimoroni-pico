import gc
import time
from stellar import StellarUnicorn
from picographics import PicoGraphics, DISPLAY_STELLAR_UNICORN, PEN_RGB888
from ulab import numpy

"""
This example demonstrates how to work with full RGB888 colour in ulab/numpy.

Each colour channel is given its own array, and these are combined before
copying them into the PicoGraphics buffer.

At great cost to performance (about half the speed) this example works in
floating point 0.0 to 1.0 and converts the result to 8bits per channel.
"""

# MAXIMUM OVERKILL
# machine.freq(250_000_000)

su = StellarUnicorn()
su.set_brightness(0.5)
graphics = PicoGraphics(DISPLAY_STELLAR_UNICORN, pen_type=PEN_RGB888)


def update():
    # Do something basic with the colour channels
    # to prove this actually works.
    red[:] = numpy.roll(red, 1, axis=1)
    green[:] *= 0.999   # Slowly desaturate green
    blue[:] *= 1.001    # Slowly saturate blue


def draw():
    # Copy the red, green, blue channels into
    # their respective places in the RGB_ array
    rgb[2::4] = red.flatten()
    rgb[1::4] = green.flatten()
    rgb[0::4] = blue.flatten()

    # Convert the results to 8bit RGB and copy to the framebuffer
    memoryview(graphics)[:] = numpy.ndarray(numpy.clip(rgb, 0, 1) * 255, dtype=numpy.uint8).tobytes()

    # Copy the framebuffer to Stellar
    su.update(graphics)
    # Whew!


width, height = graphics.get_bounds()

# Individual channels
red = numpy.zeros((height, width))
green = numpy.zeros((height, width))
blue = numpy.zeros((height, width))

# Reserved for combined channels
rgb = numpy.zeros((width * height * 4),)

# Stick some gradients in the channels so we have something to look at
red[::] = numpy.linspace(0, 1, width)

# There has to be a better way!?
for x in range(width):
    green[::, x] = numpy.linspace(0, 1, width)
    blue[::, x] = numpy.linspace(1, 0, width,)

t_count = 0
t_total = 0

while True:
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
