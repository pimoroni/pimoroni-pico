import time
import math

from breakout_dotmatrix import BreakoutDotMatrix

display = BreakoutDotMatrix()

#        Left Image       Padding    Right Image      Padding
image = [0, 0, 0, 0, 0,   0, 0, 0,   0, 1, 0, 1, 0,   0, 0, 0,
         0, 1, 0, 1, 0,   0, 0, 0,   1, 0, 1, 0, 1,   0, 0, 0,
         0, 1, 0, 1, 0,   0, 0, 0,   0, 1, 0, 1, 0,   0, 0, 0,
         0, 0, 0, 0, 0,   0, 0, 0,   1, 0, 1, 0, 1,   0, 0, 0,
         1, 0, 0, 0, 1,   0, 0, 0,   0, 1, 0, 1, 0,   0, 0, 0,
         0, 1, 1, 1, 0,   0, 0, 0,   1, 0, 1, 0, 1,   0, 0, 0,
         0, 0, 0, 0, 0,   0, 0, 0,   0, 1, 0, 1, 0,   0, 0, 0]

image_height = 7
image_padding = 3 # 3 columns of padding
image_width = math.floor(len(image) / image_height)

arr = bytearray(image)

offset = 0
while True:
    display.set_image(arr, image_width, image_height, offset_x=offset, wr=True, on_level=1, padding=image_padding)
    display.show()
    time.sleep(0.5)
    offset = (offset + 1) % image_width