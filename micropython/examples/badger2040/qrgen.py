import badger2040
import qrcode
import time
import machine
import os
import badger_os

# Check that the qrcodes directory exists, if not, make it
try:
    os.mkdir("qrcodes")
except OSError:
    pass

# Check that there is a qrcode.txt, if not preload
try:
    text = open("qrcodes/qrcode.txt", "r")
except OSError:
    text = open("qrcodes/qrcode.txt", "w")
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
    text.seek(0)

# Load all available QR Code Files
try:
    CODES = [f for f in os.listdir("/qrcodes") if f.endswith(".txt")]
    TOTAL_CODES = len(CODES)
except OSError:
    pass


print(f'There are {TOTAL_CODES} QR Codes available:')
for codename in CODES:
    print(f'File: {codename}')

display = badger2040.Badger2040()

code = qrcode.QRCode()


button_a = machine.Pin(badger2040.BUTTON_A, machine.Pin.IN, machine.Pin.PULL_DOWN)
button_b = machine.Pin(badger2040.BUTTON_B, machine.Pin.IN, machine.Pin.PULL_DOWN)
button_c = machine.Pin(badger2040.BUTTON_C, machine.Pin.IN, machine.Pin.PULL_DOWN)

button_up = machine.Pin(badger2040.BUTTON_UP, machine.Pin.IN, machine.Pin.PULL_DOWN)
button_down = machine.Pin(badger2040.BUTTON_DOWN, machine.Pin.IN, machine.Pin.PULL_DOWN)


state = {
    "current_qr": 0
}


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
    codetext = open("qrcodes/{}".format(file), "r")

    lines = codetext.read().strip().split("\n")
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

    if(TOTAL_CODES > 1):
        for i in range(TOTAL_CODES):
            x = 286
            y = int((128 / 2) - (TOTAL_CODES * 10 / 2) + (i * 10))
            display.pen(0)
            display.rectangle(x, y, 8, 8)
            if state["current_qr"] != i:
                display.pen(15)
                display.rectangle(x + 1, y + 1, 6, 6)
    display.update()


badger_os.state_load("qrcodes", state)
changed = not badger2040.woken_by_button()

while True:
    if(TOTAL_CODES > 1):
        if button_up.value():
            if state["current_qr"] > 0:
                state["current_qr"] -= 1
                changed = True

        if button_down.value():
            if state["current_qr"] < TOTAL_CODES - 1:
                state["current_qr"] += 1
                changed = True

    if button_b.value() or button_c.value():
        display.pen(15)
        display.clear()
        badger_os.warning(display, "To add QR codes, connect Badger2040 to a PC, load up Thonny, and see qrgen.py.")
        time.sleep(4)
        changed = True

    if changed:
        badger_os.state_save("qrcodes", state)
        draw_qr_file(state["current_qr"])
        changed = False

    # Halt the Badger to save power, it will wake up if any of the front buttons are pressed
    display.led(0)
    display.halt()
