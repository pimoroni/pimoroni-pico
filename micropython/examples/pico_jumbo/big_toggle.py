"""
Wire a big toggle switch up to GP16 and ground and read the on/off state.

If you have an LED connected to GP15 (with a resistor) and ground the button will turn it on and off.
"""

import machine
import time

button = machine.Pin(16, machine.Pin.IN, machine.Pin.PULL_UP)
led_external = machine.Pin(15, machine.Pin.OUT)

while True:
    # as we're using a pull up, the logic state is reversed
    if button.value() == 0:
        print("Toggle on!")
        led_external.on()
        time.sleep(0.5)
    else:
        print("Toggle off!")
        led_external.off()
        time.sleep(0.5)
