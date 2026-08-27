"""
Wire up a big LED to GP15 (with a resistor) and ground and make it blink!
"""

import machine
import time

led_external = machine.Pin(15, machine.Pin.OUT)

while True:
    led_external.toggle()
    time.sleep(1)
