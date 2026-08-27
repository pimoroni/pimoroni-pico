from pimoroni import RGBLED
from time import sleep

# Setup the RGB Led
led = RGBLED(18, 19, 20)

# Cycle through RGB

while True:
    led.set_rgb(255, 0, 0)
    sleep(1)
    led.set_rgb(0, 255, 0)
    sleep(1)
    led.set_rgb(0, 0, 255)
    sleep(1)
