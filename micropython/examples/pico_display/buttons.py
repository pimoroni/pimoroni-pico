# This example shows you a simple, non-interrupt way of reading Pico Display's buttons with a loop that checks to see if buttons are pressed.

import st7789
import utime
from pimoroni import Button


display = st7789.ST7789(st7789.DISPLAY_PICO_DISPLAY, rotate=0)
display.set_backlight(0.5)

WIDTH, HEIGHT = display.get_bounds()

button_a = Button(12)
button_b = Button(13)
button_x = Button(14)
button_y = Button(15)


# sets up a handy function we can call to clear the screen
def clear():
    display.set_pen(0, 0, 0)
    display.clear()
    display.update()


while True:
    if button_a.read():                                   # if a button press is detected then...
        clear()                                           # clear to black
        display.set_pen(255, 255, 255)                    # change the pen colour
        display.text("Button A pressed", 10, 10, 240, 4)  # display some text on the screen
        display.update()                                  # update the display
        utime.sleep(1)                                    # pause for a sec
        clear()                                           # clear to black again
    elif button_b.read():
        clear()
        display.set_pen(0, 255, 255)
        display.text("Button B pressed", 10, 10, 240, 4)
        display.update()
        utime.sleep(1)
        clear()
    elif button_x.read():
        clear()
        display.set_pen(255, 0, 255)
        display.text("Button X pressed", 10, 10, 240, 4)
        display.update()
        utime.sleep(1)
        clear()
    elif button_y.read():
        clear()
        display.set_pen(255, 255, 0)
        display.text("Button Y pressed", 10, 10, 240, 4)
        display.update()
        utime.sleep(1)
        clear()
    else:
        display.set_pen(255, 0, 0)
        display.text("Press any button!", 10, 10, 240, 4)
        display.update()
    utime.sleep(0.1)  # this number is how frequently the Pico checks for button presses
