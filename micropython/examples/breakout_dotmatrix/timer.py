import sys
import time

from breakout_dotmatrix import BreakoutDotMatrix

address = 0x61
start_time = time.time()

display = BreakoutDotMatrix(address=address)
display.clear()
display.show()

while True:
    second = str((time.time() - start_time) % 60)
    print(second)
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