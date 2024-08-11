# from picographics import PicoGraphics, DISPLAY_INKY_FRAME as DISPLAY      # 5.7"
# from picographics import PicoGraphics, DISPLAY_INKY_FRAME_4 as DISPLAY  # 4.0"
from picographics import PicoGraphics, DISPLAY_INKY_FRAME_7 as DISPLAY  # 7.3"

graphics = PicoGraphics(DISPLAY)

WIDTH, HEIGHT = graphics.get_bounds()

graphics.set_pen(1)
graphics.clear()

w = int(WIDTH / 8)

# Solid Colours

for p in range(8):
    graphics.set_pen(p)
    graphics.rectangle(w * p, 0, w, 50)

# "Greydient"

for x in range(WIDTH):
    g = int(x / float(WIDTH) * 255)
    graphics.set_pen(graphics.create_pen(g, g, g))
    for y in range(30):
        graphics.pixel(x, 60 + y)

# Rainbow Gradient

for x in range(WIDTH):
    h = x / float(WIDTH)
    graphics.set_pen(graphics.create_pen_hsv(h, 1.0, 1.0))
    for y in range(100):
        graphics.pixel(x, 100 + y)

# Block Colours & Text

graphics.set_pen(graphics.create_pen(128, 128, 0))
graphics.rectangle(0, 210, 200, 100)
graphics.set_pen(graphics.create_pen(200, 200, 200))
graphics.text("Hello", 10, 220)
graphics.text("Hello", 10, 240, scale=4.0)

graphics.set_pen(graphics.create_pen(0, 128, 128))
graphics.rectangle(200, 210, 200, 100)
graphics.set_pen(graphics.create_pen(200, 200, 200))
graphics.text("Hello", 210, 220)
graphics.text("Hello", 210, 240, scale=4.0)

graphics.set_pen(graphics.create_pen(128, 0, 128))
graphics.rectangle(400, 210, 200, 100)
graphics.set_pen(graphics.create_pen(200, 200, 200))
graphics.text("Hello", 410, 220)
graphics.text("Hello", 410, 240, scale=4.0)

# Red, Green and Blue gradients

for x in range(WIDTH):
    g = int(x / float(WIDTH) * 255)
    for y in range(20):
        graphics.set_pen(graphics.create_pen(g, 0, 0))
        graphics.pixel(x, 320 + y)
        graphics.set_pen(graphics.create_pen(0, g, 0))
        graphics.pixel(x, 350 + y)
        graphics.set_pen(graphics.create_pen(0, 0, g))
        graphics.pixel(x, 380 + y)

graphics.update()
