# This example grabs the time/date from your Pico's RTC and uses it to set the time on your RV3028 RTC breakout
# Your Pico/RP2040 board will only know the correct time if you're running this script through Thonny!

from pimoroni_i2c import PimoroniI2C
from breakout_rtc import BreakoutRTC
import machine

PINS_BREAKOUT_GARDEN = {"sda": 4, "scl": 5}  # i2c pins 4, 5 for Breakout Garden
PINS_PICO_EXPLORER = {"sda": 20, "scl": 21}  # Default i2c pins for Pico Explorer

rtcpico = machine.RTC()
i2c = PimoroniI2C(**PINS_BREAKOUT_GARDEN)
rtcbreakout = BreakoutRTC(i2c)

# this sets up the battery switching mode on your breakout
rtcbreakout.setup()

print(f"Getting time from Pico RTC/Thonny: {rtcpico.datetime()}")
year, month, day, weekday, hour, minute, second, microsecond = rtcpico.datetime()

print("Setting the breakout RTC!")
rtcbreakout.set_time(second, minute, hour, weekday, day, month, year)

print(f"New breakout time: {rtcbreakout.string_date()} {rtcbreakout.string_time()}")
