import os
import sys
import time
import machine
import badger2040
from badger2040 import WIDTH, HEIGHT


REAMDE = """
Images must be 296x128 pixel with 1bit colour depth.

You can use examples/badger200/image_converter/convert.py to convert them:

python3 convert.py --binary --resize image_file_1.png image_file_2.png image_file_3.png

Create a new "images" directory via Thonny, and upload the .bin files there.
"""

OVERLAY_BORDER = 40
OVERLAY_SPACING = 20
OVERLAY_TEXT_SIZE = 0.5

TOTAL_IMAGES = 0

# Try to preload awful Phil image
try:
    os.mkdir("images")
except OSError:
    pass

try:
    import phil
    with open("images/phil.bin", "wb") as f:
        f.write(phil.data())
        f.flush()
    with open("images/readme.txt", "w") as f:
        f.write(REAMDE)
        f.flush()
    del phil
except (OSError, ImportError):
    pass

try:
    IMAGES = [f for f in os.listdir("/images") if f.endswith(".bin")]
    TOTAL_IMAGES = len(IMAGES)
except OSError:
    pass


display = badger2040.Badger2040()

button_a = machine.Pin(badger2040.BUTTON_A, machine.Pin.IN, machine.Pin.PULL_DOWN)
button_b = machine.Pin(badger2040.BUTTON_B, machine.Pin.IN, machine.Pin.PULL_DOWN)
button_c = machine.Pin(badger2040.BUTTON_C, machine.Pin.IN, machine.Pin.PULL_DOWN)

button_up = machine.Pin(badger2040.BUTTON_UP, machine.Pin.IN, machine.Pin.PULL_DOWN)
button_down = machine.Pin(badger2040.BUTTON_DOWN, machine.Pin.IN, machine.Pin.PULL_DOWN)

image = bytearray(int(296 * 128 / 8))
current_image = 0


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


show_image(current_image)


while True:
    if button_up.value():
        if current_image > 0:
            current_image -= 1
        show_image(current_image)
    if button_down.value():
        if current_image < TOTAL_IMAGES - 1:
            current_image += 1
        show_image(current_image)
    if button_a.value() or button_b.value() or button_c.value():
        display.pen(15)
        display.clear()
        draw_overlay("To add images connect Badger2040 to a PC, load up Thonny, and see readme.txt in images/", WIDTH - OVERLAY_BORDER, HEIGHT - OVERLAY_BORDER, OVERLAY_SPACING, 0.5)
        display.update()
        time.sleep(4)
        show_image(current_image)

    time.sleep(0.01)
