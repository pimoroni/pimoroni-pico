import badger2040
from badger2040 import WIDTH

TEXT_SIZE = 0.45
LINE_HEIGHT = 16

display = badger2040.Badger2040()
display.led(128)

display.pen(0)
display.rectangle(0, 0, WIDTH, 16)
display.thickness(1)
display.pen(15)
display.text("badgerOS", 3, 8, 0.4)

display.pen(0)

y = 16 + int(LINE_HEIGHT / 2)

display.thickness(2)
display.text("Normal:", 0, y, TEXT_SIZE)
display.thickness(1)
y += LINE_HEIGHT
display.text("Up / Down - Change launcher page", 0, y, TEXT_SIZE)
y += LINE_HEIGHT
display.text("a, b or c - Launch app", 0, y, TEXT_SIZE)
y += LINE_HEIGHT
y += LINE_HEIGHT

display.thickness(2)
display.text("Hold USER after:", 0, y, TEXT_SIZE)
display.thickness(1)
y += LINE_HEIGHT
display.text("Up / Down - Change font size", 0, y, TEXT_SIZE)
y += LINE_HEIGHT
display.text("a  - Toggle invert", 0, y, TEXT_SIZE)
y += LINE_HEIGHT

display.update()
display.halt()
