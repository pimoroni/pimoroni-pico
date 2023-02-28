# This example shows you a simple, non-interrupt way of reading Inky Frame's buttons with a loop that checks to see if buttons are pressed.

from pimoroni import ShiftRegister
# from picographics import PicoGraphics, DISPLAY_INKY_FRAME as DISPLAY      # 5.7"
# from picographics import PicoGraphics, DISPLAY_INKY_FRAME_4 as DISPLAY  # 4.0"
from picographics import PicoGraphics, DISPLAY_INKY_FRAME_7 as DISPLAY  # 7.3"
from machine import Pin

display = PicoGraphics(display=DISPLAY)

display.set_font("bitmap8")

# Inky Frame uses a shift register to read the buttons
SR_CLOCK = 8
SR_LATCH = 9
SR_OUT = 10

sr = ShiftRegister(SR_CLOCK, SR_LATCH, SR_OUT)

# set up the button LEDs
button_a_led = Pin(11, Pin.OUT)
button_b_led = Pin(12, Pin.OUT)
button_c_led = Pin(13, Pin.OUT)
button_d_led = Pin(14, Pin.OUT)
button_e_led = Pin(15, Pin.OUT)


# a handy function we can call to clear the screen
# display.set_pen(1) is white and display.set_pen(0) is black
def clear():
    display.set_pen(1)
    display.clear()


# set up
clear()
display.set_pen(0)
display.text("Press any button!", 10, 10, scale=4)
display.update()

while True:
    button_a_led.off()
    button_b_led.off()
    button_c_led.off()
    button_d_led.off()
    button_e_led.off()

    # read the shift register
    # we can tell which button has been pressed by checking if a specific bit is 0 or 1
    result = sr.read()
    button_a = sr[7]
    button_b = sr[6]
    button_c = sr[5]
    button_d = sr[4]
    button_e = sr[3]

    if button_a == 1:                                      # if a button press is detected then...
        button_a_led.on()
        clear()                                            # clear to white
        display.set_pen(4)                                 # change the pen colour
        display.text("Button A pressed", 10, 10, scale=4)  # display some text on the screen
        display.update()                                   # update the display
    elif button_b == 1:
        button_b_led.on()
        clear()
        display.set_pen(6)
        display.text("Button B pressed", 10, 50, scale=4)
        display.update()
    elif button_c == 1:
        button_c_led.on()
        clear()
        display.set_pen(5)
        display.text("Button C pressed", 10, 90, scale=4)
        display.update()
    elif button_d == 1:
        button_d_led.on()
        clear()
        display.set_pen(2)
        display.text("Button D pressed", 10, 130, scale=4)
        display.update()
    elif button_e == 1:
        button_e_led.on()
        clear()
        display.set_pen(3)
        display.text("Button E pressed", 10, 170, scale=4)
        display.update()
