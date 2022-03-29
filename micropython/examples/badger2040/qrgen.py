import badger2040
import qrcode


# Open the qrcode file
try:
    text = open("qrcode.txt", "r")
except OSError:
    with open("qrcode.txt", "w") as text:
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
    text = open("qrcode.txt", "r")


lines = text.read().strip().split("\n")
code_text = lines.pop(0)
title_text = lines.pop(0)
detail_text = lines

display = badger2040.Badger2040()
display.led(128)
code = qrcode.QRCode()


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

display.update()

# Call halt in a loop, on battery this switches off power.
# On USB, the app will exit when A+C is pressed because the launcher picks that up.
while True:
    display.halt()
