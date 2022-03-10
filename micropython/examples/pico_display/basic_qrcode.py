import picodisplay as display  # Comment this line out to use PicoDisplay2
# import picodisplay2 as display  # Uncomment this line to use PicoDisplay2
import qrcode


def measure_qr_code(size, code):
    w, h = code.get_size()
    module_size = int(size / w)
    return module_size * w, module_size


def draw_qr_code(ox, oy, size, code):
    size, module_size = measure_qr_code(size, code)
    display.set_pen(128, 128, 128)
    display.rectangle(ox, oy, size, size)
    display.set_pen(0, 0, 0)
    for x in range(size):
        for y in range(size):
            if code.get_module(x, y):
                display.rectangle(ox + x * module_size, oy + y * module_size, module_size, module_size)


width = display.get_width()
height = display.get_height()

code = qrcode.QRCode()
code.set_text("shop.pimoroni.com")

display.init(bytearray(width * height * 2))

display.set_pen(128, 128, 128)
display.clear()
display.set_pen(0, 0, 0)

size, module_size = measure_qr_code(height, code)
left = int((width // 2) - (size // 2))
top = int((height // 2) - (size // 2))
draw_qr_code(left, top, height, code)

display.update()

while True:
    pass
