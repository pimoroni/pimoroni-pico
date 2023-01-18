import gc
import os
import time
import math
import badger2040w as badger2040
import badger_os
import jpegdec

APP_DIR = "/examples"
FONT_SIZE = 2

changed = False
exited_to_launcher = False
woken_by_button = badger2040.woken_by_button()  # Must be done before we clear_pressed_to_wake

if badger2040.pressed_to_wake(badger2040.BUTTON_A) and badger2040.pressed_to_wake(badger2040.BUTTON_C):
    # Pressing A and C together at start quits app
    exited_to_launcher = badger_os.state_clear_running()
else:
    # Otherwise restore previously running app
    badger_os.state_launch()

# for e.g. 2xAAA batteries, try max 3.4 min 3.0
MAX_BATTERY_VOLTAGE = 4.0
MIN_BATTERY_VOLTAGE = 3.2

display = badger2040.Badger2040W()
display.set_font("bitmap8")
display.led(128)

jpeg = jpegdec.JPEG(display.display)

state = {
    "page": 0,
    "running": "launcher"
}

badger_os.state_load("launcher", state)

examples = [x[:-3] for x in os.listdir("/examples") if x.endswith(".py")]

# Approximate center lines for buttons A, B and C
centers = (41, 147, 253)

MAX_PAGE = math.ceil(len(examples) / 3)

WIDTH = 296


def map_value(input, in_min, in_max, out_min, out_max):
    return (((input - in_min) * (out_max - out_min)) / (in_max - in_min)) + out_min


def draw_battery(level, x, y):
    # Outline
    display.set_pen(15)
    display.rectangle(x, y, 19, 10)
    # Terminal
    display.rectangle(x + 19, y + 3, 2, 4)
    display.set_pen(0)
    display.rectangle(x + 1, y + 1, 17, 8)
    if level < 1:
        display.set_pen(0)
        display.line(x + 3, y, x + 3 + 10, y + 10)
        display.line(x + 3 + 1, y, x + 3 + 11, y + 10)
        display.set_pen(15)
        display.line(x + 2 + 2, y - 1, x + 4 + 12, y + 11)
        display.line(x + 2 + 3, y - 1, x + 4 + 13, y + 11)
        return
    # Battery Bars
    display.set_pen(15)
    for i in range(4):
        if level / 4 > (1.0 * i) / 4:
            display.rectangle(i * 4 + x + 2, y + 2, 3, 6)


def draw_disk_usage(x):
    _, f_used, _ = badger_os.get_disk_usage()

    display.set_pen(15)
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
    display.rectangle(x + 10, 3, 80, 10)
    display.set_pen(0)
    display.rectangle(x + 11, 4, 78, 8)
    display.set_pen(15)
    display.rectangle(x + 12, 5, int(76 / 100.0 * f_used), 6)
    display.text("{:.2f}%".format(f_used), x + 91, 4, WIDTH, 1.0)


def render():
    display.set_pen(15)
    display.clear()
    display.set_pen(0)

    max_icons = min(3, len(examples[(state["page"] * 3):]))

    for i in range(max_icons):
        x = centers[i]
        label = examples[i + (state["page"] * 3)]
        icon = f"{APP_DIR}/icon-{label}.jpg"
        label = label.replace("_", " ")
        jpeg.open_file(icon)
        jpeg.decode(x - 32, 24)
        display.set_pen(0)
        w = display.measure_text(label, FONT_SIZE)
        display.text(label, int(x - (w / 2)), 16 + 80, WIDTH, FONT_SIZE)

    for i in range(MAX_PAGE):
        x = 286
        y = int((128 / 2) - (MAX_PAGE * 10 / 2) + (i * 10))
        display.set_pen(0)
        display.rectangle(x, y, 8, 8)
        if state["page"] != i:
            display.set_pen(15)
            display.rectangle(x + 1, y + 1, 6, 6)

    display.set_pen(0)
    display.rectangle(0, 0, WIDTH, 16)
    draw_disk_usage(90)
    vbat = badger_os.get_battery_level()
    bat = int(map_value(vbat, MIN_BATTERY_VOLTAGE, MAX_BATTERY_VOLTAGE, 0, 4))
    draw_battery(bat, WIDTH - 22 - 3, 3)
    display.set_pen(15)
    display.text("badgerOS", 4, 4, WIDTH, 1.0)

    display.update()


def wait_for_user_to_release_buttons():
    pr = display.pressed
    while pr(badger2040.BUTTON_A) or pr(badger2040.BUTTON_B) or pr(badger2040.BUTTON_C) or pr(badger2040.BUTTON_UP) or pr(badger2040.BUTTON_DOWN):
        time.sleep(0.01)


def launch_example(index):
    wait_for_user_to_release_buttons()

    file = examples[(state["page"] * 3) + index]
    file = f"{APP_DIR}/{file}"

    for k in locals().keys():
        if k not in ("gc", "file", "badger_os"):
            del locals()[k]

    gc.collect()

    badger_os.launch(file)


def button(pin):
    global changed
    changed = True

    if not display.pressed(badger2040.BUTTON_USER):  # User button is NOT held down
        if pin == badger2040.BUTTON_A:
            launch_example(0)
        if pin == badger2040.BUTTON_B:
            launch_example(1)
        if pin == badger2040.BUTTON_C:
            launch_example(2)
        if pin == badger2040.BUTTON_UP:
            if state["page"] > 0:
                state["page"] -= 1
            render()
        if pin == badger2040.BUTTON_DOWN:
            if state["page"] < MAX_PAGE - 1:
                state["page"] += 1
            render()
    else:  # User button IS held down
        if pin == badger2040.BUTTON_UP:
            pass
        if pin == badger2040.BUTTON_DOWN:
            pass
        if pin == badger2040.BUTTON_A:
            pass


if exited_to_launcher or not woken_by_button:
    wait_for_user_to_release_buttons()
    display.set_update_speed(badger2040.UPDATE_MEDIUM)
    render()

display.set_update_speed(badger2040.UPDATE_FAST)

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
        badger_os.state_save("launcher", state)
        changed = False

    display.halt()
