import time
import badger2040
from machine import RTC

rtc = RTC()
screen = badger2040.Badger2040()
screen.update_speed(badger2040.UPDATE_TURBO)
screen.font("gothic")

year, month, day, wd, hour, minute, second, _ = rtc.datetime()

if (year, month, day) == (2021, 1, 1):
    rtc.datetime((2022, 2, 25, 3, 12, 0, 0, 0))

while True:
    year, month, day, wd, hour, minute, second, _ = rtc.datetime()
    hms = "{:02}:{:02}:{:02}".format(hour, minute, second)
    ymd = "{:04}/{:02}/{:02}".format(year, month, day)

    screen.pen(15)
    screen.clear()
    screen.pen(0)
    screen.thickness(5)
    screen.text(hms, 0, 40, 1.8)
    screen.thickness(3)
    screen.text(ymd, 0, 100, 1.0)

    screen.update()

    time.sleep(0.5)
