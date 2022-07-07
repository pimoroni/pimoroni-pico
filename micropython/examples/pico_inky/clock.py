import time
import machine
from picographics import PicoGraphics, DISPLAY_INKY_PACK


# Buttons
button_a = machine.Pin(12, machine.Pin.IN, pull=machine.Pin.PULL_UP)
button_b = machine.Pin(13, machine.Pin.IN, pull=machine.Pin.PULL_UP)
button_c = machine.Pin(14, machine.Pin.IN, pull=machine.Pin.PULL_UP)

# Display
graphics = PicoGraphics(DISPLAY_INKY_PACK)
WIDTH, HEIGHT = graphics.get_bounds()
graphics.set_update_speed(3)
graphics.set_font("gothic")

# RTC
rtc = machine.RTC()

cursors = ["hour", "minute", "year", "month", "day", "finish"]
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

    adjust = 0
    changed = False

    time.sleep(0.01)
    if pin.value():
        return

    if pin == button_b and not set_clock:
        cursor = 0
        set_clock = True
        draw_clock()
        return

    if set_clock:
        if pin == button_b:
            cursor += 1
            cursor %= len(cursors)
            changed = True

        if pin == button_a:
            adjust = 1
            changed = True

        if pin == button_c:
            adjust = -1
            changed = True

        if cursors[cursor] == "finish":
            if adjust != 0:
                set_clock = False
                changed = True
                if not set_clock:
                    rtc.datetime((year, month, day, 0, hour, minute, second, 0))

        if cursors[cursor] == "year":
            year += adjust
            year = max(year, 2022)
            day = min(day, days_in_month(month, year))

        if cursors[cursor] == "month":
            month += adjust
            month = min(max(month, 1), 12)
            day = min(day, days_in_month(month, year))

        if cursors[cursor] == "day":
            day += adjust
            day = min(max(day, 1), days_in_month(month, year))

        if cursors[cursor] == "hour":
            hour += adjust
            hour %= 24

        if cursors[cursor] == "minute":
            minute += adjust
            minute %= 60

    if changed:
        draw_clock()


button_a.irq(trigger=machine.Pin.IRQ_FALLING, handler=button)
button_b.irq(trigger=machine.Pin.IRQ_FALLING, handler=button)
button_c.irq(trigger=machine.Pin.IRQ_FALLING, handler=button)


def draw_clock():
    hms = "{:02}:{:02}:{:02}".format(hour, minute, second)
    ymd = "{:04}/{:02}/{:02}".format(year, month, day)

    hms_width = graphics.measure_text(hms, 1.8)
    hms_offset = int((WIDTH / 2) - (hms_width / 2))
    h_width = graphics.measure_text(hms[0:2], 1.8)
    mi_width = graphics.measure_text(hms[3:5], 1.8)
    mi_offset = graphics.measure_text(hms[0:3], 1.8)

    ymd_width = graphics.measure_text(ymd, 1.0)
    ymd_offset = int((WIDTH / 2) - (ymd_width / 2))
    y_width = graphics.measure_text(ymd[0:4], 1.0)
    m_width = graphics.measure_text(ymd[5:7], 1.0)
    m_offset = graphics.measure_text(ymd[0:5], 1.0)
    d_width = graphics.measure_text(ymd[8:10], 1.0)
    d_offset = graphics.measure_text(ymd[0:8], 1.0)

    graphics.set_pen(15)
    graphics.clear()
    graphics.set_pen(0)

    # No "thickness" setting in PG so, uh, fake it!
    graphics.text(hms, hms_offset, 40, scale=1.8)
    graphics.text(hms, hms_offset, 41, scale=1.8)
    graphics.text(hms, hms_offset + 1, 40, scale=1.8)
    graphics.text(hms, hms_offset - 1, 40, scale=1.8)

    # Double up the text to fill out the lines
    graphics.text(ymd, ymd_offset, 100, scale=1.0)
    graphics.text(ymd, ymd_offset, 101, scale=1.0)

    if set_clock:
        if cursors[cursor] == "year":
            graphics.line(ymd_offset, 120, ymd_offset + y_width, 120)

        if cursors[cursor] == "month":
            graphics.line(ymd_offset + m_offset, 120, ymd_offset + m_offset + m_width, 120)

        if cursors[cursor] == "day":
            graphics.line(ymd_offset + d_offset, 120, ymd_offset + d_offset + d_width, 120)

        if cursors[cursor] == "hour":
            graphics.line(hms_offset, 70, hms_offset + h_width, 70)

        if cursors[cursor] == "minute":
            graphics.line(hms_offset + mi_offset, 70, hms_offset + mi_offset + mi_width, 70)

        done_width = graphics.measure_text("done", scale=0.5)
        graphics.text("done", WIDTH - done_width - 5, HEIGHT - 15, scale=0.5)
        if cursors[cursor] == "finish":
            graphics.line(WIDTH - done_width - 5, HEIGHT - 5, WIDTH - 5, HEIGHT - 5)

    graphics.update()


year, month, day, wd, hour, minute, second, _ = rtc.datetime()

if (year, month, day) == (2021, 1, 1):
    rtc.datetime((2022, 2, 28, 0, 12, 0, 0, 0))

last_second = second

while True:
    if not set_clock:
        year, month, day, wd, hour, minute, second, _ = rtc.datetime()
        if second != last_second:
            draw_clock()
            last_second = second
    time.sleep(0.01)
