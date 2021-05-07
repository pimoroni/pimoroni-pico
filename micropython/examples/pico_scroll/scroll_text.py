import time
import picoscroll as scroll

scroll.init()

while True:
    scroll.scroll_text("Hello World", 128, 80)
    time.sleep(1)
