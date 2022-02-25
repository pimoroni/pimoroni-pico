import gc
import os
import sys
import time
import math
import machine
import badger2040
from badger2040 import WIDTH
import launchericons

MAX_BATTERY_VOLTAGE = 4.0
MIN_BATTERY_VOLTAGE = 3.2


page = 0
font_size = 1
inverted = False

icons = bytearray(launchericons.data())

examples = [
    ("_clock.py", 0),
    ("_fonts.py", 1),
    ("_ebook.py", 2),
    ("_image.py", 3),
    ("_list.py", 4),
    ("_badge.py", 5),
    ("_info.py", 6),
    ("_help.py", 7)
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

# Battery measurement
vbat_adc = machine.ADC(badger2040.PIN_BATTERY)
vref_adc = machine.ADC(badger2040.PIN_1V2_REF)
vref_en = machine.Pin(badger2040.PIN_VREF_POWER)
vref_en.init(machine.Pin.OUT)
vref_en.value(0)


display = badger2040.Badger2040()


def map_value(input, in_min, in_max, out_min, out_max):
    return (((input - in_min) * (out_max - out_min)) / (in_max - in_min)) + out_min


def get_battery_level():
    # Enable the onboard voltage reference
    vref_en.value(1)

    # Calculate the logic supply voltage, as will be lower that the usual 3.3V when running off low batteries
    vdd = 1.24 * (65535 / vref_adc.read_u16())
    vbat = (vbat_adc.read_u16() / 65535) * 3 * vdd  # 3 in this is a gain, not rounding of 3.3V

    # Disable the onboard voltage reference
    vref_en.value(0)

    # Convert the voltage to a level to display onscreen
    return int(map_value(vbat, MIN_BATTERY_VOLTAGE, MAX_BATTERY_VOLTAGE, 0, 4))


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
    # f_bfree and f_bavail should be the same?
    # f_files, f_ffree, f_favail and f_flag are unsupported.
    f_bsize, f_frsize, f_blocks, f_bfree, _, _, _, _, _, f_namemax = os.statvfs("/")

    f_total_size = f_frsize * f_blocks
    f_total_free = f_bsize * f_bfree
    f_total_used = f_total_size - f_total_free

    f_used = 100 / f_total_size * f_total_used
    # f_free = 100 / f_total_size * f_total_free

    display.image(bytearray((
        0b00000000,
        0b00111100,
        0b00111100,
        0b00111100,
        0b00111000,
        0b00000000,
        0b00000000,
        0b00000001)), 8, 8, x, 4)
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
        label = label[1:-3]
        display.pen(0)
        display.icon(icons, icon, 512, 64, x - 32, 24)
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
    draw_battery(get_battery_level(), WIDTH - 22 - 3, 3)
    display.pen(15)
    display.text("badgerOS", 3, 8, 0.4)

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
    global page, font_size, inverted

    if button_user.value():  # User button is NOT held down
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
    else:  # User button IS held down
        if pin == button_up:
            font_size += 1
            if font_size == len(font_sizes):
                font_size = 0
            render()
        if pin == button_down:
            font_size -= 1
            if font_size < 0:
                font_size = 0
            render()
        if pin == button_a:
            inverted = not inverted
            display.invert(inverted)
            render()


display.update_speed(badger2040.UPDATE_MEDIUM)
render()
display.update_speed(badger2040.UPDATE_FAST)


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

    time.sleep(0.01)
