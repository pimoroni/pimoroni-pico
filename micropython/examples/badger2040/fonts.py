import badger2040
import machine
import time

# Global Constants
FONT_NAMES = ("sans", "gothic", "cursive", "serif", "serif_italic")

WIDTH = badger2040.WIDTH
HEIGHT = badger2040.HEIGHT

MENU_TEXT_SIZE = 0.5
MENU_SPACING = 20
MENU_WIDTH = 84
MENU_PADDING = 2

TEXT_SIZE = 0.8
TEXT_INDENT = MENU_WIDTH + 10

ARROW_THICKNESS = 3
ARROW_WIDTH = 18
ARROW_HEIGHT = 14
ARROW_PADDING = 2


# ------------------------------
#      Drawing functions
# ------------------------------

# Draw a upward arrow
def draw_up(x, y, width, height, thickness, padding):
    border = (thickness // 4) + padding
    display.line(x + border, y + height - border,
                 x + (width // 2), y + border)
    display.line(x + (width // 2), y + border,
                 x + width - border, y + height - border)


# Draw a downward arrow
def draw_down(x, y, width, height, thickness, padding):
    border = (thickness // 2) + padding
    display.line(x + border, y + border,
                 x + (width // 2), y + height - border)
    display.line(x + (width // 2), y + height - border,
                 x + width - border, y + border)


# Draw the frame of the reader
def draw_frame():
    display.pen(15)
    display.clear()
    display.pen(12)
    display.rectangle(WIDTH - ARROW_WIDTH, 0, ARROW_WIDTH, HEIGHT)
    display.pen(0)
    display.thickness(ARROW_THICKNESS)
    draw_up(WIDTH - ARROW_WIDTH, (HEIGHT // 4) - (ARROW_HEIGHT // 2),
            ARROW_WIDTH, ARROW_HEIGHT, ARROW_THICKNESS, ARROW_PADDING)
    draw_down(WIDTH - ARROW_WIDTH, ((HEIGHT * 3) // 4) - (ARROW_HEIGHT // 2),
              ARROW_WIDTH, ARROW_HEIGHT, ARROW_THICKNESS, ARROW_PADDING)


# Draw the fonts and menu
def draw_fonts():
    display.font("sans")
    display.thickness(1)
    for i in range(len(FONT_NAMES)):
        name = FONT_NAMES[i]
        display.pen(0)
        if i == selected_font:
            display.rectangle(0, i * MENU_SPACING, MENU_WIDTH, MENU_SPACING)
            display.pen(15)

        display.text(name, MENU_PADDING, (i * MENU_SPACING) + (MENU_SPACING // 2), MENU_TEXT_SIZE)

    display.font(FONT_NAMES[selected_font])
    display.thickness(2)

    display.pen(0)
    display.text("The quick", TEXT_INDENT, 10, TEXT_SIZE)
    display.text("brown fox", TEXT_INDENT, 32, TEXT_SIZE)
    display.text("jumped over", TEXT_INDENT, 54, TEXT_SIZE)
    display.text("the lazy dog.", TEXT_INDENT, 76, TEXT_SIZE)
    display.text("0123456789", TEXT_INDENT, 98, TEXT_SIZE)
    display.text("!\"£$%^&*()", TEXT_INDENT, 120, TEXT_SIZE)
    display.thickness(1)

    display.update()


# ------------------------------
#        Program setup
# ------------------------------

# Global variables
selected_font = 0
pressed = False

# Create a new Badger and set it to update FAST
display = badger2040.Badger2040()
display.update_speed(badger2040.UPDATE_FAST)

# Set up the buttons
button_up = machine.Pin(badger2040.BUTTON_UP, machine.Pin.IN, machine.Pin.PULL_DOWN)
button_down = machine.Pin(badger2040.BUTTON_DOWN, machine.Pin.IN, machine.Pin.PULL_DOWN)


# Button handling function
def button(pin):
    global pressed
    global selected_font

    if not pressed:
        if pin == button_up:
            selected_font -= 1
            if selected_font < 0:
                selected_font = len(FONT_NAMES) - 1
            pressed = True
            return
        if pin == button_down:
            selected_font += 1
            if selected_font >= len(FONT_NAMES):
                selected_font = 0
            pressed = True
            return


# Register the button handling function with the buttons
button_up.irq(trigger=machine.Pin.IRQ_RISING, handler=button)
button_down.irq(trigger=machine.Pin.IRQ_RISING, handler=button)


# ------------------------------
#       Main program loop
# ------------------------------

while True:
    draw_frame()
    draw_fonts()

    pressed = False
    while not pressed:
        time.sleep(0.1)
