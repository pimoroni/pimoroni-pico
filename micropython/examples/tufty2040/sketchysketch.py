import time
from pimoroni import Button
from picographics import PicoGraphics, DISPLAY_TUFTY_2040

display = PicoGraphics(display=DISPLAY_TUFTY_2040)

# Tufty constants
A = 7
B = 8
C = 15
UP = 22
DOWN = 6
LED = 25

WIDTH, HEIGHT = display.get_bounds()
display.set_backlight(1.0)

# Buttons
button_a = Button(7, invert=False)
button_b = Button(8, invert=False)
button_c = Button(9, invert=False)
button_up = Button(22, invert=False)
button_down = Button(6, invert=False)

# Pens
WHITE = display.create_pen(255, 255, 255)
BLACK = display.create_pen(0, 0, 0)
RED = display.create_pen(200, 0, 0)
YELLOW = display.create_pen(255, 215, 0)


def draw_area():
    display.set_pen(RED)
    display.clear()

    display.set_pen(YELLOW)
    display.text("Sketchy-Sketch", 90, 5, 0, 2)
    display.set_pen(WHITE)
    display.circle(55, 215, 15)
    display.circle(260, 215, 15)
    display.rectangle(10, 25, 300, 170)
    display.update()


position_x = 15
position_y = 30

last_x = 15
last_y = 30

draw_area()

while True:
    display.set_pen(0)

    if button_c.raw():
        if position_x < 308:
            last_x = position_x
            last_y = position_y
            position_x += 2

    if button_a.raw():
        if position_x > 12:
            last_x = position_x
            last_y = position_y
            position_x -= 2

    if button_up.raw():
        if position_y > 26:
            last_x = position_x
            last_y = position_y
            position_y -= 2

    if button_down.raw():
        if position_y < 193:
            last_x = position_x
            last_y = position_y
            position_y += 2

    if button_b.raw():
        draw_area()
        position_x = 15
        position_y = 30
        last_x = 15
        last_y = 30

    display.line(last_x, last_y, position_x, position_y)
    display.update()
    time.sleep(0.01)
