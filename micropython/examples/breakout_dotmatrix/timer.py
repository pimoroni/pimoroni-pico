import time

from pimoroni_i2c import PimoroniI2C
from breakout_dotmatrix import BreakoutDotMatrix

PINS_BREAKOUT_GARDEN = {"sda": 4, "scl": 5}
PINS_PICO_EXPLORER = {"sda": 20, "scl": 21}

address = 0x61
start_time = time.time()

i2c = PimoroniI2C(**PINS_BREAKOUT_GARDEN)
display = BreakoutDotMatrix(i2c, address=address)
display.clear()
display.show()

while True:
    second = str((time.time() - start_time) % 60)
    if len(second) == 1:
        left = '0'
        right = second[0]
    else:
        left = second[0]
        right = second[1]

    display.set_character(0, ord(left))
    display.set_character(5, ord(right))
    display.show()
    time.sleep(1.0 / 60)
