import time
import math
from picographics import PicoGraphics, DISPLAY_TUFTY_2040

display = PicoGraphics(display=DISPLAY_TUFTY_2040)

display.set_backlight(1.0)

WHITE = display.create_pen(255, 255, 255)
BLACK = display.create_pen(0, 0, 0)

# Reads name from file, and then closes the file.
try:
    file = open("message.txt", "r")
    message = file.readline()
    file.close()
except OSError:
    message = "CREATE MESSAGE.TXT IN THONNY :)"

message_length = display.measure_text(message, 10)

while True:
    t = 0    # counter
    x = 320  # Starting position of message, begins just off screen.
    offset = 10

    # How many loops it will take to scroll the whole message plus a little extra margin.
    scroll = (message_length / offset) + 30

    while t < scroll:
        step = t + t
        y = int(80 + math.sin(step / 5) * 30)
        h = 100 + math.sin(step / 5) * 100

        x -= offset

        display.set_pen(WHITE)
        display.clear()

        display.set_pen(BLACK)
        display.text(message, x, y, message_length, 10)
        display.update()

        time.sleep(0.02)

        t += 1
