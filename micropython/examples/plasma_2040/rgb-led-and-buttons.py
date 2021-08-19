import time

# Import plasma2040
from plasma import plasma2040

# Import helpers for RGB LEDs and Buttons
from pimoroni import RGBLED, Button

led = RGBLED(plasma2040.LED_R, plasma2040.LED_G, plasma2040.LED_B)
led.set_rgb(0, 0, 0)

user_sw = Button(plasma2040.USER_SW)
button_a = Button(plasma2040.BUTTON_A)
button_b = Button(plasma2040.BUTTON_B)

while True:
    if user_sw.read():
        print("Pressed User SW - {}".format(time.ticks_ms()))
        led.set_rgb(255, 0, 0)
    if button_a.read():
        print("Pressed A - {}".format(time.ticks_ms()))
        led.set_rgb(0, 255, 0)
    if button_b.read():
        print("Pressed B - {}".format(time.ticks_ms()))
        led.set_rgb(0, 0, 255)
