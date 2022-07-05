# A customisable Pride flag. (Look in the Tufty 2040 examples for a name badge version!)

from picographics import PicoGraphics, DISPLAY_PICO_DISPLAY

display = PicoGraphics(display=DISPLAY_PICO_DISPLAY, rotate=0)

WIDTH, HEIGHT = display.get_bounds()

# List of available pen colours, add more if necessary
RED = display.create_pen(209, 34, 41)
ORANGE = display.create_pen(246, 138, 30)
YELLOW = display.create_pen(255, 216, 0)
GREEN = display.create_pen(0, 121, 64)
INDIGO = display.create_pen(36, 64, 142)
VIOLET = display.create_pen(115, 41, 130)
WHITE = display.create_pen(255, 255, 255)
PINK = display.create_pen(255, 175, 200)
BLUE = display.create_pen(116, 215, 238)
BROWN = display.create_pen(97, 57, 21)
BLACK = display.create_pen(0, 0, 0)
MAGENTA = display.create_pen(255, 33, 140)
CYAN = display.create_pen(33, 177, 255)

# Uncomment one of these to change flag
# If adding your own, colour order is left to right (or top to bottom)
COLOUR_ORDER = [RED, ORANGE, YELLOW, GREEN, INDIGO, VIOLET]  # traditional pride flag
# COLOUR_ORDER = [BLACK, BROWN, RED, ORANGE, YELLOW, GREEN, INDIGO, VIOLET]  # Philadelphia pride flag
# COLOUR_ORDER = [BLUE, PINK, WHITE, PINK, BLUE]  # trans flag
# COLOUR_ORDER = [MAGENTA, YELLOW, CYAN]  # pan flag
# COLOUR_ORDER = [MAGENTA, VIOLET, INDIGO]  # bi flag

# Change this for vertical stripes
STRIPES_DIRECTION = "horizontal"

# Draw the flag
if STRIPES_DIRECTION == "horizontal":
    stripe_width = round(HEIGHT / len(COLOUR_ORDER))
    for x in range(len(COLOUR_ORDER)):
        display.set_pen(COLOUR_ORDER[x])
        display.rectangle(0, stripe_width * x, WIDTH, stripe_width)

if STRIPES_DIRECTION == "vertical":
    stripe_width = round(WIDTH / len(COLOUR_ORDER))
    for x in range(len(COLOUR_ORDER)):
        display.set_pen(COLOUR_ORDER[x])
        display.rectangle(stripe_width * x, 0, stripe_width, HEIGHT)

display.update()
