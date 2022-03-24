import os
import sys
import time
import machine
import badger2040
from badger2040 import WIDTH, HEIGHT

README = """
Images must be 296x128 pixel with 1bit colour depth.

You can use examples/badger2040/image_converter/convert.py to convert them:

python3 convert.py --binary --resize image_file_1.png image_file_2.png image_file_3.png

Create a new "images" directory via Thonny, and upload the .bin files there.
"""

OVERLAY_BORDER = 40
OVERLAY_SPACING = 20
OVERLAY_TEXT_SIZE = 0.5

TOTAL_IMAGES = 0

# I am using fastrefresh to keep track of which screen refresh speed I have selected using the C button.
# 1 = MEDIUM. 2 = NORMAL. 3 = FAST. 4 = TURBO. Turbo is very very fast but gross. Fast is the best of both. I hate slow.
fastrefresh = 3

inverted = False

# This makes the /images folder if it does not already exist.
try:
    os.mkdir("images")
except OSError:
    pass

# This puts the badger photo and readme text file in the /images folder if they do not already exist.
try:
    import badgerpunk
    with open("images/badgerpunk.bin", "wb") as f:
        f.write(badgerpunk.data())
        f.flush()
    with open("images/readme.txt", "w") as f:
        f.write(README)
        f.flush()
    del badgerpunk
except (OSError, ImportError):
    pass

# This loads up an array with the images in the /images folder.
try:
    IMAGES = [f for f in os.listdir("/images") if f.endswith(".bin")]
    TOTAL_IMAGES = len(IMAGES)
except OSError:
    pass

display = badger2040.Badger2040()

# This is setting the initial screen refresh speed.
display.update_speed(badger2040.UPDATE_FAST)

button_a = machine.Pin(badger2040.BUTTON_A, machine.Pin.IN, machine.Pin.PULL_DOWN)
button_b = machine.Pin(badger2040.BUTTON_B, machine.Pin.IN, machine.Pin.PULL_DOWN)
button_c = machine.Pin(badger2040.BUTTON_C, machine.Pin.IN, machine.Pin.PULL_DOWN)
button_up = machine.Pin(badger2040.BUTTON_UP, machine.Pin.IN, machine.Pin.PULL_DOWN)
button_down = machine.Pin(badger2040.BUTTON_DOWN, machine.Pin.IN, machine.Pin.PULL_DOWN)
# Inverted. For reasons.
button_user = machine.Pin(badger2040.BUTTON_USER, machine.Pin.IN, machine.Pin.PULL_UP)

image = bytearray(int(296 * 128 / 8))

# This is the first photo that will be shown.
current_image = 0

# This sets whether the filename and count is shown by default.
show_info = False


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

show_image(current_image)

# Button Loop
while True:
    # Up/Down Buttons for Photo Navigation
    # (I am not putting those under the User button shift)
    if button_up.value():
        if current_image > 0:
            current_image -= 1
        else:
            current_image = TOTAL_IMAGES - 1
        show_image(current_image)
    if button_down.value():
        if current_image < TOTAL_IMAGES - 1:
            current_image += 1
        else:
            current_image = 0
        show_image(current_image)
    # End Up/Down Buttons

    # Start ABC Buttons, with fancy User button modifier
    if button_user.value():  # User button is NOT held down
        if button_a.value():
            show_info = not show_info
            show_image(current_image)

        if button_b.value():
            inverted = not inverted
            display.invert(inverted)
            show_image(current_image)

        if button_c.value():
            display.pen(15)
            display.clear()
            if fastrefresh == 3:
                fastrefresh = 4
                display.update_speed(badger2040.UPDATE_FAST)
                draw_overlay("TURBO SCREEN SPEED! AAAAAA", WIDTH - OVERLAY_BORDER, HEIGHT - OVERLAY_BORDER, OVERLAY_SPACING, 0.5)
                display.update()
                time.sleep(2)
                display.update_speed(badger2040.UPDATE_TURBO)
            else:
                fastrefresh = 3
                display.update_speed(badger2040.UPDATE_FAST)
                draw_overlay("Default Fast Refresh Speed", WIDTH - OVERLAY_BORDER, HEIGHT - OVERLAY_BORDER, OVERLAY_SPACING, 0.5)
                display.update()
                time.sleep(2)
            show_image(current_image)

    else:  # User button IS held down
        if button_a.value():
            display.update_speed(badger2040.UPDATE_TURBO)
            show_image(current_image)
            display.update_speed(badger2040.UPDATE_FAST)

        if button_b.value():
            display.pen(15)
            display.clear()
            draw_overlay("Exiting Image Viewer! o3o", WIDTH - OVERLAY_BORDER, HEIGHT - OVERLAY_BORDER, OVERLAY_SPACING, 0.5)
            display.update()
            time.sleep(2)
            machine.reset()

        if button_c.value():
            display.pen(15)
            display.clear()
            if fastrefresh == 1:
                fastrefresh = 2
                display.update_speed(badger2040.UPDATE_FAST)
                draw_overlay("Slowest refresh speed", WIDTH - OVERLAY_BORDER, HEIGHT - OVERLAY_BORDER, OVERLAY_SPACING, 0.5)
                display.update()
                time.sleep(2)
                display.update_speed(badger2040.UPDATE_NORMAL)
            else:
                fastrefresh = 1
                display.update_speed(badger2040.UPDATE_FAST)
                draw_overlay("Medium screen refresh!", WIDTH - OVERLAY_BORDER, HEIGHT - OVERLAY_BORDER, OVERLAY_SPACING, 0.5)
                display.update()
                time.sleep(2)
                display.update_speed(badger2040.UPDATE_MEDIUM)
            show_image(current_image)

    time.sleep(0.01)
