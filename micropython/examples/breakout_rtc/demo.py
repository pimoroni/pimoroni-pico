from breakout_rtc import BreakoutRTC

rtc = BreakoutRTC()

if rtc.is_12_hour():
    rtc.set_24_hour()

rtc.enable_periodic_update_interrupt(True)

while True:
    if rtc.read_periodic_update_interrupt_flag():
        rtc.clear_periodic_update_interrupt_flag()

        if rtc.update_time():
            date = rtc.string_date()
            time = rtc.string_time()
            print("Date: ", date, ", Time: ", time, sep="")