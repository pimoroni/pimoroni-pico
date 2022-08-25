# This example shows you a simple, non-interrupt way of reading Pico Explorer's buttons with a loop that checks to see if buttons are pressed.

import time
from pimoroni import Button
from picographics import PicoGraphics, DISPLAY_PICO_EXPLORER, PEN_P4

# We're only using a few colours so we can use a 4 bit/16 colour palette and save RAM!
display = PicoGraphics(display=DISPLAY_PICO_EXPLORER, pen_type=PEN_P4)

button_a = Button(12)
button_b = Button(13)
button_x = Button(14)
button_y = Button(15)

WHITE = display.create_pen(255, 255, 255)
BLACK = display.create_pen(0, 0, 0)
CYAN = display.create_pen(0, 255, 255)
MAGENTA = display.create_pen(255, 0, 255)
YELLOW = display.create_pen(255, 255, 0)
GREEN = display.create_pen(0, 255, 0)


# sets up a handy function we can call to clear the screen
def clear():
    display.set_pen(BLACK)
    display.clear()
    display.update()


# set up
clear()
display.set_font("bitmap8")

while True:
    if button_a.read():                                   # if a button press is detected then...
        clear()                                           # clear to black
        display.set_pen(WHITE)                            # change the pen colour
        display.text("Button A pressed", 10, 10, 240, 4)  # display some text on the screen
        display.update()                                  # update the display
        time.sleep(1)                                     # pause for a sec
        clear()                                           # clear to black again
    elif button_b.read():
        clear()
        display.set_pen(CYAN)
        display.text("Button B pressed", 10, 10, 240, 4)
        display.update()
        time.sleep(1)
        clear()
    elif button_x.read():
        clear()
        display.set_pen(MAGENTA)
        display.text("Button X pressed", 10, 10, 240, 4)
        display.update()
        time.sleep(1)
        clear()
    elif button_y.read():
        clear()
        display.set_pen(YELLOW)
        display.text("Button Y pressed", 10, 10, 240, 4)
        display.update()
        time.sleep(1)
        clear()
    else:
        display.set_pen(GREEN)
        display.text("Press any button!", 10, 10, 240, 4)
        display.update()
    time.sleep(0.1)  # this number is how frequently the Pico checks for button presses
