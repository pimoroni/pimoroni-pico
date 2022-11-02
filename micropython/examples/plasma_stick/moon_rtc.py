import time
import plasma
from plasma import plasma_stick
from machine import RTC

"""
Spooky moon simulator!
The LEDs will get brighter as midnight approaches!
It won't do much in the day...
Gets the time from a connected RV3028 RTC breakout:
https://shop.pimoroni.com/products/rv3028-real-time-clock-rtc-breakout
"""

# Set how many LEDs you have
NUM_LEDS = 50

# pick a hue (0 - 360° on the colour wheel)
# warm white moon - 60, blue moon - 230 , blood moon - 0
HUE = 60
SATURATION = 0.2  # increase this for a more colourful moon (max 1.0)

# when to start counting down from, in seconds before midnight
# eg from 10pm = 2 hours = 2 * 60 * 60 = 7200
COUNT_FROM = 14400


def set_pico_time():
    # the function sets the Pico's RTC from a RV3028 RTC breakout
    # to setup breakout (and set the time) run this first:
    # https://github.com/pimoroni/pimoroni-pico/blob/main/micropython/examples/breakout_rtc/set-time.py
    from pimoroni_i2c import PimoroniI2C
    from breakout_rtc import BreakoutRTC
    from machine import RTC

    # set up I2C
    i2c = PimoroniI2C(plasma_stick.SDA, plasma_stick.SCL)

    # set up the RTC breakout
    RV3028 = BreakoutRTC(i2c)

    # set the Pico's RTC from the RTC breakout
    RV3028.update_time()
    RTC().datetime([RV3028.get_year(), RV3028.get_month(), RV3028.get_date(),
                    RV3028.get_weekday(), RV3028.get_hours(), RV3028.get_minutes(),
                    RV3028.get_seconds(), 0])
    print(f"Pico RTC set to breakout time: {RV3028.string_date()} {RV3028.string_time()}")


# set up the WS2812 / NeoPixel™ LEDs
led_strip = plasma.WS2812(NUM_LEDS, 0, 0, plasma_stick.DAT, color_order=plasma.COLOR_ORDER_RGB)

# start updating the LED strip
led_strip.start()

# call our function to set the Pico's RTC
set_pico_time()

while True:
    # get the time from Pico RTC
    year, month, day, _, hour, minute, second, _ = RTC().datetime()
    print(f'Time is {hour:02d}:{minute:02d}')

    # calculate how long to go until midnight
    if hour >= 12:
        hours_to_go = 23 - hour
        minutes_to_go = 59 - minute
        seconds_to_go = 59 - second
        total_seconds = hours_to_go * 60 * 60 + minutes_to_go * 60 + seconds_to_go
        print(f'{total_seconds} seconds until midnight')
    # or, if it's after midnight
    else:
        hours_since = 0 + hour
        minutes_since = 0 + minute
        seconds_since = 0 + second
        total_seconds = hours_since * 60 * 60 + minutes_since * 60 + seconds_since
        print(f'{total_seconds} seconds since midnight')

    # gets brighter as witching hour approacheth
    brightness = max(0, (COUNT_FROM - total_seconds) / COUNT_FROM)
    for i in range(NUM_LEDS):
        led_strip.set_hsv(i, HUE / 360.0, SATURATION, brightness)
    print(f'Brightness - {brightness * 100} %')
    time.sleep(10.0)
