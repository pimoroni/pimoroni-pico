import os
import sys
import time
import badger2040w
from badger2040w import HEIGHT, WIDTH
import badger_os
import jpegdec


REAMDE = """
Images must be 296x128 pixel JPEGs

Create a new "images" directory via Thonny, and upload your .jpg files there.
"""

OVERLAY_BORDER = 40
OVERLAY_SPACING = 20
OVERLAY_TEXT_SIZE = 0.5

TOTAL_IMAGES = 0


# Turn the act LED on as soon as possible
display = badger2040w.Badger2040W()
display.led(128)

jpeg = jpegdec.JPEG(display.display)

# Try to preload BadgerPunk image
try:
    os.mkdir("/images")
    with open("/images/readme.txt", "w") as f:
        f.write(REAMDE)
        f.flush()
except (OSError, ImportError):
    pass

# Load images
try:
    IMAGES = [f for f in os.listdir("/images") if f.endswith(".jpg")]
    TOTAL_IMAGES = len(IMAGES)
except OSError:
    pass


state = {
    "current_image": 0,
    "show_info": True
}


def show_image(n):
    file = IMAGES[n]
    name = file.split(".")[0]
    jpeg.open_file("/images/{}".format(file))
    jpeg.decode()

    if state["show_info"]:
        name_length = display.measure_text(name, 0.5)
        display.set_pen(0)
        display.rectangle(0, HEIGHT - 21, name_length + 11, 21)
        display.set_pen(15)
        display.rectangle(0, HEIGHT - 20, name_length + 10, 20)
        display.set_pen(0)
        display.text(name, 5, HEIGHT - 10, WIDTH, 0.5)

        for i in range(TOTAL_IMAGES):
            x = 286
            y = int((128 / 2) - (TOTAL_IMAGES * 10 / 2) + (i * 10))
            display.set_pen(0)
            display.rectangle(x, y, 8, 8)
            if state["current_image"] != i:
                display.set_pen(15)
                display.rectangle(x + 1, y + 1, 6, 6)

    display.update()


if TOTAL_IMAGES == 0:
    display.set_pen(15)
    display.clear()
    badger_os.warning(display, "To run this demo, create an /images directory on your device and upload some 1bit 296x128 pixel images.")
    time.sleep(4.0)
    sys.exit()


badger_os.state_load("image", state)

changed = not badger2040w.woken_by_button()


while True:
    if display.pressed(badger2040w.BUTTON_UP):
        if state["current_image"] > 0:
            state["current_image"] -= 1
            changed = True
    if display.pressed(badger2040w.BUTTON_DOWN):
        if state["current_image"] < TOTAL_IMAGES - 1:
            state["current_image"] += 1
            changed = True
    if display.pressed(badger2040w.BUTTON_A):
        state["show_info"] = not state["show_info"]
        changed = True
    if display.pressed(badger2040w.BUTTON_B) or display.pressed(badger2040w.BUTTON_C):
        display.set_pen(15)
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
