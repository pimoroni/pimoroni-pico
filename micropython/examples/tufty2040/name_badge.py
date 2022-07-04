from picographics import PicoGraphics, DISPLAY_TUFTY_2040

display = PicoGraphics(display=DISPLAY_TUFTY_2040)
WIDTH, HEIGHT = display.get_bounds()
display.set_backlight(1.0)

# Tufty constants
A = 7
B = 8
C = 15
UP = 22
DOWN = 6
LED = 25

WHITE = display.create_pen(255, 255, 255)
BLACK = display.create_pen(0, 0, 0)
RED = display.create_pen(200, 0, 0)

# Read name from file
try:
    file = open("badge.txt", "r")
except OSError:
    with open("badge.txt", "w") as f:
        f.write("open badge.txt in thonny to edit badge :)")
        f.flush()
    file = open("badge.txt", "r")

name = file.readline()
file.close()

text_size = 12
text_x = 0
text_y = 100

# Clear the screen
display.set_pen(WHITE)
display.clear()
display.update()


# Draws a blank badge
def draw_badge():
    display.set_pen(RED)
    display.rectangle(0, 0, WIDTH, 60)
    display.rectangle(0, HEIGHT - 20, WIDTH, 50)
    display.set_pen(WHITE)
    display.text("HELLO", 125, 5, 0, 3)
    display.text("My name is:", 110, 35, 320, 2)
    display.update()


def calculate_text_size():
    global text_size
    name_width = display.measure_text(name, text_size)
    # Calculate the width of the text in pixels, adjusts according to the screen width
    while name_width > 290:
        text_size -= 1
        name_width = display.measure_text(name, text_size)

    # Calculate the margin to be applied on X
    margin_x = (WIDTH - name_width) / 2

    return int(margin_x)


draw_badge()
text_x = calculate_text_size()

display.set_pen(BLACK)
display.text(name, text_x, text_y, 300, text_size)
display.update()
