from picographics import PicoGraphics, DISPLAY_TUFTY_2040, PEN_P4
from pimoroni import Button
import math
import time

"""
Tufty2040: Solarized

Sets up Tufty 2040 in 4-bit colour mode and fills the
16-bit palette with some nice Solarized colours.

See: https://ethanschoonover.com/solarized/
"""


# Background & Content Tones
BASE_03 = 0
BASE_02 = 1
BASE_01 = 2
BASE_00 = 3
BASE_0 = 4
BASE_1 = 5
BASE_2 = 6
BASE_3 = 7

# Accent Colours
YELLOW = 8
ORANGE = 9
RED = 10
MAGENTA = 11
VIOLET = 12
BLUE = 13
CYAN = 14
GREEN = 15

# Friendly names for light/dark backgrounds
BG_DARK = BASE_03
BG_LIGHT = BASE_2


def bargraph(ox, oy, w, h, value, colors, pips=32):
    step = int(pips / len(colors))
    filled = int(value * pips)
    pip_width = int(w / pips)
    for i in range(pips):
        p = colors[int(i / step)]
        lcd.set_pen(p)
        x = i * pip_width
        if i > filled:
            lcd.rectangle(ox + x, oy + h - 1, pip_width - 2, 1)
        else:
            lcd.rectangle(ox + x, oy, pip_width - 2, h)


lcd = PicoGraphics(DISPLAY_TUFTY_2040, pen_type=PEN_P4)

lcd.set_backlight(0.8)
lcd.set_font("bitmap8")

button_up = Button(22, invert=False)
button_down = Button(6, invert=False)


# Prime the palette with Solarized 16-colours!
lcd.set_palette([
    (0x00, 0x2b, 0x36),  # Base 03
    (0x07, 0x36, 0x42),  # Base 02
    (0x58, 0x6e, 0x75),  # base 01
    (0x65, 0x7b, 0x83),  # Base 00
    (0x83, 0x94, 0x96),  # Base 0
    (0x93, 0xa1, 0xa1),  # Base 1
    (0xee, 0xe8, 0xd5),  # Base 2
    (0xfd, 0xf6, 0xe3),  # Base 3
    (0xb5, 0x89, 0x00),  # Yellow
    (0xcb, 0x4b, 0x16),  # Orange
    (0xdc, 0x32, 0x2f),  # Red
    (0xd3, 0x36, 0x82),  # Magenta
    (0x6c, 0x71, 0xc4),  # Violet
    (0x26, 0x8b, 0xd2),  # Blue
    (0x2a, 0xa1, 0x98),  # Cyan
    (0x85, 0x99, 0x00),  # Green
])

dark_mode = True


while True:
    if button_down.is_pressed:
        dark_mode = True
    if button_up.is_pressed:
        dark_mode = False

    lcd.set_pen(BG_DARK if dark_mode else BG_LIGHT)
    lcd.clear()

    texts = [
        "Vær så god",
        "Testing. Testing. 123.",
        "Helló Világ",
        "Comment ça va",
        "meine Straße hinauf",
        "It's a spicy 25°C",
        "What do you think?",
        "éphéméréité"
    ]

    for i, pen in enumerate([YELLOW, ORANGE, RED, MAGENTA, VIOLET, BLUE, CYAN, GREEN]):
        lcd.set_pen(pen)
        lcd.text(texts[i], 10, i * 22 + 10, 320)

    v1 = (math.sin(time.ticks_ms() / 400.0 / math.pi) + 1.0) / 2.0
    v2 = (math.sin(time.ticks_ms() / 650.0 / math.pi) + 1.0) / 2.0
    v3 = (math.sin(time.ticks_ms() / 900.0 / math.pi) + 1.0) / 2.0

    bargraph(10, 188, 300, 8, v1, [VIOLET, BLUE, CYAN, GREEN])
    bargraph(10, 200, 300, 8, v2, [GREEN, YELLOW, RED, MAGENTA])
    bargraph(10, 212, 300, 8, v3, [CYAN, GREEN, YELLOW, ORANGE])

    lcd.update()
