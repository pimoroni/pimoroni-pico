from audio import WavPlayer
from cosmic import CosmicUnicorn
from picographics import PicoGraphics, DISPLAY_COSMIC_UNICORN as DISPLAY
from time import sleep

cu = CosmicUnicorn()
graphics = PicoGraphics(DISPLAY)

audio = WavPlayer(0, 10, 11, 9, amp_enable=22)

WHITE = graphics.create_pen(255, 255, 255)
RED = graphics.create_pen(255, 0, 0)


class Menu(object):

    def __init__(self):
        self.items = ["Pew 1", "Pew 2", "Pew 3"]
        self.selected = 0

    # A function to draw only the menu elements.
    # Helps to keep our main  function tidy!
    def draw_menu(self):
        graphics.set_pen(WHITE)

        for item in range(len(self.items)):
            if self.selected == item:
                graphics.set_pen(RED)

            graphics.text(self.items[item], 0, 2 + item * 10, 31, 1)
            graphics.set_pen(WHITE)

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
cu.set_brightness(0.7)

while True:

    graphics.set_pen(graphics.create_pen(0, 0, 0))
    graphics.clear()

    if cu.is_pressed(CosmicUnicorn.SWITCH_BRIGHTNESS_DOWN):
        audio.play_wav("buttonbeep.wav", False)
        if menu.selected + 1 < len(menu.items):
            menu.selected += 1
        else:
            menu.selected = 0

    if cu.is_pressed(CosmicUnicorn.SWITCH_BRIGHTNESS_UP):
        audio.play_wav("buttonbeep.wav", False)
        if menu.selected > 0:
            menu.selected -= 1
        else:
            menu.selected = len(menu.items) - 1

    if cu.is_pressed(CosmicUnicorn.SWITCH_SLEEP):
        menu.process_selected()

    menu.draw_menu()

    cu.update(graphics)
    sleep(0.2)
