# This example borrows a CircuitPython hsv_to_rgb function to cycle through some rainbows on Pico Display's screen and RGB LED . If you're into rainbows, HSV (Hue, Saturation, Value) is very useful!

import utime
import st7789
from pimoroni import RGBLED

# Set the display resolution
# in most cases you can swap WIDTH weith HEIGHT for portrait mode
WIDTH, HEIGHT = 240, 135    # Pico Display
# WIDTH, HEIGHT = 320, 240  # Pico Display 2.0

display = st7789.ST7789(WIDTH, HEIGHT, rotate180=False)
display.set_backlight(0.8)

led = RGBLED(6, 7, 8)


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
    display.set_pen(r, g, b)  # Set pen to a converted HSV value
    display.clear()           # Fill the screen with the colour
    display.set_pen(0, 0, 0)  # Set pen to black
    display.text("pico disco!", 10, 10, 240, 6)  # Add some text
    display.update()          # Update the display
    utime.sleep(1.0 / 60)
