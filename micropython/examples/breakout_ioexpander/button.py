import time
from breakout_ioexpander import BreakoutIOExpander

ioe_button_pin = 14

ioe = BreakoutIOExpander(0x18)

ioe.set_mode(ioe_button_pin, BreakoutIOExpander.PIN_IN_PU)

last_state = True

while True:
    state = ioe.input(ioe_button_pin)
    if state is not last_state:
        if state:
            print("Button has been released")
        else:
            print("Button has been pressed")
        last_state = state

    time.sleep(0.02)
