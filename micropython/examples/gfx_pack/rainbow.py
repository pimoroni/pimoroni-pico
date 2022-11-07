# This example borrows a CircuitPython hsv_to_rgb function to cycle through some rainbows on GFX Pack's screen.
# If you're into rainbows, HSV (Hue, Saturation, Value) is very useful!

import time
from picographics import PicoGraphics, DISPLAY_GFX_PACK
from gfx_pack import GfxPack

display = PicoGraphics(display=DISPLAY_GFX_PACK, rotate=0)

WIDTH, HEIGHT = display.get_bounds()

gp = GfxPack()


# From CPython Lib/colorsys.py
def hsv_to_rgb(h, s, v):
    if s == 0.0:
        return v, v, v
    i = int(h * 6.0)
    f = (h * 6.0) - i
    p = v * (1.0 - s)
    q = v * (1.0 - s * f)
    t = v * (1.0 - s * (1.0 - f))
    i = i % 6
    if i == 0:
        return v, t, p
    if i == 1:
        return q, v, p
    if i == 2:
        return p, v, t
    if i == 3:
        return p, q, v
    if i == 4:
        return t, p, v
    if i == 5:
        return v, p, q


h = 0
display.set_font("bitmap8")

while True:
    h += 1
    r, g, b = [int(255 * c) for c in hsv_to_rgb(h / 360.0, 1.0, 1.0)]  # rainbow magic
    gp.set_backlight(r, g, b, 0)  # Set backlight to a converted HSV value
    display.set_pen(15)           # Set pen to black
    display.clear()
    display.set_pen(0)            # Set pen to white
    display.text("pico disco!", 18, 5, WIDTH, 2)   # Add some text
    display.text("\\o/ \\o/ \\o/", 14, 25, WIDTH, 2)  # and some more text
    display.text("\\o/ \\o/ \\o/", 14, 45, WIDTH, 2)  # and a bit more text
    display.update()              # Update the display
    time.sleep(1.0 / 60)
