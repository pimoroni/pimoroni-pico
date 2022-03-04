import badger2040
import qrcode
import time

display = badger2040.Badger2040()
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


code.set_text("pimoroni.com/badger2040")
size, _ = measure_qr_code(128, code)
left = int((badger2040.WIDTH / 2) - (size / 2))
top = int((badger2040.HEIGHT / 2) - (size / 2))
draw_qr_code(left, top, 128, code)

display.update()

while True:
    time.sleep(1.0)
