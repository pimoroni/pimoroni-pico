import badger2040
import qrcode
import time
import machine
import os

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


OVERLAY_BORDER = 40
OVERLAY_SPACING = 20
OVERLAY_TEXT_SIZE = 0.5


button_a = machine.Pin(badger2040.BUTTON_A, machine.Pin.IN, machine.Pin.PULL_DOWN)
button_b = machine.Pin(badger2040.BUTTON_B, machine.Pin.IN, machine.Pin.PULL_DOWN)
button_c = machine.Pin(badger2040.BUTTON_C, machine.Pin.IN, machine.Pin.PULL_DOWN)

button_up = machine.Pin(badger2040.BUTTON_UP, machine.Pin.IN, machine.Pin.PULL_DOWN)
button_down = machine.Pin(badger2040.BUTTON_DOWN, machine.Pin.IN, machine.Pin.PULL_DOWN)

current_qr = 0


# Draw an overlay box with a given message within it
def draw_overlay(message, width, height, line_spacing, text_size):

    # Draw a light grey background
    display.pen(12)
    display.rectangle((badger2040.WIDTH - width) // 2, (badger2040.HEIGHT - height) // 2, width, height)

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
        display.text(lines[i], (badger2040.WIDTH - length) // 2, (badger2040.HEIGHT // 2) + current_line, text_size)


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
    file = CODES[n]
    # print(f'Reading file: {file}')
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
            if current_qr != i:
                display.pen(15)
                display.rectangle(x + 1, y + 1, 6, 6)
    display.update()


draw_qr_file(current_qr)

while True:
    if(TOTAL_CODES > 1):
        if button_up.value():
            if current_qr > 0:
                current_qr -= 1
            draw_qr_file(current_qr)
        if button_down.value():
            if current_qr < TOTAL_CODES - 1:
                current_qr += 1
            draw_qr_file(current_qr)
    if button_b.value() or button_c.value():
        display.pen(15)
        display.clear()
        draw_overlay("To add QR codes, connect Badger2040 to a PC, load up Thonny, and see qrgen.py", badger2040.WIDTH - OVERLAY_BORDER, badger2040.HEIGHT - OVERLAY_BORDER, OVERLAY_SPACING, 0.5)
        display.update()
        time.sleep(4)
        draw_qr_file(current_qr)
    time.sleep(0.01)
