import badger2040
import qrcode
import time
import os
import badger_os

# Check that the qrcodes directory exists, if not, make it
try:
    os.mkdir("qrcodes")
except OSError:
    pass

# Load all available QR Code Files
try:
    CODES = [f for f in os.listdir("/qrcodes") if f.endswith(".txt")]
except OSError:
    CODES = []

# create demo QR code file if no QR code files exist
if len(CODES) == 0:
    try:
        newQRcodeFilename = "qrcode.txt"
        text = open("qrcodes/{}".format(newQRcodeFilename), "w")
        text.write("""https://pimoroni.com/badger2040
Badger 2040
* 296x128 1-bit e-ink
* six user buttons
* user LED
* 2MB QSPI flash

Scan this code to learn
more about Badger 2040.
""")
        text.flush()
        text.close()

        # Set the CODES list to contain the newQRcodeFilename (created above)
        CODES = [newQRcodeFilename]

    except:
        CODES = []

TOTAL_CODES = len(CODES)

display = badger2040.Badger2040()

code = qrcode.QRCode()


state = {
    "current_qr": 0
}

def set_state_current_index_in_range():
    badger_os.state_load("qrcodes", state)
    
    if state["current_qr"] >= len(CODES):
        state["current_qr"] = len(CODES) - 1  # set to last index (zero-based). Note: will set to -1 if currently 0

    # check that the index is not negative, thus still out of range
    if state["current_qr"] < 0:
        state["current_qr"] = 0
        
    badger_os.state_save("qrcodes", state)
    

def measure_qr_code(size, code):
    w, h = code.get_size()
    module_size = int(size / w)
    return module_size * w, module_size


def draw_qr_code(ox, oy, size, code):
    size, module_size = measure_qr_code(size, code)
    display.pen(15)
    display.rectangle(ox, oy, size, size)
    display.pen(0)
    for x in range(size):
        for y in range(size):
            if code.get_module(x, y):
                display.rectangle(ox + x * module_size, oy + y * module_size, module_size, module_size)


def draw_qr_file(n):
    display.led(128)
    file = CODES[n]

    try:
        codetext = open("qrcodes/{}".format(file), "r")

        lines = codetext.read().strip().split("\n")
        
        codetext.close()
    except:
        lines = ["", "", "", "", "", "", "", "", "", ""]

    code_text = lines.pop(0)
    title_text = lines.pop(0)
    detail_text = lines

    # Clear the Display
    display.pen(15)  # Change this to 0 if a white background is used
    display.clear()
    display.pen(0)

    code.set_text(code_text)
    size, _ = measure_qr_code(128, code)
    left = top = int((badger2040.HEIGHT / 2) - (size / 2))
    draw_qr_code(left, top, 128, code)

    left = 128 + 5

    display.thickness(2)
    display.text(title_text, left, 20, 0.5)
    display.thickness(1)

    top = 40
    for line in detail_text:
        display.text(line, left, top, 0.4)
        top += 10

    if TOTAL_CODES > 1:
        for i in range(TOTAL_CODES):
            x = 286
            y = int((128 / 2) - (TOTAL_CODES * 10 / 2) + (i * 10))
            display.pen(0)
            display.rectangle(x, y, 8, 8)
            if state["current_qr"] != i:
                display.pen(15)
                display.rectangle(x + 1, y + 1, 6, 6)
    display.update()


set_state_current_index_in_range()
changed = not badger2040.woken_by_button()

while True:
    if TOTAL_CODES > 1:
        if display.pressed(badger2040.BUTTON_UP):
            if state["current_qr"] > 0:
                state["current_qr"] -= 1
                changed = True

        if display.pressed(badger2040.BUTTON_DOWN):
            if state["current_qr"] < TOTAL_CODES - 1:
                state["current_qr"] += 1
                changed = True

    if display.pressed(badger2040.BUTTON_B) or display.pressed(badger2040.BUTTON_C):
        display.pen(15)
        display.clear()
        badger_os.warning(display, "To add QR codes, connect Badger2040 to a PC, load up Thonny, and see qrgen.py.")
        time.sleep(4)
        changed = True

    if changed:
        draw_qr_file(state["current_qr"])
        badger_os.state_save("qrcodes", state)
        changed = False

    # Halt the Badger to save power, it will wake up if any of the front buttons are pressed
    display.halt()
