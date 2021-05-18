import time
import math
import random

from pimoroni_i2c import PimoroniI2C
from breakout_dotmatrix import BreakoutDotMatrix

PINS_BREAKOUT_GARDEN = {"sda": 4, "scl": 5}
PINS_PICO_EXPLORER = {"sda": 20, "scl": 21}

i2c = PimoroniI2C(**PINS_BREAKOUT_GARDEN)
display = BreakoutDotMatrix(i2c)
width = display.WIDTH
height = display.HEIGHT

values = [0, 0, 0, 0, 0]
start_time = time.time()


while True:
    # Add a new random value to our list and prune the list to visible values
    values.insert(0, random.randint(0, height))
    values = values[:width]

    # Animate a value from 0 to height + 1
    value = (math.sin((time.time() - start_time) * math.pi) + 1) / 2.0
    value *= height + 1
    value = math.floor(value)

    for y in range(height):
        y = height - 1 - y

        for x in range(width // 2):
            # Left
            display.set_pixel(x, y, value <= y)

            # Right
            display.set_pixel(x + (width // 2), y, values[x] <= y)

    display.show()
    time.sleep(1.0 / height)
