from time import ticks_ms
import math

import hub75
# from font_8x12 import font, letter_width, letter_height
from font_10x14 import font, letter_width, letter_height


WIDTH, HEIGHT = 64, 64

hub = hub75.Hub75(WIDTH, HEIGHT, stb_invert=True)
hub.start()
hub.clear()

text = "      Hello World!"

t_s = ticks_ms()
f_s = ticks_ms() / 1000.0

frames = 0


def scroll_text(text, y, t):
    text_length = len(text)
    x = int(t)
    letter = int((x / letter_width) % text_length)
    pixel = x % letter_width
    char = ord(text[letter])
    for s_x in range(WIDTH):
        col = font[char - 32][pixel]
        s_y = y + int(math.sin((t / 3.0) + s_x / 30.0) * 8)
        hub.set_color_masked(s_x, s_y, col, hub75.color_hsv(s_x / WIDTH, 1.0, 1.0))
        pixel += 1
        if pixel == letter_width:
            pixel = 0
            letter += 1
            if letter == text_length:
                letter = 0
            char = ord(text[letter])


while True:
    hub.clear()
    t = (ticks_ms() - t_s) / 50.0
    scroll_text(text, int(HEIGHT / 2) - int(letter_height / 2), t)
    hub.flip()
    frames += 1
    if frames % 60 == 0:
        f_e = ticks_ms() / 1000.0
        print(frames / (f_e - f_s))
