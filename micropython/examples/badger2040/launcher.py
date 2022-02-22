import gc
import sys
import time
import math
import machine
import badger2040

page = 0

examples = [
    "clock.py",
    "fonts.py",
    "clean.py",
    "interrupt.py",
    "scanline.py",
    "lut-test.py",
    "quictest.py"
]

MAX_PAGE = math.ceil(len(examples) / 3)

button_a = machine.Pin(badger2040.BUTTON_A, machine.Pin.IN, machine.Pin.PULL_DOWN)
button_b = machine.Pin(badger2040.BUTTON_B, machine.Pin.IN, machine.Pin.PULL_DOWN)
button_c = machine.Pin(badger2040.BUTTON_C, machine.Pin.IN, machine.Pin.PULL_DOWN)
button_up = machine.Pin(badger2040.BUTTON_UP, machine.Pin.IN, machine.Pin.PULL_DOWN)
button_down = machine.Pin(badger2040.BUTTON_DOWN, machine.Pin.IN, machine.Pin.PULL_DOWN)

# Early exit if a button is pressed
if button_a.value() or button_b.value() or button_c.value():
    sys.exit(0)


screen = badger2040.Badger2040()
screen.update_speed(1)


def render():
    screen.pen(15)
    screen.clear()
    screen.pen(0)
    screen.thickness(2)

    max_icons = min(3, len(examples[(page * 3):]))

    for i in range(max_icons):
        x = (14 + 80) * i + 14
        label = examples[i + (page * 3)].replace(".py", "")
        screen.rectangle(x, 24, 80, 80)
        screen.text(label, x, 24 + 80 + 10, 0.5)

    for i in range(MAX_PAGE):
        x = 286
        y = int((128 / 2) - (MAX_PAGE * 10 / 2) + (i * 10))
        screen.pen(0)
        screen.rectangle(x, y, 8, 8)
        if page != i:
            screen.pen(15)
            screen.rectangle(x + 1, y + 1, 6, 6)

    screen.update()


def launch(file):
    for k in locals().keys():
        if k not in ("gc", "file"):
            del locals()[k]
    gc.collect()
    __import__(file)


def launch_example(index):
    try:
        launch(examples[(page * 3) + index])
        return True
    except IndexError:
        return False


def button(pin):
    global page
    if pin == button_a:
        launch_example(0)
    if pin == button_b:
        launch_example(1)
    if pin == button_c:
        launch_example(2)
    if pin == button_up:
        if page > 0:
            page -= 1
            render()
    if pin == button_down:
        if page < MAX_PAGE - 1:
            page += 1
            render()


button_a.irq(trigger=machine.Pin.IRQ_RISING, handler=button)
button_b.irq(trigger=machine.Pin.IRQ_RISING, handler=button)
button_c.irq(trigger=machine.Pin.IRQ_RISING, handler=button)
button_up.irq(trigger=machine.Pin.IRQ_RISING, handler=button)
button_down.irq(trigger=machine.Pin.IRQ_RISING, handler=button)

render()

while True:
    time.sleep(1.0)
