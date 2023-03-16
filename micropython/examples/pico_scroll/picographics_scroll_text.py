from picographics import PicoGraphics, DISPLAY_SCROLL_PACK, PEN_P8
from picoscroll import PicoScroll
import time

TEXT = "Hello World"

# By default P8 has a greyscale palette
graphics = PicoGraphics(DISPLAY_SCROLL_PACK, pen_type=PEN_P8)
scroll = PicoScroll()

t = scroll.get_width()

wrap = -graphics.measure_text(TEXT, scale=0)

while True:
    graphics.set_pen(0)
    graphics.clear()
    graphics.set_pen(255)
    graphics.text(TEXT, t, 0, scale=1)
    scroll.update(graphics)
    t -= 1
    time.sleep(0.1)
    if t <= wrap:
        t = scroll.get_width()
