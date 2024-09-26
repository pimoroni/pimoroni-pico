# A scrolling menu example
# Use B and Y buttons to navigate LEFT and RIGHT
# Press A to select a character. Selected characters are shown at the top of the display.
# Press X to delete the last character.

import time
from picographics import PicoGraphics, DISPLAY_PICO_DISPLAY_2, PEN_RGB565
from pimoroni import RGBLED
from machine import Pin

button_a = Pin(12, Pin.IN, Pin.PULL_UP)
button_b = Pin(13, Pin.IN, Pin.PULL_UP)
button_x = Pin(14, Pin.IN, Pin.PULL_UP)
button_y = Pin(15, Pin.IN, Pin.PULL_UP)

display = PicoGraphics(display=DISPLAY_PICO_DISPLAY_2, pen_type=PEN_RGB565, rotate=0)
display.set_backlight(0.8)

# set up constants for drawing
WIDTH, HEIGHT = display.get_bounds()
BLACK = display.create_pen(0, 0, 0)
RED = display.create_pen(255, 0, 0)
GREEN = display.create_pen(0, 255, 0)
BLUE = display.create_pen(0, 0, 255)
WHITE = display.create_pen(255, 255, 255)
PURPLE = display.create_pen(255, 0, 255)

led = RGBLED(26, 27, 28)


class Menu(object):
    def __init__(self):
        self.items = ['a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l',
                      'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x',
                      'y', 'z']
        self.selected = 0
        self.shadow_offset = 2
        self.cursor = "<-"

        self.start = 0
        self.stop = self.start + 10
        self.last_press = 0
        self.title = "Scrolling Menu"

        self.characters = []

    # A function to draw only the menu elements.
    # Helps to keep our main draw function tidy!
    def draw_menu(self):
        display.set_pen(WHITE)
        display.clear()

        self.slice = self.items[self.start:self.stop]

        display.set_pen(BLACK)
        length = display.measure_text(self.title, 4)
        display.text(self.title, WIDTH // 2 - length // 2 + self.shadow_offset, 10 + self.shadow_offset, WIDTH, 4)
        display.set_pen(RED)
        display.text(self.title, WIDTH // 2 - length // 2, 10, WIDTH, 4)

        display.set_pen(RED)
        display.rectangle(5, HEIGHT - 35, 35, 35)

        for item in range(len(self.slice)):
            if item == 0:
                display.set_pen(WHITE)
            else:
                display.set_pen(BLACK)

            display.text(self.slice[item], 15 + item * 30, HEIGHT - 30, WIDTH, 4)

        display.set_pen(BLACK)
        for i in range(len(self.characters)):
            display.text(self.characters[i], 5 + 15 * i, 50, WIDTH, 2)

    def remove_last_char(self):
        if len(self.characters) > 0:
            self.characters.pop()

    # Do a thing based on the currently selected menu item
    # We're taking the highlighted character and saving it in our array for later.
    def process_selected(self):
        if len(self.characters) > 20:
            self.characters = []
        self.characters += self.items[self.selected]

    def user_input(self):

        # Process the user input and update the currently selected item
        if button_y.value() == 0:
            if self.selected < len(self.items) - 1:
                self.selected += 1
            if self.stop < len(self.items) + 9:
                self.start += 1
                self.stop += 1
            else:
                self.start = 0
                self.stop = self.start + 10
                self.selected = 0

        if button_b.value() == 0:
            if self.selected > 0:
                self.selected -= 1
            if self.stop > 10:
                self.start -= 1
                self.stop -= 1
            else:
                self.start = len(self.items) - 1
                self.stop = self.start + 10
                self.selected = len(self.items) - 1

        if button_a.value() == 0:
            self.process_selected()

        if button_x.value() == 0:
            self.remove_last_char()


menu = Menu()

while True:

    menu.draw_menu()
    menu.user_input()

    display.update()

    time.sleep(1.0 / 20)
