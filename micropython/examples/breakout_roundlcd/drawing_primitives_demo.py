from picographics import PicoGraphics, DISPLAY_ROUND_LCD_240X240

display = PicoGraphics(display=DISPLAY_ROUND_LCD_240X240)
display.set_backlight(1.0)

width, height = display.get_bounds()

BLACK = display.create_pen(0, 0, 0)
RED = display.create_pen(255, 0, 0)
YELLOW = display.create_pen(255, 255, 0)
GREEN = display.create_pen(0, 255, 0)
CYAN = display.create_pen(0, 255, 255)
WHITE = display.create_pen(255, 255, 255)
BLUE = display.create_pen(0, 0, 255)
MAGENTA = display.create_pen(255, 0, 255)

while True:
    display.set_pen(BLACK)
    display.clear()

    # circle
    display.set_pen(RED)
    display.circle(
        width // 5,  # center point x
        height // 3,  # center point y
        16,  # radius
    )

    # rectangle
    display.set_pen(YELLOW)
    display.rectangle(
        int((width * 2 / 5) - 16),  # starting point x
        int(height // 3) - 8,  # starting point y
        32,  # width
        16,  # height
    )

    # triangle
    display.set_pen(GREEN)
    display.triangle(
        int(width * 3 / 5), int(height // 3) - 16,  # point 1 x, y
        int(width * 3 / 5) - 16, int(height // 3) + 16,  # point 2 x, y
        int(width * 3 / 5) + 16, int(height // 3) + 16,  # point 3 x, y
    )

    # character
    display.set_pen(CYAN)
    display.character(
        64,  # int character code
        int(width * 4 / 5 - 16),  # box starting point x
        int(height // 3) - 16,  # box starting point y
        4,  # scale - not required, default is 2
    )

    # pixel span
    display.set_pen(WHITE)
    display.pixel_span(
        int(width * 1 / 5),  # starting point x
        int(height * 2.5 / 5),  # starting point y
        130,  # length
    )

    # text
    display.set_pen(BLUE)
    display.text(
        'test text',  # text
        int(width // 5),  # box starting point x
        int(height * 3 / 5),  # box starting point y
        True,  # word wrap
        6,  # scale - not required, default is 2
    )

    # lines
    display.set_pen(MAGENTA)
    display.line(
        0,  # staring point x
        int(height / 2),  # staring point y
        width,  # end point x
        height,  # end point y
    )
    display.line(
        width,  # staring point x
        int(height / 2),  # staring point y
        0,  # end point x
        height,  # end point y
    )

    # set clip
    display.set_clip(
        int(width * 2 / 5),
        int(height * 2 / 5),
        int(width * 1 / 5),
        int(height * 1 / 5)
    )

    # draw a clipped circle
    display.circle(
        int(width * 2 / 5),
        int(height * 2 / 5),
        32,
    )

    # remove clip
    display.remove_clip()

    display.update()
