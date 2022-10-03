# This example allows you to test Inky Frame's buttons
# It does not update the screen.

from pimoroni import ShiftRegister
from machine import Pin
import time


# Inky Frame uses a shift register to read the buttons
SR_CLOCK = 8
SR_LATCH = 9
SR_OUT = 10

sr = ShiftRegister(SR_CLOCK, SR_LATCH, SR_OUT)


# Simple class to debounce button input and handle LED
class Button:
    def __init__(self, idx, led, debounce=50):
        self.led = Pin(led, Pin.OUT)  # LEDs are just regular IOs
        self.led.on()
        self._idx = idx
        self._debounce_time = debounce
        self._changed = time.ticks_ms()
        self._last_value = None

    def debounced(self):
        return time.ticks_ms() - self._changed > self._debounce_time

    def get(self, sr):
        value = sr[self._idx]
        if value != self._last_value and self.debounced():
            self._last_value = value
            self._changed = time.ticks_ms()
            return value


button_a = Button(7, 11)
button_b = Button(6, 12)
button_c = Button(5, 13)
button_d = Button(4, 14)
button_e = Button(3, 15)


while True:
    sr.read()

    if button_a.get(sr):
        button_a.led.toggle()
    if button_b.get(sr):
        button_b.led.toggle()
    if button_c.get(sr):
        button_c.led.toggle()
    if button_d.get(sr):
        button_d.led.toggle()
    if button_e.get(sr):
        button_e.led.toggle()

    time.sleep(1.0 / 60)  # Poll 60 times/second
