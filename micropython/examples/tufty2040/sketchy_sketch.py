import time
from pimoroni import Button
from picographics import PicoGraphics, DISPLAY_TUFTY_2040, PEN_RGB332

display = PicoGraphics(display=DISPLAY_TUFTY_2040, pen_type=PEN_RGB332, rotate=180)

# Tufty constants
A = 7
B = 8
C = 15
UP = 22
DOWN = 6
LED = 25

WIDTH, HEIGHT = 320, 240
display.set_backlight(1.0)

# Buttons
button_a = Button(7, invert=False)
button_b = Button(8, invert=False)
button_c = Button(9, invert=False)
button_up = Button(22, invert=False)
button_down = Button(6, invert=False)


def draw_area():
    display.set_pen(display.create_pen(200, 0, 0))
    display.clear()

    display.set_pen(display.create_pen(255, 215, 0))
    display.text("Sketchy-Sketch", 90, 5, 0, 2)

    # draw main surface
    display.set_pen(display.create_pen(205, 205, 205))
    display.rectangle(25, 25, 270, 180)

    # draw knobs
    display.set_pen(display.create_pen(150, 50, 50))
    display.circle(25 + 5, 225 + 5, 20)
    display.circle(295 + 5, 225 + 5, 20)
    display.set_pen(display.create_pen(255, 255, 255))
    display.circle(25 - 1, 225 - 1, 16)
    display.circle(295 - 1, 225 - 1, 16)
    display.set_pen(display.create_pen(155, 155, 155))
    display.circle(25 + 1, 225 + 1, 16)
    display.circle(295 + 1, 225 + 1, 16)
    display.set_pen(display.create_pen(205, 205, 205))
    display.circle(25, 225, 15)
    display.circle(295, 225, 15)


# start position for drawing cursor
position_x = 160
position_y = 110

# draw the sketchy sketch
draw_area()

while True:
    # check for user input and update cursor position as needed
    if button_c.raw() and position_x < 290:
        position_x += 1

    if button_a.raw() and position_x > 30:
        position_x -= 1

    if button_up.raw() and position_y > 30:
        position_y -= 1

    if button_down.raw() and position_y < 200:
        position_y += 1

    if button_b.raw():
        draw_area()

    # draw the line
    display.set_pen(display.create_pen(50, 50, 50))
    display.circle(position_x, position_y, 2)

    # update the screen contents
    display.update()

    time.sleep(0.005)
