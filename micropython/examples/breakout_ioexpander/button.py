import time
from pimoroni_i2c import PimoroniI2C
from breakout_ioexpander import BreakoutIOExpander

PINS_BREAKOUT_GARDEN = {"sda": 4, "scl": 5}
PINS_PICO_EXPLORER = {"sda": 20, "scl": 21}

ioe_button_pin = 14

i2c = PimoroniI2C(**PINS_BREAKOUT_GARDEN)
ioe = BreakoutIOExpander(i2c, address=0x18)

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
