import time
import machine
import ntptime
import badger2040w


display = badger2040w.Badger2040W()
display.set_update_speed(3)
display.set_thickness(4)

WIDTH, HEIGHT = display.get_bounds()

try:
    display.connect()
    if display.isconnected():
        ntptime.settime()
except RuntimeError:
    pass  # no WiFI

rtc = machine.RTC()

display.set_font("gothic")


def draw_clock():
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

    display.update()


year, month, day, wd, hour, minute, second, _ = rtc.datetime()

if (year, month, day) == (2021, 1, 1):
    rtc.datetime((2022, 2, 28, 0, 12, 0, 0, 0))

last_second = second


while True:
    year, month, day, wd, hour, minute, second, _ = rtc.datetime()
    if second != last_second:
        draw_clock()
        last_second = second
    time.sleep(0.01)
