# A spinny rainbow wheel. Change up some of the constants below to see what happens.

import math
from picographics import PicoGraphics, DISPLAY_PICO_DISPLAY_2

# Constants for drawing
INNER_RADIUS = 40
OUTER_RADIUS = 120
NUMBER_OF_LINES = 24
HUE_SHIFT = 0.02
ROTATION_SPEED = 2
LINE_THICKNESS = 2

# Set up the display
graphics = PicoGraphics(display=DISPLAY_PICO_DISPLAY_2)

WIDTH, HEIGHT = graphics.get_bounds()

BLACK = graphics.create_pen(0, 0, 0)

# Variables to keep track of rotation and hue positions
r = 0
t = 0

while True:
    graphics.set_pen(BLACK)
    graphics.clear()
    for i in range(0, 360, 360 // NUMBER_OF_LINES):
        graphics.set_pen(graphics.create_pen_hsv((i / 360) + t, 1.0, 1.0))
        # Draw some lines, offset by the rotation variable
        graphics.line(int(WIDTH / 2 + math.cos(math.radians(i + r)) * INNER_RADIUS),
                      int(HEIGHT / 2 + math.sin(math.radians(i + r)) * INNER_RADIUS),
                      int(WIDTH / 2 + math.cos(math.radians(i + 90 + r)) * OUTER_RADIUS),
                      int(HEIGHT / 2 + math.sin(math.radians(i + 90 + r)) * OUTER_RADIUS),
                      LINE_THICKNESS)
    graphics.update()
    r += ROTATION_SPEED
    t += HUE_SHIFT
