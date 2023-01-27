import time
import machine
import ntptime
import badger2040w


display = badger2040w.Badger2040W()
display.set_update_speed(2)
display.set_thickness(4)

WIDTH, HEIGHT = display.get_bounds()

try:
    display.connect()
    if display.isconnected():
        ntptime.settime()
except (RuntimeError, OSError):
    pass  # no WiFI

rtc = machine.RTC()

display.set_font("gothic")


def draw_clock():
    global second_offset, second_unit_offset

    hms = "{:02}:{:02}:{:02}".format(hour, minute, second)
    ymd = "{:04}/{:02}/{:02}".format(year, month, day)

    hms_width = display.measure_text(hms, 1.8)
    hms_offset = int((WIDTH / 2) - (hms_width / 2))

    ymd_width = display.measure_text(ymd, 1.0)
    ymd_offset = int((WIDTH / 2) - (ymd_width / 2))

    display.set_pen(15)
    display.clear()
    display.set_pen(0)

    display.text(hms, hms_offset, 40, 0, 1.8)
    display.text(ymd, ymd_offset, 100, 0, 1.0)

    display.set_update_speed(2)
    display.update()
    display.set_update_speed(3)

    hms = "{:02}:{:02}:".format(hour, minute)
    second_offset = hms_offset + display.measure_text(hms, 1.8)
    hms = "{:02}:{:02}:{}".format(hour, minute, second // 10)
    second_unit_offset = hms_offset + display.measure_text(hms, 1.8)


def draw_second():
    global second_offset, second_unit_offset

    display.set_pen(15)
    display.rectangle(second_offset, 8, 75, 56)
    display.set_pen(0)

    if second // 10 != last_second // 10:
        s = "{:02}".format(second)
        display.text(s, second_offset, 40, 0, 1.8)
        display.partial_update(second_offset, 8, 75, 56)

        s = "{}".format(second // 10)
        second_unit_offset = second_offset + display.measure_text(s, 1.8)

    else:
        s = "{}".format(second % 10)
        display.text(s, second_unit_offset, 40, 0, 1.8)
        display.partial_update(second_unit_offset, 8, 75 - (second_unit_offset - second_offset), 56)


year, month, day, wd, hour, minute, second, _ = rtc.datetime()

if (year, month, day) == (2021, 1, 1):
    rtc.datetime((2022, 2, 28, 0, 12, 0, 0, 0))

last_second = second
last_minute = minute
draw_clock()


while True:
    year, month, day, wd, hour, minute, second, _ = rtc.datetime()
    if second != last_second:
        if minute != last_minute:
            draw_clock()
            last_minute = minute
        else:
            draw_second()
        last_second = second
    time.sleep(0.01)
