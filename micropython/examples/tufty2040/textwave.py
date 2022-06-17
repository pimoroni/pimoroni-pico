import math
import time
from picographics import PicoGraphics, DISPLAY_TUFTY_2040

display = PicoGraphics(display=DISPLAY_TUFTY_2040)

WIDTH, HEIGHT = 320, 240
display.set_backlight(1.0)

WHITE = display.create_pen(255, 255, 255)
BLACK = display.create_pen(0, 0, 0)

display.set_pen(WHITE)
display.clear()
display.update()

message = "HELLO WORLD!"


def calculate_text_size(text):
    size = 20
    width = display.measure_text(text, size)
    while width > 280 and size > 1:
        size -= 1
        width = display.measure_text(text, size)

    return size


size = calculate_text_size(message)
length = len(message)

while True:
    display.set_pen(WHITE)
    display.clear()
    display.set_pen(BLACK)

    t = time.ticks_ms() / 10 / size

    left = int((WIDTH - ((length - 1) * size * 6)) / 2)
    top = 120 - int((size * 6) / 2)

    for i in range(length):
        step = t + i
        y = top - math.sin(step / length * math.pi) * 10
        display.text(message[i], left + (i * size * 6), int(y), 0, size)

    display.update()
