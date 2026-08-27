import time
from machine import Pin

# Setup the LED pin.
led = Pin(25, Pin.OUT)

# Blink the LED!
while True:

    led.value(1)
    time.sleep(1)

    led.value(0)
    time.sleep(1)
