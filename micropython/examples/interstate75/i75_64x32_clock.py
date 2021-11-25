import hub75
from time import ticks_ms, sleep
import math
from machine import RTC

WIDTH, HEIGHT = 64, 32
MAX_DIST = (WIDTH * WIDTH + HEIGHT * HEIGHT) * 0.5
DIGITS = [0b1110111, 0b0100100, 0b1011101, 0b1101101, 0b0101110,
          0b1101011, 0b1111010, 0b0100101, 0b1111111, 0b0101111]

ox = 32
oy = 16
hue = 0

rtc = RTC()

hub = hub75.Hub75(WIDTH, HEIGHT, stb_invert=False)
print("hub75: init")
hub.start()
print("hub75: start")
hub.clear()
print("hub75: clear")

set_hsv = hub.set_hsv
set_rgb = hub.set_rgb


def dot(x, y, h, s, v):
    set_hsv(x, y, h, s, v)
    set_hsv(x + 1, y, h, s, v)
    set_hsv(x, y + 1, h, s, v)
    set_hsv(x + 1, y + 1, h, s, v)


def shader_fg(x, y):
    """Shades the lit pixels of a digit"""
    h = ((x - ox) * (x - ox) + (y - oy) * (y - oy)) / MAX_DIST
    set_hsv(x, y, h + hue, 1.0, 1.0)


def shader_bg(x, y):
    """Shades the unlit pixels of a digit"""
    set_rgb(x, y, 10, 10, 10)


def draw_number(x, y, number, fg=None, bg=None, digit_width=8, digit_height=15, digit_spacing=2):
    """Draw a sequence of digits.

    Uses lines to draw digits like so:
     _      _  _       _      _   _   _
    | |  |  _| _| |_| |_  |_   | |_| |_|
    |_|  | |_  _|   |  _| |_|  | |_|   |

    Digits are bit-packed into DIGITS,
    each part corresponds to 1-bit:

        0b0000001 = Top
        0b0000010 = Top Left
        0b0000100 = Top Right
        0b0001000 = Middle
        0b0010000 = Bottom Left
        0b0100000 = Bottom Right
        0b1000000 = Bottom

    """
    v_line = int((digit_height - 3) / 2)
    h_line = digit_width - 2

    if fg is None:
        def fg(x, y):
            set_rgb(x, y, 255, 255, 255)

    if bg is None:
        def bg(x, y):
            pass

    for digit in number:
        if digit == " ":
            x += digit_spacing
            continue

        try:
            parts = DIGITS[ord(digit) - 48]
        except IndexError:
            x += digit_spacing
            continue

        shader = fg if parts & 1 else bg  # top
        for px in range(h_line):
            shader(x + px + 1, y)

        shader = fg if parts & 2 else bg  # top left
        for py in range(v_line):
            shader(x, y + py + 1)

        shader = fg if parts & 4 else bg  # top right
        for py in range(v_line):
            shader(x + h_line + 1, y + 1 + py)

        shader = fg if parts & 8 else bg  # middle
        for px in range(h_line):
            shader(x + px + 1, y + v_line + 1)

        shader = fg if parts & 16 else bg  # bottom left
        for py in range(v_line):
            shader(x, y + v_line + 2 + py)

        shader = fg if parts & 32 else bg  # bottom right
        for py in range(v_line):
            shader(x + h_line + 1, y + v_line + 2 + py)

        shader = fg if parts & 64 else bg  # bottom
        for px in range(h_line):
            shader(x + px + 1, y + v_line + v_line + 2)

        x += digit_width + digit_spacing

    return x, y


while True:
    t = ticks_ms()
    hue = t / 3000.0
    ox = int(32 * math.sin(t / 4000.0) + 32)
    oy = int(16 * math.sin((t + 5000) / 3000.0) + 16)
    hub.clear()

    year, month, day, wd, hour, minute, second, _ = rtc.datetime()
    hms = "{:02} {:02} {:02}".format(hour, minute, second)
    ymd = "{:04} {:02} {:02}".format(year, month, day)

    # Hour / Minute / Second
    draw_number(1, 1, hms, fg=shader_fg, bg=shader_bg)

    # Year / Month / Day
    draw_number(8, 20, ymd, fg=shader_fg, bg=shader_bg, digit_width=5, digit_height=7, digit_spacing=1)

    # Blinking dots
    dot_v = (math.sin(t / 1000.0 * math.pi * 2) + 1.0) / 2.0
    dot(20, 5, hue, 0.5, dot_v)
    dot(20, 10, hue, 0.5, dot_v)
    dot(42, 5, hue, 0.5, dot_v)
    dot(42, 10, hue, 0.5, dot_v)

    # hub.set_rgb(ox, oy, 255, 255, 255)

    hub.flip()
    sleep(1.0 / 60)
