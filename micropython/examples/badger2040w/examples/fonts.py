import badger2040w
import badger_os

# Global Constants
FONT_NAMES = (
    ("sans", 0.7, 2),
    ("gothic", 0.7, 2),
    ("cursive", 0.7, 2),
    ("serif", 0.7, 2),
    ("serif_italic", 0.7, 2),
    ("bitmap6", 3, 1),
    ("bitmap8", 2, 1),
    ("bitmap14_outline", 1, 1)
)

WIDTH = badger2040w.WIDTH
HEIGHT = badger2040w.HEIGHT

MENU_TEXT_SIZE = 0.5
MENU_SPACING = 16
MENU_WIDTH = 84
MENU_PADDING = 5

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
    display.set_pen(15)
    display.clear()
    display.set_pen(12)
    display.rectangle(WIDTH - ARROW_WIDTH, 0, ARROW_WIDTH, HEIGHT)
    display.set_pen(0)
    draw_up(WIDTH - ARROW_WIDTH, (HEIGHT // 4) - (ARROW_HEIGHT // 2),
            ARROW_WIDTH, ARROW_HEIGHT, ARROW_THICKNESS, ARROW_PADDING)
    draw_down(WIDTH - ARROW_WIDTH, ((HEIGHT * 3) // 4) - (ARROW_HEIGHT // 2),
              ARROW_WIDTH, ARROW_HEIGHT, ARROW_THICKNESS, ARROW_PADDING)


# Draw the fonts and menu
def draw_fonts():
    display.set_font("bitmap8")
    for i in range(len(FONT_NAMES)):
        name, size, thickness = FONT_NAMES[i]
        display.set_pen(0)
        if i == state["selected_font"]:
            display.rectangle(0, i * MENU_SPACING, MENU_WIDTH, MENU_SPACING)
            display.set_pen(15)

        display.text(name, MENU_PADDING, (i * MENU_SPACING) + int((MENU_SPACING - 8) / 2), WIDTH, MENU_TEXT_SIZE)

    name, size, thickness = FONT_NAMES[state["selected_font"]]
    display.set_font(name)

    y = 0 if name.startswith("bitmap") else 10

    display.set_pen(0)
    for line in ("The quick", "brown fox", "jumps over", "the lazy dog.", "0123456789", "!\"£$%^&*()"):
        display.text(line, TEXT_INDENT, y, WIDTH, size)
        y += 22

    display.update()


# ------------------------------
#        Program setup
# ------------------------------

# Global variables
state = {"selected_font": 0}
badger_os.state_load("fonts", state)

# Create a new Badger and set it to update FAST
display = badger2040w.Badger2040W()
display.led(128)
display.set_update_speed(badger2040w.UPDATE_FAST)

changed = not badger2040w.woken_by_button()

# ------------------------------
#       Main program loop
# ------------------------------

while True:
    if display.pressed(badger2040w.BUTTON_UP):
        state["selected_font"] -= 1
        if state["selected_font"] < 0:
            state["selected_font"] = len(FONT_NAMES) - 1
        changed = True
    if display.pressed(badger2040w.BUTTON_DOWN):
        state["selected_font"] += 1
        if state["selected_font"] >= len(FONT_NAMES):
            state["selected_font"] = 0
        changed = True

    if changed:
        draw_frame()
        draw_fonts()
        badger_os.state_save("fonts", state)
        changed = False

    display.halt()
