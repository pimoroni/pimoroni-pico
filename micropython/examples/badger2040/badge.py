import time
import badger2040
import badger_os
import os

# Multiple badge text and image files supported. All files to be placed in newly created '\badges' folder.
# Associated image files to be named the same as the text file with '-image' appended.
# For each text file e.g. 'badge1.txt', an associated image file e.g. 'badge1-image.bin' will load

# Global Constants
WIDTH = badger2040.WIDTH
HEIGHT = badger2040.HEIGHT

IMAGE_WIDTH = 104

COMPANY_HEIGHT = 30
DETAILS_HEIGHT = 20
NAME_HEIGHT = HEIGHT - COMPANY_HEIGHT - (DETAILS_HEIGHT * 2) - 2
TEXT_WIDTH = WIDTH - IMAGE_WIDTH - 1

COMPANY_TEXT_SIZE = 0.6
DETAILS_TEXT_SIZE = 0.5

LEFT_PADDING = 5
NAME_PADDING = 20
DETAIL_SPACING = 10

DEFAULT_TEXT = """mustelid inc
H. Badger
RP2040
2MB Flash
E ink
296x128px"""

# Check that the badges directory exists, if not, make it
try:
    os.mkdir("badges")
except OSError:
    pass

# Check that there is a badges.txt, if not preload
try:
    text = open("badges/badge.txt", "r")
except OSError:
    with open("badges/badge.txt", "w") as text:
        text.write(DEFAULT_TEXT)
        text.flush()
        text.seek(0)

# Load all available badge Files
try:
    BADGES = [f for f in os.listdir("/badges") if f.endswith(".txt")]
    TOTAL_BADGES = len(BADGES)
except OSError:
    pass

state = {
    "current_badge": 0
}
# ------------------------------
#      Utility functions
# ------------------------------

# Reduce the size of a string until it fits within a given width
def truncatestring(text, text_size, width):
    while True:
        length = display.measure_text(text, text_size)
        if length > 0 and length > width:
            text = text[:-1]
        else:
            text += ""
            return text


# ------------------------------
#      Drawing functions
# ------------------------------

# Draw the badge, including user text
def draw_badge(n):

    BADGE_IMAGE = bytearray(int(IMAGE_WIDTH * HEIGHT / 8))

    # Reset to default badge if state badge removed
    try:
        file = BADGES[n]
    except:
        n = 0
        file = BADGES[n]
        badger_os.state_modify("badges", {"current badge": n})

    imgfile = str(file).split(".")[0] + "-image.bin"

    # Try and import image related to txt file
    try:
        with open("badges/" + imgfile, "rb") as f:
            f.readinto(BADGE_IMAGE)
    except OSError:
        try:
            import badge_image
            BADGE_IMAGE = bytearray(badge_image.data())
            del badge_image
        except ImportError:
            pass

    with open("badges/{}".format(file), "r") as badge:
        # Read in the next 6 lines
        company = badge.readline()        # "mustelid inc"
        name = badge.readline()           # "H. Badger"
        detail1_title = badge.readline()  # "RP2040"
        detail1_text = badge.readline()   # "2MB Flash"
        detail2_title = badge.readline()  # "E ink"
        detail2_text = badge.readline()   # "296x128px"

    # Truncate all of the text (except for the name as that is scaled)
    company = truncatestring(company, COMPANY_TEXT_SIZE, TEXT_WIDTH)

    detail1_title = truncatestring(detail1_title, DETAILS_TEXT_SIZE, TEXT_WIDTH)
    detail1_text = truncatestring(detail1_text, DETAILS_TEXT_SIZE,
                                TEXT_WIDTH - DETAIL_SPACING - display.measure_text(detail1_title, DETAILS_TEXT_SIZE))

    detail2_title = truncatestring(detail2_title, DETAILS_TEXT_SIZE, TEXT_WIDTH)
    detail2_text = truncatestring(detail2_text, DETAILS_TEXT_SIZE,
                                TEXT_WIDTH - DETAIL_SPACING - display.measure_text(detail2_title, DETAILS_TEXT_SIZE))

    display.pen(0)
    display.clear()

    # Draw badge image
    display.image(BADGE_IMAGE, IMAGE_WIDTH, HEIGHT, WIDTH - IMAGE_WIDTH, 0)

    # Draw a border around the image
    display.pen(0)
    display.thickness(1)
    display.line(WIDTH - IMAGE_WIDTH, 0, WIDTH - 1, 0)
    display.line(WIDTH - IMAGE_WIDTH, 0, WIDTH - IMAGE_WIDTH, HEIGHT - 1)
    display.line(WIDTH - IMAGE_WIDTH, HEIGHT - 1, WIDTH - 1, HEIGHT - 1)
    display.line(WIDTH - 1, 0, WIDTH - 1, HEIGHT - 1)

    # Uncomment this if a white background is wanted behind the company
    # display.pen(15)
    # display.rectangle(1, 1, TEXT_WIDTH, COMPANY_HEIGHT - 1)

    # Draw the company
    display.pen(15)  # Change this to 0 if a white background is used
    display.font("serif")
    display.thickness(3)
    display.text(company, LEFT_PADDING, (COMPANY_HEIGHT // 2) + 1, COMPANY_TEXT_SIZE)

    # Draw a white background behind the name
    display.pen(15)
    display.thickness(1)
    display.rectangle(1, COMPANY_HEIGHT + 1, TEXT_WIDTH, NAME_HEIGHT)

    # Draw the name, scaling it based on the available width
    display.pen(0)
    display.font("sans")
    display.thickness(4)
    name_size = 2.0  # A sensible starting scale
    while True:
        name_length = display.measure_text(name, name_size)
        if name_length >= (TEXT_WIDTH - NAME_PADDING) and name_size >= 0.1:
            name_size -= 0.01
        else:
            display.text(name, (TEXT_WIDTH - name_length) // 2, (NAME_HEIGHT // 2) + COMPANY_HEIGHT + 1, name_size)
            break

    # Draw a white backgrounds behind the details
    display.pen(15)
    display.thickness(1)
    display.rectangle(1, HEIGHT - DETAILS_HEIGHT * 2, TEXT_WIDTH, DETAILS_HEIGHT - 1)
    display.rectangle(1, HEIGHT - DETAILS_HEIGHT, TEXT_WIDTH, DETAILS_HEIGHT - 1)

    # Draw the first detail's title and text
    display.pen(0)
    display.font("sans")
    display.thickness(3)
    name_length = display.measure_text(detail1_title, DETAILS_TEXT_SIZE)
    display.text(detail1_title, LEFT_PADDING, HEIGHT - ((DETAILS_HEIGHT * 3) // 2), DETAILS_TEXT_SIZE)
    display.thickness(2)
    display.text(detail1_text, 5 + name_length + DETAIL_SPACING, HEIGHT - ((DETAILS_HEIGHT * 3) // 2), DETAILS_TEXT_SIZE)

    # Draw the second detail's title and text
    display.thickness(3)
    name_length = display.measure_text(detail2_title, DETAILS_TEXT_SIZE)
    display.text(detail2_title, LEFT_PADDING, HEIGHT - (DETAILS_HEIGHT // 2), DETAILS_TEXT_SIZE)
    display.thickness(2)
    display.text(detail2_text, LEFT_PADDING + name_length + DETAIL_SPACING, HEIGHT - (DETAILS_HEIGHT // 2), DETAILS_TEXT_SIZE)

    # Draw selection indicator to bottom right of image
    if TOTAL_BADGES > 1:
        for i in range(TOTAL_BADGES):
            x = 291
            y = int((128) - (TOTAL_BADGES * 5) + (i * 5))
            display.pen(0)
            display.rectangle(x, y, 4, 4)
            if state["current_badge"] != i:
                display.pen(15)
                display.rectangle(x + 1, y + 1, 2, 2)

# ------------------------------
#        Program setup
# ------------------------------

# Create a new Badger and set it to update NORMAL
display = badger2040.Badger2040()
display.led(128)
display.update_speed(badger2040.UPDATE_NORMAL)

# ------------------------------
#       Main program
# ------------------------------

badger_os.state_load("badges", state)
changed = not badger2040.woken_by_button()

while True:

    if TOTAL_BADGES > 1:
        if display.pressed(badger2040.BUTTON_UP):
            if state["current_badge"] > 0:
                state["current_badge"] -= 1
                changed = True

        if display.pressed(badger2040.BUTTON_DOWN):
            if state["current_badge"] < TOTAL_BADGES - 1:
                state["current_badge"] += 1
                changed = True

    if display.pressed(badger2040.BUTTON_B):
        display.pen(15)
        display.clear()
        badger_os.warning(display, "To add badges, connect Badger2040 to a PC, load up Thonny, and see badges.py.")
        time.sleep(4)
        changed = True

    if changed:
        draw_badge(state["current_badge"])
        badger_os.state_save("badges", state)
        changed = False      

    display.update()

    # If on battery, halt the Badger to save power, it will wake up if any of the front buttons are pressed
    display.halt()
