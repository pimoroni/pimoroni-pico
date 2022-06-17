import time
import machine
from picographics import PicoGraphics, DISPLAY_TUFTY_2040

rtc = machine.RTC()
display = PicoGraphics(display=DISPLAY_TUFTY_2040)

# Tufty constants.
A = 7
B = 8
C = 15
UP = 22
DOWN = 6
LED = 25

WIDTH, HEIGHT = display.get_bounds()
display.set_backlight(1.0)

# Buttons
button_a = machine.Pin(A, machine.Pin.IN)
button_b = machine.Pin(B, machine.Pin.IN)
button_c = machine.Pin(C, machine.Pin.IN)
button_up = machine.Pin(UP, machine.Pin.IN)
button_down = machine.Pin(DOWN, machine.Pin.IN)

WHITE = display.create_pen(255, 255, 255)
BLACK = display.create_pen(0, 0, 0)

PINK = display.create_pen(214, 28, 78)
ORANGE_1 = display.create_pen(247, 126, 33)
ORANGE_2 = display.create_pen(250, 194, 19)

cursors = ["hour", "minute"]
set_clock = False
cursor = 0
last = 0


def days_in_month(month, year):
    if month == 2 and ((year % 4 == 0 and year % 100 != 0) or year % 400 == 0):
        return 29
    return (31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31)[month - 1]


# Button handling function
def button(pin):
    global last, set_clock, cursor, year, month, day, hour, minute

    time.sleep(0.01)
    if not pin.value():
        return

    if button_a.value() and button_c.value():
        machine.reset()

    adjust = 0
    changed = False

    if pin == button_b:
        set_clock = not set_clock
        changed = True
        if not set_clock:
            rtc.datetime((year, month, day, 0, hour, minute, second, 0))

    if set_clock:
        if pin == button_c:
            cursor += 1
            cursor %= len(cursors)

        if pin == button_a:
            cursor -= 1
            cursor %= len(cursors)

        if pin == button_up:
            adjust = 1

        if pin == button_down:
            adjust = -1

        if cursors[cursor] == "hour":
            hour += adjust
            hour %= 24
        if cursors[cursor] == "minute":
            minute += adjust
            minute %= 60

    if set_clock or changed:
        draw_clock()


# Register the button handling function with the buttons
button_down.irq(trigger=machine.Pin.IRQ_RISING, handler=button)
button_up.irq(trigger=machine.Pin.IRQ_RISING, handler=button)
button_a.irq(trigger=machine.Pin.IRQ_RISING, handler=button)
button_b.irq(trigger=machine.Pin.IRQ_RISING, handler=button)
button_c.irq(trigger=machine.Pin.IRQ_RISING, handler=button)


def draw_clock():

    display.set_pen(WHITE)
    display.clear()

    hr = "{:02}".format(hour)
    min = "{:02}".format(minute)
    sec = "{:02}".format(second)

    hr_width = display.measure_text(hr, 1)
    hr_offset = 15

    minute_width = display.measure_text(min, 1)
    minute_offset = 15

    second_width = display.measure_text(sec, 1)
    second_offset = 5

    display.set_pen(PINK)
    display.rectangle(10, 10, (hour * 13), 60)
    display.set_pen(ORANGE_1)
    display.rectangle(10, 85, (minute * 5), 60)
    display.set_pen(ORANGE_2)
    display.rectangle(10, 160, (second * 5), 60)

    display.set_pen(WHITE)
    display.text(hr, (hour * 13) - hr_width - hr_offset, 45, 10, 3)
    display.text(min, (minute * 5) - minute_width - minute_offset, 120, 10, 3)
    display.text(sec, (second * 5) - second_width - second_offset, 202, 10, 2)

    display.set_pen(BLACK)

    if set_clock:

        if cursors[cursor] == "hour":
            display.line(5, 10, 5, 70)
        if cursors[cursor] == "minute":
            display.line(5, 85, 5, 145)

    display.update()


year, month, day, wd, hour, minute, second, _ = rtc.datetime()

last_second = second

while True:
    if not set_clock:
        year, month, day, wd, hour, minute, second, _ = rtc.datetime()
        if second != last_second:
            draw_clock()
            last_second = second
    time.sleep(0.01)
