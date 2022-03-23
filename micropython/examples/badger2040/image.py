import os
import sys
import time
import machine
import badger2040
from badger2040 import WIDTH, HEIGHT


REAMDE = """
Images must be 296x128 pixel with 1bit colour depth.

You can use examples/badger2040/image_converter/convert.py to convert them:

python3 convert.py --binary --resize image_file_1.png image_file_2.png image_file_3.png

Create a new "images" directory via Thonny, and upload the .bin files there.
"""

OVERLAY_BORDER = 40
OVERLAY_SPACING = 20
OVERLAY_TEXT_SIZE = 0.5

TOTAL_IMAGES = 0

# Try to preload BadgerPunk image
try:
    os.mkdir("images")
except OSError:
    pass

try:
    import badgerpunk
    with open("images/badgerpunk.bin", "wb") as f:
        f.write(badgerpunk.data())
        f.flush()
    with open("images/readme.txt", "w") as f:
        f.write(REAMDE)
        f.flush()
    del badgerpunk
except (OSError, ImportError):
    pass

try:
    IMAGES = [f for f in os.listdir("/images") if f.endswith(".bin")]
    TOTAL_IMAGES = len(IMAGES)
except OSError:
    pass


display = badger2040.Badger2040()

image = bytearray(int(296 * 128 / 8))
current_image = 0
show_info = True

try:
    with open("appstate.txt", "r") as f:
        f.readline()
        current_image = int(f.readline().strip('\n'))
        show_info = f.readline().strip('\n') == "True"
except OSError:
    pass

# Draw an overlay box with a given message within it
def draw_overlay(message, width, height, line_spacing, text_size):

    # Draw a light grey background
    display.pen(12)
    display.rectangle((WIDTH - width) // 2, (HEIGHT - height) // 2, width, height)

    # Take the provided message and split it up into
    # lines that fit within the specified width
    words = message.split(" ")

    lines = []
    current_line = ""
    for word in words:
        if display.measure_text(current_line + word + " ", text_size) < width:
            current_line += word + " "
        else:
            lines.append(current_line.strip())
            current_line = word + " "
    lines.append(current_line.strip())

    display.pen(0)
    display.thickness(2)

    # Display each line of text from the message, centre-aligned
    num_lines = len(lines)
    for i in range(num_lines):
        length = display.measure_text(lines[i], text_size)
        current_line = (i * line_spacing) - ((num_lines - 1) * line_spacing) // 2
        display.text(lines[i], (WIDTH - length) // 2, (HEIGHT // 2) + current_line, text_size)


def show_image(n):
    file = IMAGES[n]
    name = file.split(".")[0]
    open("images/{}".format(file), "r").readinto(image)
    display.image(image)

    if show_info:
        name_length = display.measure_text(name, 0.5)
        display.pen(0)
        display.rectangle(0, HEIGHT - 21, name_length + 11, 21)
        display.pen(15)
        display.rectangle(0, HEIGHT - 20, name_length + 10, 20)
        display.pen(0)
        display.text(name, 5, HEIGHT - 10, 0.5)

        for i in range(TOTAL_IMAGES):
            x = 286
            y = int((128 / 2) - (TOTAL_IMAGES * 10 / 2) + (i * 10))
            display.pen(0)
            display.rectangle(x, y, 8, 8)
            if current_image != i:
                display.pen(15)
                display.rectangle(x + 1, y + 1, 6, 6)

    display.update()


if TOTAL_IMAGES == 0:
    display.pen(15)
    display.clear()
    draw_overlay("To run this demo, create an /images directory on your device and upload some 1bit 296x128 pixel images.", WIDTH - OVERLAY_BORDER, HEIGHT - OVERLAY_BORDER, OVERLAY_SPACING, OVERLAY_TEXT_SIZE)
    display.update()
    sys.exit()

if display.pressed_to_wake(badger2040.BUTTON_UP):
    if current_image > 0:
        current_image -= 1
if display.pressed_to_wake(badger2040.BUTTON_DOWN):
    if current_image < TOTAL_IMAGES - 1:
        current_image += 1
if display.pressed_to_wake(badger2040.BUTTON_A):
    show_info = not show_info
if display.pressed_to_wake(badger2040.BUTTON_B) or display.pressed_to_wake(badger2040.BUTTON_C):
    display.pen(15)
    display.clear()
    draw_overlay("To add images connect Badger2040 to a PC, load up Thonny, and see readme.txt in images/", WIDTH - OVERLAY_BORDER, HEIGHT - OVERLAY_BORDER, OVERLAY_SPACING, 0.5)
    display.update()
    time.sleep(4)

show_image(current_image)


with open("appstate.txt", "w") as f:
    f.write("image\n")
    f.write("{}\n{}\n".format(current_image,show_info))

display.halt()

