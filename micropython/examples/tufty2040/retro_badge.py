# A retro badge with photo and QR code.
# Copy your image to your Tufty alongside this example - it should be a 120 x 120 jpg.

from picographics import PicoGraphics, DISPLAY_TUFTY_2040
from pimoroni import Button
import time
import jpegdec
import qrcode

display = PicoGraphics(display=DISPLAY_TUFTY_2040)
button_c = Button(9, invert=False)

WIDTH, HEIGHT = display.get_bounds()

# Uncomment one of these four colour palettes - find more at lospec.com !
# Nostalgia colour palette by WildLeoKnight - https://lospec.com/palette-list/nostalgia
LIGHTEST = display.create_pen(208, 208, 88)
LIGHT = display.create_pen(160, 168, 64)
DARK = display.create_pen(112, 128, 40)
DARKEST = display.create_pen(64, 80, 16)

# 2bit Demichrome colour palette by Space Sandwich - https://lospec.com/palette-list/2bit-demichrome
# LIGHTEST = display.create_pen(233, 239, 236)
# LIGHT = display.create_pen(160, 160, 139)
# DARK = display.create_pen(85, 85, 104)
# DARKEST = display.create_pen(33, 30, 32)

# CGA PALETTE 1 (HIGH) - https://lospec.com/palette-list/cga-palette-1-high
# LIGHTEST = display.create_pen(255, 255, 255)
# LIGHT = display.create_pen(85, 254, 255)
# DARK = display.create_pen(255, 85, 255)
# DARKEST = display.create_pen(0, 0, 0)

# Change your badge and QR details here!
COMPANY_NAME = "sciurus cybernetics"
NAME = "M. 'TuFTy'"
BLURB1 = "RP2040 plus 320x240 TFT LCD"
BLURB2 = "Nuts From Trees collector"
BLURB3 = "Will work for peanuts"

QR_TEXT = "pimoroni.com/tufty2040"

IMAGE_NAME = "squirrel.jpg"

# Some constants we'll use for drawing
BORDER_SIZE = 4
PADDING = 10
COMPANY_HEIGHT = 40


def draw_badge():
    # draw border
    display.set_pen(LIGHTEST)
    display.clear()

    # draw background
    display.set_pen(DARK)
    display.rectangle(BORDER_SIZE, BORDER_SIZE, WIDTH - (BORDER_SIZE * 2), HEIGHT - (BORDER_SIZE * 2))

    # draw company box
    display.set_pen(DARKEST)
    display.rectangle(BORDER_SIZE, BORDER_SIZE, WIDTH - (BORDER_SIZE * 2), COMPANY_HEIGHT)

    # draw company text
    display.set_pen(LIGHT)
    display.set_font("bitmap6")
    display.text(COMPANY_NAME, BORDER_SIZE + PADDING, BORDER_SIZE + PADDING, WIDTH, 3)

    # draw name text
    display.set_pen(LIGHTEST)
    display.set_font("bitmap8")
    display.text(NAME, BORDER_SIZE + PADDING, BORDER_SIZE + PADDING + COMPANY_HEIGHT, WIDTH, 5)

    # draws the bullet points
    display.set_pen(DARKEST)
    display.text("*", BORDER_SIZE + PADDING + 120 + PADDING, 105, 160, 2)
    display.text("*", BORDER_SIZE + PADDING + 120 + PADDING, 140, 160, 2)
    display.text("*", BORDER_SIZE + PADDING + 120 + PADDING, 175, 160, 2)

    # draws the blurb text (4 - 5 words on each line works best)
    display.set_pen(LIGHTEST)
    display.text(BLURB1, BORDER_SIZE + PADDING + 135 + PADDING, 105, 160, 2)
    display.text(BLURB2, BORDER_SIZE + PADDING + 135 + PADDING, 140, 160, 2)
    display.text(BLURB3, BORDER_SIZE + PADDING + 135 + PADDING, 175, 160, 2)


def show_photo():
    j = jpegdec.JPEG(display)

    # Open the JPEG file
    j.open_file(IMAGE_NAME)

    # Draws a box around the image
    display.set_pen(DARKEST)
    display.rectangle(PADDING, HEIGHT - ((BORDER_SIZE * 2) + PADDING) - 120, 120 + (BORDER_SIZE * 2), 120 + (BORDER_SIZE * 2))

    # Decode the JPEG
    j.decode(BORDER_SIZE + PADDING, HEIGHT - (BORDER_SIZE + PADDING) - 120)

    # Draw QR button label
    display.set_pen(LIGHTEST)
    display.text("QR", 240, 215, 160, 2)


def measure_qr_code(size, code):
    w, h = code.get_size()
    module_size = int(size / w)
    return module_size * w, module_size


def draw_qr_code(ox, oy, size, code):
    size, module_size = measure_qr_code(size, code)
    display.set_pen(LIGHTEST)
    display.rectangle(ox, oy, size, size)
    display.set_pen(DARKEST)
    for x in range(size):
        for y in range(size):
            if code.get_module(x, y):
                display.rectangle(ox + x * module_size, oy + y * module_size, module_size, module_size)


def show_qr():
    display.set_pen(DARK)
    display.clear()

    code = qrcode.QRCode()
    code.set_text(QR_TEXT)

    size, module_size = measure_qr_code(HEIGHT, code)
    left = int((WIDTH // 2) - (size // 2))
    top = int((HEIGHT // 2) - (size // 2))
    draw_qr_code(left, top, HEIGHT, code)


# draw the badge for the first time
badge_mode = "photo"
draw_badge()
show_photo()
display.update()

while True:
    if button_c.is_pressed:
        if badge_mode == "photo":
            badge_mode = "qr"
            show_qr()
            display.update()
        else:
            badge_mode = "photo"
            draw_badge()
            show_photo()
            display.update()
        time.sleep(1)
