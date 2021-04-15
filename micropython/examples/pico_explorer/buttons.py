# This example shows you a simple, non-interrupt way of reading Pico Explorer's buttons with a loop that checks to see if buttons are pressed.

import picoexplorer as display
import utime

# Initialise display with a bytearray display buffer
buf = bytearray(display.get_width() * display.get_height() * 2)
display.init(buf)


# sets up a handy function we can call to clear the screen
def clear():
    display.set_pen(0, 0, 0)
    display.clear()
    display.update()


while True:
    if display.is_pressed(display.BUTTON_A):              # if a button press is detected then...
        clear()                                           # clear to black
        display.set_pen(255, 255, 255)                    # change the pen colour
        display.text("Button A pressed", 10, 10, 240, 4)  # display some text on the screen
        display.update()                                  # update the display
        utime.sleep(1)                                    # pause for a sec
        clear()                                           # clear to black again
    elif display.is_pressed(display.BUTTON_B):
        clear()
        display.set_pen(0, 255, 255)
        display.text("Button B pressed", 10, 10, 240, 4)
        display.update()
        utime.sleep(1)
        clear()
    elif display.is_pressed(display.BUTTON_X):
        clear()
        display.set_pen(255, 0, 255)
        display.text("Button X pressed", 10, 10, 240, 4)
        display.update()
        utime.sleep(1)
        clear()
    elif display.is_pressed(display.BUTTON_Y):
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
