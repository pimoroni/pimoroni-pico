import time
import math

from pimoroni_i2c import PimoroniI2C
from breakout_dotmatrix import BreakoutDotMatrix

PINS_BREAKOUT_GARDEN = {"sda": 4, "scl": 5}
PINS_PICO_EXPLORER = {"sda": 20, "scl": 21}

i2c = PimoroniI2C(**PINS_BREAKOUT_GARDEN)
display = BreakoutDotMatrix(i2c)

#        Left Image     Padding  Right Image    Padding
image = [0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0,
         0, 1, 0, 1, 0, 0, 0, 0, 1, 0, 1, 0, 1, 0, 0, 0,
         0, 1, 0, 1, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0,
         0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 1, 0, 0, 0,
         1, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0,
         0, 1, 1, 1, 0, 0, 0, 0, 1, 0, 1, 0, 1, 0, 0, 0,
         0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0]

image_height = 7
image_padding = 3  # 3 columns of padding
image_width = math.floor(len(image) / image_height)

arr = bytearray(image)

offset = 0
while True:
    display.set_image(arr, image_width, image_height, offset_x=offset, wr=True, on_level=1, padding=image_padding)
    display.show()
    time.sleep(0.5)
    offset = (offset + 1) % image_width
