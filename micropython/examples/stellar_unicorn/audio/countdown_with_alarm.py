# Countdown Timer with Alarm
# i2s Audio Example
# Use VOL +/- to increase/decrease the amount of time
# Use the Sleep ZZZ button to start the countdown

from machine import Pin, Timer
from audio import WavPlayer
from stellar import StellarUnicorn
from picographics import PicoGraphics, DISPLAY_STELLAR_UNICORN as DISPLAY
import time

su = StellarUnicorn()
graphics = PicoGraphics(DISPLAY)

amp_enable = Pin(22, Pin.OUT)
amp_enable.on()

graphics.set_font("bitmap6")
WHITE = graphics.create_pen(255, 255, 255)
BLUE = graphics.create_pen(0, 0, 255)
CLEAR = graphics.create_pen(0, 0, 0)
RED = graphics.create_pen(255, 0, 0)
GREEN = graphics.create_pen(0, 255, 0)
su.set_brightness(0.5)

audio = WavPlayer(0, 10, 11, 9)


class Countdown(object):
    def __init__(self):
        self.timer_running = False
        self.total_seconds = 0
        self.timer = None

    def process_input(self):
        if su.is_pressed(StellarUnicorn.SWITCH_VOLUME_UP):
            self.total_seconds += 1
        if su.is_pressed(StellarUnicorn.SWITCH_VOLUME_DOWN):
            if self.total_seconds > 0:
                self.total_seconds -= 1
        if su.is_pressed(StellarUnicorn.SWITCH_SLEEP):
            self.start_timer()

    def display_time(self):
        seconds = self.total_seconds % (24 * 3600)
        seconds %= 3600
        minutes = seconds // 60
        seconds %= 60

        # Add leading zeros to the minutes and seconds
        if len(str(minutes)) == 1:
            minutes = "0{}".format(minutes)
        if len(str(seconds)) == 1:
            seconds = "0{}".format(seconds)

        return "{}\n{}".format(minutes, seconds)

    def draw(self):
        graphics.set_pen(graphics.create_pen(0, 0, 0))
        graphics.clear()

        graphics.set_pen(RED)
        graphics.rectangle(0, 0, 16, 16)

        graphics.set_pen(CLEAR)
        graphics.rectangle(2, 2, StellarUnicorn.WIDTH - 4, 12)

        graphics.set_pen(WHITE)
        graphics.text(self.display_time(), 2, 1, -1, 1)
        su.update(graphics)

    def start_timer(self):
        if not self.timer_running:
            self.timer = Timer(mode=Timer.PERIODIC, period=1000, callback=self.countdown)
            self.timer_running = True

    def reset(self):
        time.sleep(0.2)
        self.timer.deinit()
        self.timer_running = False

    def countdown(self, arg):

        if self.total_seconds == 0:
            audio.play_wav("doorbell.wav", False)
            self.reset()
        else:
            self.total_seconds -= 1


count = Countdown()

while 1:
    count.process_input()
    count.draw()
    time.sleep(0.07)
