# This example shows you a simple, non-interrupt way of reading Pico Enviro's buttons with a loop that checks to see if buttons are pressed.

import time
from pimoroni import Button
from picographics import PicoGraphics, DISPLAY_ENVIRO_PLUS

display = PicoGraphics(display=DISPLAY_ENVIRO_PLUS)

display.set_backlight(1.0)
display.set_font("bitmap8")

button_a = Button(12, invert=True)
button_b = Button(13, invert=True)
button_x = Button(14, invert=True)
button_y = Button(15, invert=True)


WHITE = display.create_pen(255, 255, 255)
BLACK = display.create_pen(0, 0, 0)
TEAL = display.create_pen(0, 255, 255)
MAGENTA = display.create_pen(255, 0, 255)
YELLOW = display.create_pen(255, 255, 0)
RED = display.create_pen(255, 0, 0)
GREEN = display.create_pen(0, 255, 0)
BLUE = display.create_pen(0, 0, 255)

WIDTH, HEIGHT = display.get_bounds()

while True:
    if button_a.is_pressed:                               # if a button press is detected then...
        display.set_pen(BLACK)                            # set pen to black
        display.clear()                                   # clear display to the pen colour
        display.set_pen(WHITE)                            # change the pen colour
        display.text("Button A pressed", 10, 10, WIDTH - 10, 3)  # display some text on the screen
        display.update()                                  # update the display
        time.sleep(1)                                    # pause for a sec

    elif button_b.is_pressed:
        display.set_pen(BLACK)
        display.clear()
        display.set_pen(TEAL)
        display.text("Button B pressed", 10, 10, WIDTH - 10, 3)
        display.update()
        time.sleep(1)

    elif button_x.is_pressed:
        display.set_pen(BLACK)
        display.clear()
        display.set_pen(MAGENTA)
        display.text("Button X pressed", 10, 10, WIDTH - 10, 3)
        display.update()
        time.sleep(1)

    elif button_y.is_pressed:
        display.set_pen(BLACK)
        display.clear()
        display.set_pen(YELLOW)
        display.text("Button Y pressed", 10, 10, WIDTH - 10, 3)
        display.update()
        time.sleep(1)

    else:
        display.set_pen(BLACK)
        display.clear()
        display.set_pen(RED)
        display.text("Press any button!", 10, 10, WIDTH, 3)
        display.update()

    time.sleep(0.1)  # this number is how frequently the pico checks for button presses
