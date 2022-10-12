"""
Drop this function into your code to set your board's time/date from your RV3028 RTC breakout!
"""


def set_pico_time():
    # the function sets the Pico's RTC from a RV3028 RTC breakout
    # to setup breakout (and set the time) run this first:
    # https://github.com/pimoroni/pimoroni-pico/blob/main/micropython/examples/breakout_rtc/set-time.py
    from pimoroni_i2c import PimoroniI2C
    from breakout_rtc import BreakoutRTC
    from machine import RTC

    # set up I2C
    PINS_BREAKOUT_GARDEN = {"sda": 4, "scl": 5}  # i2c pins 4, 5 for Breakout Garden
    # for boards that use the alternate I2C pins use the line below instead
    # PINS_PICO_EXPLORER = {"sda": 20, "scl": 21}  # Default i2c pins for Pico Explorer

    i2c = PimoroniI2C(**PINS_BREAKOUT_GARDEN)
    # for boards that use the alternate I2C pins use the line below instead
    # i2c = PimoroniI2C(**PINS_PICO_EXPLORER)

    # set up the RTC breakout
    RV3028 = BreakoutRTC(i2c)

    # set the Pico's RTC from the RTC breakout
    RV3028.update_time()
    RTC().datetime([RV3028.get_year(), RV3028.get_month(), RV3028.get_date(),
                    RV3028.get_weekday(), RV3028.get_hours(), RV3028.get_minutes(),
                    RV3028.get_seconds(), 0])
    print(f"Pico RTC set to breakout time: {RV3028.string_date()} {RV3028.string_time()}")


# call the function
set_pico_time()
