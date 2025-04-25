# This example cycles through some rainbows on Pico Display's screen and RGB LED, using the HSV colour model.
# (If you're into rainbows, HSV (Hue, Saturation, Value) is very useful)
# We're using a RAM intensive 64K colour palette here to get a nice smooth colour transition.
# If you have a Display Pack 2.0" or 2.8" use DISPLAY_PICO_DISPLAY_2 instead of DISPLAY_PICO_DISPLAY

import time
from picographics import PicoGraphics, DISPLAY_PICO_DISPLAY, PEN_RGB565
from pimoroni import RGBLED

display = PicoGraphics(display=DISPLAY_PICO_DISPLAY, pen_type=PEN_RGB565, rotate=0)
display.set_backlight(0.8)

# set up constants for drawing
WIDTH, HEIGHT = display.get_bounds()
BLACK = display.create_pen(0, 0, 0)

# what size steps to take around the colour wheel
OFFSET = 0.0025

# variable to keep track of the hue
h = 0.0

# Set up the RGB LED For Display Pack and Display Pack 2.0":
led = RGBLED(6, 7, 8)

# For Display Pack 2.8" uncomment the following line and comment out the line above:
# led = RGBLED(26, 27, 28)


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
    if i == 5:  # noqa: RET503
        return v, p, q


while True:
    # increment the hue each time round the loop
    h += OFFSET

    # The LED needs to be set using RGB values, so convert HSV to RGB using the hsv_to_rgb() function above
    r, g, b = [int(255 * c) for c in hsv_to_rgb(h, 1.0, 1.0)]
    led.set_rgb(r, g, b)

    # Fill the screen with the chosen hue, we can use PicoGraphics' built in HSV pen function for this
    RAINBOW = display.create_pen_hsv(h, 1.0, 1.0)
    display.set_pen(RAINBOW)
    display.clear()

    # Draw some black text
    display.set_pen(BLACK)
    display.text("pico disco!", 10, 10, 240, 6)

    display.update()
    time.sleep(1.0 / 60)
