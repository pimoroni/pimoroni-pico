"""
Bitmap font demo!

Bitmap fonts are fast but blocky. They are best used for small text.
"""

from picographics import PicoGraphics, DISPLAY_PICO_DISPLAY_2

display = PicoGraphics(display=DISPLAY_PICO_DISPLAY_2)

WIDTH, HEIGHT = display.get_bounds()

hue = 0.0

display.set_font("bitmap8")

for i in range(7):
    # create a pen and set the drawing color
    PEN_COLOUR = display.create_pen_hsv(hue, 1.0, 1.0)
    display.set_pen(PEN_COLOUR)
    # draw text
    display.text("Hello World", i * WIDTH // 12, i * HEIGHT // 7 + 6, scale=3)
    # increment hue
    hue += 1.0 / 7

display.update()
