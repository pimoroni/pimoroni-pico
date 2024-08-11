import time
from picoscroll import PicoScroll

scroll = PicoScroll()

while True:
    scroll.scroll_text("Hello World", 128, 80)
    time.sleep(1)
