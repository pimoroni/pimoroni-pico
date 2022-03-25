import time
import math
import machine
import badger2040
from badger2040 import WIDTH
import launchericons
import badger_os

# for e.g. 2xAAA batteries, try max 3.4 min 3.0
MAX_BATTERY_VOLTAGE = 4.0
MIN_BATTERY_VOLTAGE = 3.2

# Reduce clock speed to 48MHz, that's fast enough!
machine.freq(48000000)


# Restore previously running app
try:
    # Pressing A and C together at start quits app
    if badger2040.pressed_to_wake(badger2040.BUTTON_A) and badger2040.pressed_to_wake(badger2040.BUTTON_C):
        badger_os.state_delete()
    else:
        if badger_os.state_app() != "launcher":
            badger_os.state_launch()
except OSError:
    pass
except ImportError:
    # Happens if appstate names an unknown app.  Delete appstate and reset
    badger_os.state_delete()
    machine.reset()


display = badger2040.Badger2040()

page, font_size, inverted = badger_os.state_load("launcher", 0, 1, False)
changed = badger_os.state_app() != "launcher"

icons = bytearray(launchericons.data())
icons_width = 576

examples = [
    ("_clock", 0),
    ("_fonts", 1),
    ("_ebook", 2),
    ("_image", 3),
    ("_list", 4),
    ("_badge", 5),
    ("_qrgen", 8),
    ("_info", 6),
    ("_help", 7),
]

font_sizes = (0.5, 0.7, 0.9)

# Approximate center lines for buttons A, B and C
centers = (41, 147, 253)

MAX_PAGE = math.ceil(len(examples) / 3)


def map_value(input, in_min, in_max, out_min, out_max):
    return (((input - in_min) * (out_max - out_min)) / (in_max - in_min)) + out_min


def draw_battery(level, x, y):
    # Outline
    display.thickness(1)
    display.pen(15)
    display.rectangle(x, y, 19, 10)
    # Terminal
    display.rectangle(x + 19, y + 3, 2, 4)
    display.pen(0)
    display.rectangle(x + 1, y + 1, 17, 8)
    if level < 1:
        display.pen(0)
        display.line(x + 3, y, x + 3 + 10, y + 10)
        display.line(x + 3 + 1, y, x + 3 + 11, y + 10)
        display.pen(15)
        display.line(x + 2 + 2, y - 1, x + 4 + 12, y + 11)
        display.line(x + 2 + 3, y - 1, x + 4 + 13, y + 11)
        return
    # Battery Bars
    display.pen(15)
    for i in range(4):
        if level / 4 > (1.0 * i) / 4:
            display.rectangle(i * 4 + x + 2, y + 2, 3, 6)


def draw_disk_usage(x):
    _, f_used, _ = badger_os.get_disk_usage()

    display.image(
        bytearray(
            (
                0b00000000,
                0b00111100,
                0b00111100,
                0b00111100,
                0b00111000,
                0b00000000,
                0b00000000,
                0b00000001,
            )
        ),
        8,
        8,
        x,
        4,
    )
    display.pen(15)
    display.rectangle(x + 10, 3, 80, 10)
    display.pen(0)
    display.rectangle(x + 11, 4, 78, 8)
    display.pen(15)
    display.rectangle(x + 12, 5, int(76 / 100.0 * f_used), 6)
    display.text("{:.2f}%".format(f_used), x + 91, 8, 0.4)


def render():
    display.pen(15)
    display.clear()
    display.pen(0)
    display.thickness(2)

    max_icons = min(3, len(examples[(page * 3):]))

    for i in range(max_icons):
        x = centers[i]
        label, icon = examples[i + (page * 3)]
        label = label[1:].replace("_", " ")
        display.pen(0)
        display.icon(icons, icon, icons_width, 64, x - 32, 24)
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
    display.rectangle(0, 0, WIDTH, 16)
    display.thickness(1)
    draw_disk_usage(90)
    vbat = badger_os.get_battery_level()
    bat = int(map_value(vbat, MIN_BATTERY_VOLTAGE, MAX_BATTERY_VOLTAGE, 0, 4))
    draw_battery(bat, WIDTH - 22 - 3, 3)
    display.pen(15)
    display.text("badgerOS", 3, 8, 0.4)

    display.update()


def launch_example(index):
    while display.pressed(badger2040.BUTTON_A) or display.pressed(badger2040.BUTTON_B) or display.pressed(badger2040.BUTTON_C) or display.pressed(badger2040.BUTTON_UP) or display.pressed(badger2040.BUTTON_DOWN):
        time.sleep(0.01)
    try:
        badger_os.launch(examples[(page * 3) + index][0])
        return True
    except IndexError:
        return False


def button(pin):
    global page, font_size, inverted, changed
    changed = True

    if not display.pressed(badger2040.BUTTON_USER):  # User button is NOT held down
        if pin == badger2040.BUTTON_A:
            launch_example(0)
        if pin == badger2040.BUTTON_B:
            launch_example(1)
        if pin == badger2040.BUTTON_C:
            launch_example(2)
        if pin == badger2040.BUTTON_UP:
            if page > 0:
                page -= 1
            render()
        if pin == badger2040.BUTTON_DOWN:
            if page < MAX_PAGE - 1:
                page += 1
            render()
    else:  # User button IS held down
        if pin == badger2040.BUTTON_UP:
            font_size += 1
            if font_size == len(font_sizes):
                font_size = 0
            render()
        if pin == badger2040.BUTTON_DOWN:
            font_size -= 1
            if font_size < 0:
                font_size = 0
            render()
        if pin == badger2040.BUTTON_A:
            inverted = not inverted
            display.invert(inverted)
            render()


if changed:
    # Wait for any wakeup button to be released
    while display.pressed(badger2040.BUTTON_A) or display.pressed(badger2040.BUTTON_B) or display.pressed(badger2040.BUTTON_C) or display.pressed(badger2040.BUTTON_UP) or display.pressed(badger2040.BUTTON_DOWN):
        time.sleep(0.01)
    display.update_speed(badger2040.UPDATE_MEDIUM)
    render()

display.update_speed(badger2040.UPDATE_FAST)
if not changed and not display.woken():
    render()

while True:
    if display.pressed(badger2040.BUTTON_A):
        button(badger2040.BUTTON_A)
    if display.pressed(badger2040.BUTTON_B):
        button(badger2040.BUTTON_B)
    if display.pressed(badger2040.BUTTON_C):
        button(badger2040.BUTTON_C)

    if display.pressed(badger2040.BUTTON_UP):
        button(badger2040.BUTTON_UP)
    if display.pressed(badger2040.BUTTON_DOWN):
        button(badger2040.BUTTON_DOWN)

    if changed:
        badger_os.state_save("launcher", page, font_size, inverted)
        changed = False

    display.halt()
