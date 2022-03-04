# Blinky badger fun!

import badger2040
import time

badger = badger2040.Badger2040()

while True:
    badger.led(255)
    time.sleep(1)
    badger.led(0)
    time.sleep(1)
