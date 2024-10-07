import time
from picographics import PicoGraphics, DISPLAY_PICO_DISPLAY_2, PEN_RGB565
from pimoroni import RGBLED
from machine import Pin

button_a = Pin(12, Pin.IN)
button_x = Pin(14, Pin.IN)
button_y = Pin(15, Pin.IN)

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
        self.items = ["Red", "Green", "Blue", "Purple"]
        self.selected = 0
        self.shadow_offset = 2
        self.cursor = "<-"
        self.colour = BLACK
        self.title = "Simple Menu"

    # A function to draw only the menu elements.
    def draw_menu(self):
        display.set_pen(WHITE)
        display.clear()
        display.set_pen(BLACK)
        length = display.measure_text(self.title, 4)
        display.text(self.title, WIDTH // 2 - length // 2 + self.shadow_offset, 10 + self.shadow_offset, WIDTH, 4)
        display.set_pen(self.colour)
        display.text(self.title, WIDTH // 2 - length // 2, 10, WIDTH, 4)

        display.set_pen(BLACK)
        for item in range(len(self.items)):
            length = display.measure_text(self.items[item], 3)
            if self.selected == item:
                display.set_pen(self.colour)
                display.text(self.cursor, length + 40, HEIGHT // 2 + item * 20, WIDTH, 3)

            display.text(self.items[item], 30, HEIGHT // 2 + item * 20, WIDTH, 3)

            display.set_pen(self.colour)
            display.rectangle(0, HEIGHT - 10, WIDTH, 10)
            display.set_pen(BLACK)

    # Do a thing based on the currently selected menu item
    # For our example we'll be changing the text and LED colour
    def process_selected(self):
        if self.selected == 0:
            self.colour = RED
            led.set_rgb(255, 0, 0)

        if self.selected == 1:
            self.colour = GREEN
            led.set_rgb(0, 255, 0)

        if self.selected == 2:
            self.colour = BLUE
            led.set_rgb(0, 0, 255)

        if self.selected == 3:
            self.colour = PURPLE
            led.set_rgb(255, 0, 255)

    def user_input(self):
        # Process the user input and update the currently selected item
        if button_y.value() == 0:
            if self.selected + 1 < len(self.items):
                self.selected += 1
            else:
                self.selected = 0

        if button_x.value() == 0:
            if self.selected > 0:
                self.selected -= 1
            else:
                self.selected = len(self.items) - 1

        if button_a.value() == 0:
            self.process_selected()


menu = Menu()

while True:

    menu.draw_menu()
    menu.user_input()

    display.update()

    time.sleep(1.0 / 20)
