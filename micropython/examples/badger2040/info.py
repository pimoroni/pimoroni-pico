import badger2040
from badger2040 import WIDTH

TEXT_SIZE = 0.45
LINE_HEIGHT = 16

display = badger2040.Badger2040()
display.pen(0)
display.rectangle(0, 0, WIDTH, 16)
display.thickness(1)
display.pen(15)
display.text("badgerOS", 3, 8, 0.4)

display.pen(0)

y = 16 + int(LINE_HEIGHT / 2)

display.text("Made by Pimoroni, powered by MicroPython", 0, y, TEXT_SIZE)
y += LINE_HEIGHT
display.text("Dual-core RP2040, 133MHz, 264KB RAM", 0, y, TEXT_SIZE)
y += LINE_HEIGHT
display.text("2MB Flash (1MB OS, 1MB Storage)", 0, y, TEXT_SIZE)
y += LINE_HEIGHT
display.text("296x128 pixel Black/White e-Ink", 0, y, TEXT_SIZE)
y += LINE_HEIGHT
y += LINE_HEIGHT

display.thickness(2)
display.text("For more info:", 0, y, TEXT_SIZE)
display.thickness(1)
y += LINE_HEIGHT
display.text("https://pimoroni.com/badger2040", 0, y, TEXT_SIZE)

display.update()
