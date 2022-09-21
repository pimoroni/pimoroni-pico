# A name badge with customisable Pride flag background.

from picographics import PicoGraphics, DISPLAY_TUFTY_2040
import math

display = PicoGraphics(display=DISPLAY_TUFTY_2040)

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
AMETHYST = display.create_pen(156, 89, 209)

# Uncomment one of these to change flag
# If adding your own, colour order is left to right (or top to bottom)
COLOUR_ORDER = [RED, ORANGE, YELLOW, GREEN, INDIGO, VIOLET]  # traditional pride flag
# COLOUR_ORDER = [BLACK, BROWN, RED, ORANGE, YELLOW, GREEN, INDIGO, VIOLET]  # Philadelphia pride flag
# COLOUR_ORDER = [BLUE, PINK, WHITE, PINK, BLUE]  # trans flag
# COLOUR_ORDER = [MAGENTA, YELLOW, CYAN]  # pan flag
# COLOUR_ORDER = [MAGENTA, VIOLET, INDIGO]  # bi flag
# COLOUR_ORDER = [YELLOW, WHITE, AMETHYST, BLACK]  # non-binary flag

# Add chevrons to the left
# CHEVRONS = [] # No chevrons
CHEVRONS = [WHITE, PINK, BLUE, BROWN, BLACK]  # Progress Pride Flag
# Initial chevron height compared to screen height
FIRST_CHEVRON_HEIGHT = 0.4

# Change this for vertical stripes
STRIPES_DIRECTION = "horizontal"

# Change details here! Works best with a short, one word name
NAME = "Hel"
PRONOUNS = "She/they"

# Change the colour of the text (swapping these works better on a light background)
TEXT_COLOUR = WHITE
DROP_SHADOW_COLOUR = BLACK

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

if len(CHEVRONS) > 0:
    stripe_width = round((HEIGHT * (1 - FIRST_CHEVRON_HEIGHT)) / len(CHEVRONS))
    offset = -stripe_width * math.floor((len(CHEVRONS) + 1) / 2)
    middle = round(HEIGHT / 2)
    for x in range(len(CHEVRONS) - 1, -1, -1):
        display.set_pen(CHEVRONS[x])
        display.triangle(
            x * stripe_width + offset, -stripe_width,
            (x + 1) * stripe_width + offset + middle, middle,
            x * stripe_width + offset, HEIGHT + stripe_width)

# Set a starting scale for text size.
# This is intentionally bigger than will fit on the screen, we'll shrink it to fit.
name_size = 20
pronouns_size = 20

# These loops adjust the scale of the text until it fits on the screen
while True:
    display.set_font("bitmap8")
    name_length = display.measure_text(NAME, name_size)
    if name_length >= WIDTH - 20:
        name_size -= 1
    else:
        # comment out this section if you hate drop shadow
        DROP_SHADOW_OFFSET = 5
        display.set_pen(DROP_SHADOW_COLOUR)
        display.text(NAME, int((WIDTH - name_length) / 2 + 10) - DROP_SHADOW_OFFSET, 10 + DROP_SHADOW_OFFSET, WIDTH, name_size)

        # draw name and stop looping
        display.set_pen(TEXT_COLOUR)
        display.text(NAME, int((WIDTH - name_length) / 2 + 10), 10, WIDTH, name_size)
        break

while True:
    display.set_font("bitmap8")
    pronouns_length = display.measure_text(PRONOUNS, pronouns_size)
    if pronouns_length >= WIDTH - 60:
        pronouns_size -= 1
    else:
        # draw pronouns and stop looping
        display.set_pen(TEXT_COLOUR)
        display.text(PRONOUNS, int((WIDTH - pronouns_length) / 2), 175, WIDTH, pronouns_size)
        break

# Once all the adjusting and drawing is done, update the display.
display.update()
