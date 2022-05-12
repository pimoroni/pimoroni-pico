import st7789
import qrcode

# Set the display resolution
# in most cases you can swap WIDTH weith HEIGHT for portrait mode
WIDTH, HEIGHT = 240, 135    # Pico Display
# WIDTH, HEIGHT = 320, 240  # Pico Display 2.0

display = st7789.ST7789(WIDTH, HEIGHT, rotate180=False)


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


code = qrcode.QRCode()
code.set_text("shop.pimoroni.com")

display.set_pen(128, 128, 128)
display.clear()
display.set_pen(0, 0, 0)

size, module_size = measure_qr_code(HEIGHT, code)
left = int((WIDTH // 2) - (size // 2))
top = int((HEIGHT // 2) - (size // 2))
draw_qr_code(left, top, HEIGHT, code)

display.update()

while True:
    pass
