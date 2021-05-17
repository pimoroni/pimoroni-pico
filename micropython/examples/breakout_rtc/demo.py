from breakout_rtc import BreakoutRTC
import time

# rtc = BreakoutRTC(sda=4, scl=5)  # i2c pins 4, 5 for Breakout Garden
rtc = BreakoutRTC()                # Default i2c pins for Pico Explorer

if rtc.is_12_hour():
    rtc.set_24_hour()

rtc.enable_periodic_update_interrupt(True)

while True:
    if rtc.read_periodic_update_interrupt_flag():
        rtc.clear_periodic_update_interrupt_flag()

        if rtc.update_time():
            rtc_date = rtc.string_date()
            rtc_time = rtc.string_time()
            print("Date: ", rtc_date, ", Time: ", rtc_time, sep="")

    time.sleep(0.1)
