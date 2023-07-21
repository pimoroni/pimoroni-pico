# Simple demo of how to use the RGB LED and read the button on Tiny 2040.

from pimoroni import Button, RGBLED
import time

led = RGBLED(18, 19, 20)
button_boot = Button(23)

# start with the LED off
led.set_rgb(0, 0, 0)

# flash the LED red, green and blue
led.set_rgb(255, 0, 0)
time.sleep(0.5)
led.set_rgb(0, 255, 0)
time.sleep(0.5)
led.set_rgb(0, 0, 255)
time.sleep(0.5)

print("Press the button!")

while True:
    # flash the LED white when the button is pressed
    if button_boot.read():
        print("Button pressed!")
        led.set_rgb(255, 255, 255)
        time.sleep(0.5)
    led.set_rgb(0, 0, 0)
