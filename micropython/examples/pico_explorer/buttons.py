# This example shows you a simple, non-interrupt way of reading Pico Explorer's buttons with a loop that checks to see if buttons are pressed.

import utime
import st7789
import picoexplorer
from pimoroni import Button


display = st7789.ST7789(st7789.DISPLAY_PICO_EXPLORER, rotate=0)
display.set_palette_mode(st7789.PALETTE_USER)
display.set_backlight(1.0)

button_a = Button(picoexplorer.BUTTON_A)
button_b = Button(picoexplorer.BUTTON_B)
button_x = Button(picoexplorer.BUTTON_X)
button_y = Button(picoexplorer.BUTTON_Y)

WHITE = display.create_pen(255, 255, 255)
BLACK = display.create_pen(0, 0, 0)
TEAL = display.create_pen(0, 255, 255)
MAGENTA = display.create_pen(255, 0, 255)
YELLOW = display.create_pen(255, 255, 0)
RED = display.create_pen(255, 0, 0)


while True:
    if button_a.is_pressed:                               # if a button press is detected then...
        display.set_pen(BLACK)                            # set pen to black
        display.clear()                                   # clear display to the pen colour
        display.set_pen(WHITE)                            # change the pen colour
        display.text("Button A pressed", 10, 10, 240, 4)  # display some text on the screen
        display.update()                                  # update the display
        utime.sleep(1)                                    # pause for a sec

    elif button_b.is_pressed:
        display.set_pen(BLACK)
        display.clear()
        display.set_pen(TEAL)
        display.text("Button B pressed", 10, 10, 240, 4)
        display.update()
        utime.sleep(1)

    elif button_x.is_pressed:
        display.set_pen(BLACK)
        display.clear()
        display.set_pen(MAGENTA)
        display.text("Button X pressed", 10, 10, 240, 4)
        display.update()
        utime.sleep(1)

    elif button_y.is_pressed:
        display.set_pen(BLACK)
        display.clear()
        display.set_pen(YELLOW)
        display.text("Button Y pressed", 10, 10, 240, 4)
        display.update()
        utime.sleep(1)

    else:
        display.set_pen(BLACK)
        display.clear()
        display.set_pen(RED)
        display.text("Press any button!", 10, 10, 240, 4)
        display.update()

    utime.sleep(0.1)  # this number is how frequently the Pico checks for button presses
