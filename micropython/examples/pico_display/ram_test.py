# Shows the available RAM. PEN_RGB332 is an 8 bit, fixed 256 colour palette which conserves your RAM.
# Try switching the pen_type to PEN_RGB565 (16 bit, 65K colour) and see the difference!
# If you have a Display Pack 2.0" or 2.8" use DISPLAY_PICO_DISPLAY_2 instead of DISPLAY_PICO_DISPLAY

import gc
import time
from picographics import PicoGraphics, DISPLAY_PICO_DISPLAY, PEN_RGB332

display = PicoGraphics(DISPLAY_PICO_DISPLAY, pen_type=PEN_RGB332, rotate=0)

# set up constants for drawing
WIDTH, HEIGHT = display.get_bounds()

BLACK = display.create_pen(0, 0, 0)

# what size steps to take around the colour wheel
OFFSET = 0.0025

# variable to keep track of the hue
h = 0.0


def free(full=False):
    # Calculates RAM usage
    gc.collect()
    F = gc.mem_free()
    A = gc.mem_alloc()
    T = F + A
    P = "{0:.2f}%".format(F / T * 100)
    if not full:
        return P
    return (f"Total RAM \n{T} bytes \nUnused RAM \n{F} bytes \n({P} free)")


while True:
    h += OFFSET

    display.set_pen(BLACK)
    RAINBOW = display.create_pen_hsv(h, 1.0, 1.0)
    display.set_pen(RAINBOW)
    display.set_font("bitmap8")
    display.text(free(full=True), 0, 0, WIDTH, 3)

    display.update()
    time.sleep(1.0 / 60)
