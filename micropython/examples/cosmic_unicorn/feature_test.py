import time
import math
from cosmic import CosmicUnicorn
from picographics import PicoGraphics, DISPLAY_COSMIC_UNICORN as DISPLAY

'''
Displays some text, gradients and colours and demonstrates button use.

You can adjust the brightness with LUX + and -.
'''

cu = CosmicUnicorn()
graphics = PicoGraphics(DISPLAY)

width = CosmicUnicorn.WIDTH
height = CosmicUnicorn.HEIGHT


def gradient(r, g, b):
    for y in range(0, height):
        for x in range(0, width):
            graphics.set_pen(graphics.create_pen(int((r * x) / 32), int((g * x) / 32), int((b * x) / 32)))
            graphics.pixel(x, y)


def grid(r, g, b):
    for y in range(0, height):
        for x in range(0, width):
            if (x + y) % 2 == 0:
                graphics.set_pen(graphics.create_pen(r, g, b))
            else:
                graphics.set_pen(0)
            graphics.pixel(x, y)


def outline_text(text):
    ms = time.ticks_ms()

    graphics.set_font("bitmap8")
    v = int((math.sin(ms / 100.0) + 1.0) * 127.0)
    w = graphics.measure_text(text, 1)

    x = int(32 / 2 - w / 2 + 1)
    y = 12

    graphics.set_pen(0)
    graphics.text(text, x - 1, y - 1, -1, 1)
    graphics.text(text, x, y - 1, -1, 1)
    graphics.text(text, x + 1, y - 1, -1, 1)
    graphics.text(text, x - 1, y, -1, 1)
    graphics.text(text, x + 1, y, -1, 1)
    graphics.text(text, x - 1, y + 1, -1, 1)
    graphics.text(text, x, y + 1, -1, 1)
    graphics.text(text, x + 1, y + 1, -1, 1)

    graphics.set_pen(graphics.create_pen(v, v, v))
    graphics.text(text, x, y, -1, 1)


cu.set_brightness(0.5)

while True:

    time_ms = time.ticks_ms()
    test = (time_ms // 1000) % 5

    if cu.is_pressed(CosmicUnicorn.SWITCH_BRIGHTNESS_UP):
        cu.adjust_brightness(+0.01)

    if cu.is_pressed(CosmicUnicorn.SWITCH_BRIGHTNESS_DOWN):
        cu.adjust_brightness(-0.01)

    graphics.set_pen(graphics.create_pen(0, 0, 0))
    graphics.clear()

    if test == 0:
        print("grid pattern")
        grid(255, 255, 255)
    elif test == 1:
        print("red gradient")
        gradient(255, 0, 0)
    elif test == 2:
        print("green gradient")
        gradient(0, 255, 0)
    elif test == 3:
        print("blue gradient")
        gradient(0, 0, 255)
    elif test == 4:
        print("white gradient")
        gradient(255, 255, 255)

    text = ""

    if cu.is_pressed(CosmicUnicorn.SWITCH_A):
        text = "Button A"

    if cu.is_pressed(CosmicUnicorn.SWITCH_B):
        text = "Button B"

    if cu.is_pressed(CosmicUnicorn.SWITCH_C):
        text = "Button C"

    if cu.is_pressed(CosmicUnicorn.SWITCH_D):
        text = "Button D"

    if cu.is_pressed(CosmicUnicorn.SWITCH_VOLUME_UP):
        text = "Louder!"

    if cu.is_pressed(CosmicUnicorn.SWITCH_VOLUME_DOWN):
        text = "Quieter"

    if cu.is_pressed(CosmicUnicorn.SWITCH_BRIGHTNESS_UP):
        text = "Brighter!"

    if cu.is_pressed(CosmicUnicorn.SWITCH_BRIGHTNESS_DOWN):
        text = "Darker"

    if cu.is_pressed(CosmicUnicorn.SWITCH_SLEEP):
        text = "Zzz... zzz..."

    outline_text(text)

    cu.update(graphics)
