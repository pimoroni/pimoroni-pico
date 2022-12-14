# Blinky badger fun!

import badger2040
import time

badger = badger2040.Badger2040()

while True:
    # increase badger.led brightness from 0 to 255 and back down to zero
    for i in range(0, 255):
        badger.led(i)
        time.sleep_ms(10)
    for i in range(255, 0, -1):
        badger.led(i)
        time.sleep_ms(10)
