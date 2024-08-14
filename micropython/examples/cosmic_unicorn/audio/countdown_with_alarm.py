from machine import Timer
from audio import WavPlayer
from cosmic import CosmicUnicorn
from picographics import PicoGraphics, DISPLAY_COSMIC_UNICORN as DISPLAY
import time

cu = CosmicUnicorn()
graphics = PicoGraphics(DISPLAY)

graphics.set_font("bitmap6")
WHITE = graphics.create_pen(255, 255, 255)
BLUE = graphics.create_pen(0, 0, 255)
CLEAR = graphics.create_pen(0, 0, 0)
RED = graphics.create_pen(255, 0, 0)
GREEN = graphics.create_pen(0, 255, 0)
cu.set_brightness(0.7)

audio = WavPlayer(0, 10, 11, 9, amp_enable=22)


class Countdown(object):
    def __init__(self):
        self.timer_running = False
        self.total_seconds = 0
        self.timer = None

    def process_input(self):
        if cu.is_pressed(CosmicUnicorn.SWITCH_VOLUME_UP):
            self.total_seconds += 1
        if cu.is_pressed(CosmicUnicorn.SWITCH_VOLUME_DOWN):
            if self.total_seconds > 0:
                self.total_seconds -= 1
        if cu.is_pressed(CosmicUnicorn.SWITCH_SLEEP):
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

        return "{}:{}".format(minutes, seconds)

    def draw(self):
        graphics.set_pen(graphics.create_pen(0, 0, 0))
        graphics.clear()

        graphics.set_pen(BLUE)
        graphics.circle(0, 0, 12)
        graphics.set_pen(GREEN)
        graphics.circle(25, 30, 5)
        graphics.set_pen(RED)
        graphics.circle(0, 32, 12)

        graphics.set_pen(CLEAR)
        graphics.rectangle(0, 11, CosmicUnicorn.WIDTH, 9)

        graphics.set_pen(WHITE)
        graphics.text(self.display_time(), 4, 12, -1, 1)
        cu.update(graphics)

    def start_timer(self):
        if not self.timer_running:
            self.timer = Timer(mode=Timer.PERIODIC, period=1000, callback=self.countdown)
            self.timer_running = True

    def reset(self):
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
