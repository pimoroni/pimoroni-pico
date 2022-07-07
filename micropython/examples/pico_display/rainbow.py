# This example borrows a CircuitPython hsv_to_rgb function to cycle through some rainbows on Pico Display's screen and RGB LED . If you're into rainbows, HSV (Hue, Saturation, Value) is very useful!
# We're using a RAM intensive 64K colour palette here to get a nice smooth colour transition.

import time
from picographics import PicoGraphics, DISPLAY_PICO_DISPLAY, PEN_RGB565
from pimoroni import RGBLED

display = PicoGraphics(display=DISPLAY_PICO_DISPLAY, pen_type=PEN_RGB565, rotate=0)
display.set_backlight(0.8)

led = RGBLED(6, 7, 8)

WIDTH, HEIGHT = display.get_bounds()

BLACK = display.create_pen(0, 0, 0)


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
    led.set_rgb(r, g, b)      # Set LED to a converted HSV value
    RAINBOW = display.create_pen(r, g, b)  # Create pen with converted HSV value
    display.set_pen(RAINBOW)  # Set pen
    display.clear()           # Fill the screen with the colour
    display.set_pen(BLACK)    # Set pen to black
    display.text("pico disco!", 10, 10, 240, 6)  # Add some text
    display.update()          # Update the display
    time.sleep(1.0 / 60)
