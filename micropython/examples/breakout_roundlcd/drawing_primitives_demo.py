from breakout_roundlcd import BreakoutRoundLCD


width = BreakoutRoundLCD.WIDTH
height = BreakoutRoundLCD.HEIGHT

display_buffer = bytearray(width * height * 2)  # 2-bytes per pixel (RGB565)
display = BreakoutRoundLCD(display_buffer)

display.set_backlight(1)

while True:
    display.set_pen(0, 0, 0)
    display.clear()

    # circle
    display.set_pen(255, 0, 0)
    display.circle(
        width // 5,  # center point x
        height // 3,  # center point y
        16,  # radius
    )

    # rectangle
    display.set_pen(255, 255, 0)
    display.rectangle(
        int((width * 2 / 5) - 16),  # starting point x
        int(height // 3) - 8,  # starting point y
        32,  # width
        16,  # height
    )

    # triangle
    display.set_pen(0, 255, 0)
    display.triangle(
        int(width * 3 / 5), int(height // 3) - 16,  # point 1 x, y
        int(width * 3 / 5) - 16, int(height // 3) + 16,  # point 2 x, y
        int(width * 3 / 5) + 16, int(height // 3) + 16,  # point 3 x, y
    )

    # character
    display.set_pen(0, 255, 255)
    display.character(
        64,  # int character code
        int(width * 4 / 5 - 16),  # box starting point x
        int(height // 3) - 16,  # box starting point y
        4,  # scale - not required, default is 2
    )

    # pixel span
    display.set_pen(255, 255, 255)
    display.pixel_span(
        int(width * 1 / 5),  # starting point x
        int(height * 2.5 / 5),  # starting point y
        130,  # length
    )

    # text
    display.set_pen(0, 0, 255)
    display.text(
        'test text',  # text
        int(width // 5),  # box starting point x
        int(height * 3 / 5),  # box starting point y
        True,  # word wrap
        6,  # scale - not required, default is 2
    )

    # lines
    display.set_pen(255, 0, 255)
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
