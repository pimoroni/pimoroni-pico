import os
import sys
import time
import badger2040
from badger2040 import HEIGHT
import badger_os


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


# Turn the act LED on as soon as possible
display = badger2040.Badger2040()
display.led(128)

# Try to preload BadgerPunk image
try:
    os.mkdir("images")
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

# Load images
try:
    IMAGES = [f for f in os.listdir("/images") if f.endswith(".bin")]
    TOTAL_IMAGES = len(IMAGES)
except OSError:
    pass


image = bytearray(int(296 * 128 / 8))

state = {
    "current_image": 0,
    "show_info": True
}


def show_image(n):
    file = IMAGES[n]
    name = file.split(".")[0]
    open("images/{}".format(file), "r").readinto(image)
    display.image(image)

    if state["show_info"]:
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
            if state["current_image"] != i:
                display.pen(15)
                display.rectangle(x + 1, y + 1, 6, 6)

    display.update()


if TOTAL_IMAGES == 0:
    display.pen(15)
    display.clear()
    badger_os.warning(display, "To run this demo, create an /images directory on your device and upload some 1bit 296x128 pixel images.")
    time.sleep(4.0)
    sys.exit()


badger_os.state_load("image", state)

changed = not display.woken()


while True:
    if display.pressed(badger2040.BUTTON_UP):
        if state["current_image"] > 0:
            state["current_image"] -= 1
            changed = True
    if display.pressed(badger2040.BUTTON_DOWN):
        if state["current_image"] < TOTAL_IMAGES - 1:
            state["current_image"] += 1
            changed = True
    if display.pressed(badger2040.BUTTON_A):
        state["show_info"] = not state["show_info"]
        changed = True
    if display.pressed(badger2040.BUTTON_B) or display.pressed(badger2040.BUTTON_C):
        display.pen(15)
        display.clear()
        badger_os.warning(display, "To add images connect Badger2040 to a PC, load up Thonny, and see readme.txt in images/")
        display.update()
        print(state["current_image"])
        time.sleep(4)
        changed = True

    if changed:
        badger_os.state_save("image", state)
        show_image(state["current_image"])
        changed = False

    # Halt the Badger to save power, it will wake up if any of the front buttons are pressed
    display.halt()
