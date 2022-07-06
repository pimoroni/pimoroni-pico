# This example shows you a simple, non-interrupt way of reading Pico Inky Pack's buttons with a loop that checks to see if buttons are pressed.

import time
from pimoroni import Button
from picographics import PicoGraphics, DISPLAY_INKY_PACK

display = PicoGraphics(display=DISPLAY_INKY_PACK)

# you can change the update speed here!
# it goes from 0 (slowest) to 3 (fastest)
display.set_update_speed(2)

display.set_font("bitmap8")

button_a = Button(12)
button_b = Button(13)
button_c = Button(14)


# a handy function we can call to clear the screen
# display.set_pen(15) is white and display.set_pen(0) is black
def clear():
    display.set_pen(15)
    display.clear()


# set up
clear()
display.set_pen(0)
display.text("Press any button!", 10, 10, 240, 3)
display.update()
time.sleep(0.5)

while True:
    if button_a.read():                                   # if a button press is detected then...
        clear()                                           # clear to white
        display.set_pen(0)                                # change the pen colour
        display.text("Button A pressed", 10, 10, 240, 3)  # display some text on the screen
        display.update()                                  # update the display
        time.sleep(0.5)
    elif button_b.read():
        clear()
        display.set_pen(0)
        display.text("Button B pressed", 10, 50, 240, 3)
        display.update()
        time.sleep(0.5)
    elif button_c.read():
        clear()
        display.set_pen(0)
        display.text("Button C pressed", 10, 90, 240, 3)
        display.update()
        time.sleep(0.5)
    time.sleep(0.1)  # this number is how frequently the Pico checks for button presses
