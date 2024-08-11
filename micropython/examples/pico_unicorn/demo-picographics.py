# This is a PicoGraphics version of the original demo.py

from picounicorn import PicoUnicorn
from picographics import PicoGraphics, DISPLAY_UNICORN_PACK

picounicorn = PicoUnicorn()
graphics = PicoGraphics(display=DISPLAY_UNICORN_PACK)

w = picounicorn.get_width()
h = picounicorn.get_height()

# Display a rainbow across Pico Unicorn
for x in range(w):
    for y in range(h):
        # PicoGraphics allows us to set HSV pens directly
        PEN = graphics.create_pen_hsv(x / w, y / h, 1.0)
        graphics.set_pen(PEN)
        graphics.pixel(x, y)

# Ask the Unicorn to update the graphics
picounicorn.update(graphics)

print("Press Button A")

while not picounicorn.is_pressed(picounicorn.BUTTON_A):  # Wait for Button A to be pressed
    pass

# Clear the display

# Set the pen to black
BLACK = graphics.create_pen(0, 0, 0)
graphics.set_pen(BLACK)
graphics.clear()

# Ask the Unicorn to update the graphics
picounicorn.update(graphics)

print("Button A pressed!")
