import gc
import sys
import time
import math
import machine
import badger2040
import launchericons

page = 0
font_size = 1

icons = bytearray(launchericons.data())

examples = [
    ("clock.py", 0),
    ("fonts.py", 1),
    ("ebook.py", 2),
    ("image.py", 3),
    ("list.py", 4),
    ("badge.py", 5)
]

font_sizes = (0.5, 0.7, 0.9)

# Approximate center lines for buttons A, B and C
centers = (41, 147, 253)

MAX_PAGE = math.ceil(len(examples) / 3)

button_a = machine.Pin(badger2040.BUTTON_A, machine.Pin.IN, machine.Pin.PULL_DOWN)
button_b = machine.Pin(badger2040.BUTTON_B, machine.Pin.IN, machine.Pin.PULL_DOWN)
button_c = machine.Pin(badger2040.BUTTON_C, machine.Pin.IN, machine.Pin.PULL_DOWN)
button_up = machine.Pin(badger2040.BUTTON_UP, machine.Pin.IN, machine.Pin.PULL_DOWN)
button_down = machine.Pin(badger2040.BUTTON_DOWN, machine.Pin.IN, machine.Pin.PULL_DOWN)

# Inverted. For reasons.
button_user = machine.Pin(badger2040.BUTTON_USER, machine.Pin.IN, machine.Pin.PULL_UP)

display = badger2040.Badger2040()
display.update_speed(1)


def render():
    display.pen(15)
    display.clear()
    display.pen(0)
    display.thickness(2)

    max_icons = min(3, len(examples[(page * 3):]))

    for i in range(max_icons):
        x = centers[i]
        label, icon = examples[i + (page * 3)]
        label = label.replace(".py", "")
        display.pen(0)
        display.icon(icons, icon, 384, 64, x - 32, 24)
        w = display.measure_text(label, font_sizes[font_size])
        display.text(label, x - int(w / 2), 16 + 80, font_sizes[font_size])

    for i in range(MAX_PAGE):
        x = 286
        y = int((128 / 2) - (MAX_PAGE * 10 / 2) + (i * 10))
        display.pen(0)
        display.rectangle(x, y, 8, 8)
        if page != i:
            display.pen(15)
            display.rectangle(x + 1, y + 1, 6, 6)

    display.pen(0)

    display.update()


def launch(file):
    for k in locals().keys():
        if k not in ("gc", "file"):
            del locals()[k]
    gc.collect()
    __import__(file)
    sys.exit(0)


def launch_example(index):
    try:
        launch(examples[(page * 3) + index][0])
        return True
    except IndexError:
        return False


def button(pin):
    global page, font_size
    if pin == button_user:
        font_size += 1
        if font_size == len(font_sizes):
            font_size = 0
        render()
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


render()

# Wait for wakeup button to be released
while button_a.value() or button_b.value() or button_c.value() or button_up.value() or button_down.value():
    pass


while True:
    if button_a.value():
        button(button_a)
    if button_b.value():
        button(button_b)
    if button_c.value():
        button(button_c)
    if button_up.value():
        button(button_up)
    if button_down.value():
        button(button_down)
    if not button_user.value():
        button(button_user)

    time.sleep(0.01)
