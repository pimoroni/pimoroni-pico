# This is a PicoGraphics version of the original demo.py

from picounicorn import PicoUnicorn
from picographics import PicoGraphics, DISPLAY_UNICORN_PACK
import time

picounicorn = PicoUnicorn()
graphics = PicoGraphics(display=DISPLAY_UNICORN_PACK)

w = picounicorn.get_width()
h = picounicorn.get_height()

while True:
    t = time.ticks_ms() / 3600
    for x in range(w):
        for y in range(h):
            # PicoGraphics allows us to set HSV pens directly
            PEN = graphics.create_pen_hsv(t + ((x + y) / w / 4), 1.0, 1.0)
            graphics.set_pen(PEN)
            graphics.pixel(x, y)

    # Ask the Unicorn to update the graphics
    picounicorn.update(graphics)

    # And sleep, so we update ~ 60fps
    time.sleep(1.0 / 60)
