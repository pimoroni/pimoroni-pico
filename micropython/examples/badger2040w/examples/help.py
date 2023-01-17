import badger2040w
from badger2040w import WIDTH

TEXT_SIZE = 0.45
LINE_HEIGHT = 20

display = badger2040w.Badger2040W()
display.led(128)

# Clear to white
display.set_pen(15)
display.clear()

display.set_font("bitmap8")
display.set_pen(0)
display.rectangle(0, 0, WIDTH, 16)
display.set_pen(15)
display.text("badgerOS", 3, 4, WIDTH, 1)
display.text("help", WIDTH - display.measure_text("help", 0.4) - 4, 4, WIDTH, 1)

display.set_font("sans")
display.set_pen(0)

TEXT_SIZE = 0.62
y = 20 + int(LINE_HEIGHT / 2)

display.set_font("sans")
display.text("Up/Down - Change page", 0, y, WIDTH, TEXT_SIZE)
y += LINE_HEIGHT
display.text("a, b or c - Launch app", 0, y, WIDTH, TEXT_SIZE)
y += LINE_HEIGHT
display.text("a & c - Exit app", 0, y, WIDTH, TEXT_SIZE)
y += LINE_HEIGHT

display.update()

# Call halt in a loop, on battery this switches off power.
# On USB, the app will exit when A+C is pressed because the launcher picks that up.
while True:
    display.halt()
