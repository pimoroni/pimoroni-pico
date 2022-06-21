# This example shows you a simple, non-interrupt way of reading Tufty 2040's buttons with a loop that checks to see if buttons are pressed.

import time
from pimoroni import Button
from picographics import PicoGraphics, DISPLAY_TUFTY_2040

display = PicoGraphics(display=DISPLAY_TUFTY_2040)

display.set_backlight(1.0)
display.set_font("bitmap8")

button_a = Button(7, invert=False)
button_b = Button(8, invert=False)
button_c = Button(9, invert=False)
button_up = Button(22, invert=False)
button_down = Button(6, invert=False)
button_boot = Button(23, invert=True)

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

    elif button_c.is_pressed:
        display.set_pen(BLACK)
        display.clear()
        display.set_pen(MAGENTA)
        display.text("Button C pressed", 10, 10, WIDTH - 10, 3)
        display.update()
        time.sleep(1)

    elif button_up.is_pressed:
        display.set_pen(BLACK)
        display.clear()
        display.set_pen(YELLOW)
        display.text("Button UP pressed", 10, 10, WIDTH - 10, 3)
        display.update()
        time.sleep(1)

    elif button_down.is_pressed:
        display.set_pen(BLACK)
        display.clear()
        display.set_pen(GREEN)
        display.text("Button DOWN pressed", 10, 10, WIDTH - 10, 3)
        display.update()
        time.sleep(1)

    elif button_boot.is_pressed:
        display.set_pen(BLACK)
        display.clear()
        display.set_pen(BLUE)
        display.text("Button BOOT/USR pressed", 10, 10, WIDTH - 10, 3)
        display.update()
        time.sleep(1)

    else:
        display.set_pen(BLACK)
        display.clear()
        display.set_pen(RED)
        display.text("Press any button!", 10, 10, WIDTH, 3)
        display.update()

    time.sleep(0.1)  # this number is how frequently Tufty checks for button presses
