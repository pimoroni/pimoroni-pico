# This example borrows a CircuitPython hsv_to_rgb function to cycle through some rainbows on GFX Pack's screen.
# If you're into rainbows, HSV (Hue, Saturation, Value) is very useful!

import time
from gfx_pack import GfxPack

gp = GfxPack()
display = gp.display

WIDTH, HEIGHT = display.get_bounds()
display.set_backlight(0)  # turn off the white component of the backlight

DANCE_TIME = 1.0


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


# some variables to keep track of rainbows and dancing
h = 0
dancing = True
last_time = time.ticks_ms()

display.set_font("bitmap8")

while True:
    time_ms = time.ticks_ms()

    h += 1
    r, g, b = [int(255 * c) for c in hsv_to_rgb(h / 360.0, 1.0, 1.0)]  # rainbow magic
    gp.set_backlight(r, g, b)     # Set backlight to a converted HSV value
    display.set_pen(0)            # Set pen to white
    display.clear()
    display.set_pen(15)           # Set pen to black
    # draw text and mans
    if dancing:
        display.text("pico!", 9, 5, WIDTH, 2)
        display.text("\\o\\ \\o\\ \\o\\ ", 7, 25, WIDTH, 2)
        display.text("\\o\\ \\o\\ \\o\\ ", 7, 45, WIDTH, 2)
    else:
        display.text("disco!", 69, 5, WIDTH, 2)
        display.text("/o/ /o/ /o/ ", 21, 25, WIDTH, 2)
        display.text("/o/ /o/ /o/ ", 21, 45, WIDTH, 2)
    # our main loop is faster than our dancing loop
    # this 'if' checks when it's time to dance
    if time_ms - last_time > DANCE_TIME * 1000:
        dancing = not dancing
        last_time = time_ms
    display.update()
    time.sleep(1.0 / 60)
