import badger2040
from badger2040 import WIDTH

TEXT_SIZE = 0.45
LINE_HEIGHT = 20

display = badger2040.Badger2040()
display.led(128)

display.pen(0)
display.rectangle(0, 0, WIDTH, 16)
display.thickness(1)
display.pen(15)
display.text("badgerOS", 3, 8, 0.4)
display.text("help", WIDTH - display.measure_text("help", 0.4) - 4, 8, 0.4)

display.pen(0)

TEXT_SIZE = 0.62
y = 20 + int(LINE_HEIGHT / 2)

display.thickness(2)
display.font("sans")
display.text("Up/Down - Change page", 0, y, TEXT_SIZE)
y += LINE_HEIGHT
display.text("a, b or c - Launch app", 0, y, TEXT_SIZE)
y += LINE_HEIGHT
display.text("a & c - Exit app", 0, y, TEXT_SIZE)
y += LINE_HEIGHT

y += 8
display.text("Up/Down & User - Font size", 0, y, TEXT_SIZE)
y += LINE_HEIGHT
display.text("a & User - Toggle invert", 0, y, TEXT_SIZE)
y += LINE_HEIGHT

display.update()

# Call halt in a loop, on battery this switches off power.
# On USB, the app will exit when A+C is pressed because the launcher picks that up.
while True:
    display.halt()
