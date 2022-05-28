# This example borrows a CircuitPython hsv_to_rgb function to cycle through some rainbows on Pico Explorer's screen.
# If you're into rainbows, HSV (Hue, Saturation, Value) is very useful!

import utime
import st7789


display = st7789.ST7789(st7789.DISPLAY_PICO_EXPLORER, rotate=0)
display.set_palette_mode(st7789.PALETTE_USER)
display.set_backlight(1.0)

# Create a text colour
TEXT_COLOR = display.create_pen(0, 0, 0)

# Reserve a palette entry for our rainbow background colour
RAINBOW = display.reserve_palette()


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

while True:
    h += 1
    r, g, b = [int(255 * c) for c in hsv_to_rgb(h / 360.0, 1.0, 1.0)]  # rainbow magic
    display.set_palette(RAINBOW, st7789.RGB565(r, g, b))  # Set pen to a converted HSV value
    display.set_pen(RAINBOW)
    display.clear()           # Fill the screen with the colour
    display.set_pen(TEXT_COLOR)
    display.text("pico disco!", 25, 20, 240, 6)  # Add some text
    display.text("\\o/ \\o/ \\o/ \\o/ \\o/ \\o/ \\o/ \\o/ \\o/", 25, 120, 240, 4)  # and some more text
    display.text("oontz oontz oontz", 25, 220, 240, 2)  # and a bit more tiny text
    display.update()          # Update the display
    utime.sleep(1.0 / 60)
