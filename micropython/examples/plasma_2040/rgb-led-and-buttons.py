import time

# Import pin constants from plasma
from plasma import PIN_LED_R, PIN_LED_G, PIN_LED_B, PIN_USER_SW, PIN_BUTTON_A, PIN_BUTTON_B

# Import helpers for RGB LEDs and Buttons
from pimoroni import RGBLED, Button

led = RGBLED(PIN_LED_R, PIN_LED_G, PIN_LED_B)
led.set_rgb(0, 0, 0)

user_sw = Button(PIN_USER_SW)
button_a = Button(PIN_BUTTON_A)
button_b = Button(PIN_BUTTON_B)

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
