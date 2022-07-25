# Control the brightness of Inky Frame's LEDs using PWM
# More about PWM / frequency / duty cycle here:
# https://projects.raspberrypi.org/en/projects/getting-started-with-the-pico/7

from machine import Pin, PWM
from time import sleep

led_activity = PWM(Pin(6))

# we're just PWMing the activity LED in this demo
# but here are the pins for the other LEDs for reference
led_connect = PWM(Pin(7))
led_a = PWM(Pin(11))
led_b = PWM(Pin(12))
led_c = PWM(Pin(13))
led_d = PWM(Pin(14))
led_e = PWM(Pin(15))

led_activity.freq(1000)

while True:
    for duty in range(65025):
        led_activity.duty_u16(duty)
        sleep(0.0001)
    for duty in range(65025, 0, -1):
        led_activity.duty_u16(duty)
        sleep(0.0001)
