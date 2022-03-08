# This example blinks the user LED on Pimoroni Pico Lipo whilst the BOOT button is pressed.
# The button is active low, which is why we're using an if not statement!

from machine import Pin
import time

led = Pin(25, Pin.OUT)
button = Pin(23, Pin.IN, Pin.PULL_DOWN)

while True:
    if not button.value():
        led.toggle()
        time.sleep(0.5)
