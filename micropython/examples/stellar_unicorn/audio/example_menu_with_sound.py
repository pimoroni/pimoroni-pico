# Example Menu with Sound
# i2s Audio Example
# Use Brightness +/- to move up and down
# Press Sleep to play the selected sound

from audio import WavPlayer
from stellar import StellarUnicorn
from picographics import PicoGraphics, DISPLAY_STELLAR_UNICORN as DISPLAY
from time import sleep

su = StellarUnicorn()
graphics = PicoGraphics(DISPLAY)

audio = WavPlayer(0, 10, 11, 9, amp_enable=22)

WHITE = graphics.create_pen(255, 255, 255)
RED = graphics.create_pen(255, 0, 0)
GREEN = graphics.create_pen(0, 200, 200)


class Menu(object):

    def __init__(self):
        self.items = ["1", "2", "3"]
        self.selected = 0

    # A function to draw only the menu elements.
    # Helps to keep our main  function tidy!
    def draw_menu(self):
        graphics.set_pen(GREEN)

        graphics.circle(0, 0, 3)
        graphics.circle(16, 5, 4)
        graphics.circle(3, 16, 3)

        graphics.set_pen(WHITE)

        for item in range(len(self.items)):
            if self.selected == item:
                graphics.set_pen(WHITE)
                graphics.text(self.items[self.selected], 6, 4, 31, 1)

    # Make changes based on the currently selected menu item
    def process_selected(self):
        if self.selected == 0:
            audio.play_wav("Pew1.wav", False)

        if self.selected == 1:
            audio.play_wav("Pew2.wav", False)

        if self.selected == 2:
            audio.play_wav("Pew3.wav", False)


menu = Menu()

graphics.set_font("bitmap6")

su.set_brightness(0.7)

while True:

    graphics.set_pen(graphics.create_pen(0, 0, 0))
    graphics.clear()

    if su.is_pressed(StellarUnicorn.SWITCH_BRIGHTNESS_DOWN):
        audio.play_wav("buttonbeep.wav", False)
        if menu.selected + 1 < len(menu.items):
            menu.selected += 1
        else:
            menu.selected = 0

    if su.is_pressed(StellarUnicorn.SWITCH_BRIGHTNESS_UP):
        audio.play_wav("buttonbeep.wav", False)
        if menu.selected > 0:
            menu.selected -= 1
        else:
            menu.selected = len(menu.items) - 1

    if su.is_pressed(StellarUnicorn.SWITCH_SLEEP):
        menu.process_selected()

    menu.draw_menu()

    su.update(graphics)
    sleep(0.2)
