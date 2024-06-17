# RV3028 Breakout (C/C++)

An ultra-low-power ( ~100 nA), highly accurate real-time clock breakout. The RV3028 RTC breakout is perfect for adding timekeeping to your project and, thanks to the tiny on-board battery, it'll keep time when your device is powered off. Like all the best timepieces, it's Swiss-made! 

Available here: https://shop.pimoroni.com/products/rv3028-real-time-clock-rtc-breakout

# **Example Program**

Example program shows how to setup the RV3028, set the date and time and output it once a second.

```
#include "pico/stdlib.h"
#include "breakout_rtc.hpp"

using namespace pimoroni;

I2C i2c(BOARD::BREAKOUT_GARDEN);
BreakoutRTC rtc(&i2c);

int main() {
	
	stdio_init_all();

	if(!rtc.init()) {
		printf("Init failed! Check your connections and i2c pin choices.\n");
	}

	rtc.enable_periodic_update_interrupt(true);

	rtc.set_time(10, 10, 10, 7, 12, 9, 2024);

	while(true) {
		if(rtc.read_periodic_update_interrupt_flag()) {
			rtc.clear_periodic_update_interrupt_flag();

			if(rtc.update_time()) {
				printf("Date: %s, Time: %s\n", rtc.string_date(), rtc.string_time());

			}
		}

	sleep_ms(100);

}

return 0;

}
```

# **Functions**

## `bool init()`

Initializes the RV3028. Returns `true` if successful or `false` if something went wrong.

**Example** 

```
I2C i2c(BOARD::BREAKOUT_GARDEN);
BreakoutRTC rtc(&i2c);

rtc.init();
```

## `bool setup()`

Sets the RTC to 24 Hour Mode, disables the Trickle Charge and sets Level Switching Mode to 3.

## `bool set_time(uint8_t sec, uint8_t min, uint8_t hour, uint8_t weekday, uint8_t date, uint8_t month, uint16_t year)`

Set the time and date on the RTC. Returns `true` if successful and `false` if something went wrong.

You can also set a time or date element individually with:

`set_seconds(uint8_t value)`, `set_minutes(uint8_t value)`, `set_hours(uint8_t value)`, `set_weekday(uint8_t value)`, `set_date(uint8_t value)`, `set_month(uint8_t value)` and `set_year(uint8_t value)`


## `void set_24_hour()`

Set the RTC to output 0-23 Hours. This function will convert any current hour to 2 Hour Mode.

## `void set_12_hour()`

Set the RTC to output 1-12 Hours. This function will convert any current hour to 12 Hour Mode.

## `bool is_12_hour()`

Returns `true` if the RV3028 has been configured for 12 Hour mode.

## `bool is_pm()`

Returns `true` if the RTC has the PM bit set and the 12 Hour bit set

## `bool update_time()`

This function moves the data from the RV3028 registers to an array. You need to call this function before printing the time and date. The function returns `true` if successful and `false` if something went wrong.

## `char* string_time()`

Returns a pointer to array of chars that represents the time in hh:mm:ss format. Adds AM/PM if in 12 hour mode. You will need to call `update_time()` to get the current time from the RTC before using this function.

## `char* string_date()`

Returns a pointer to array of chars that are the date in dd/mm/yyyy format. You will need to call `update_time()` to get the current date from the RTC before using this function.

## `void enable_periodic_update_interrupt(bool every_second, bool enable_clock_output)`

Enables the periodic update interrupt.

## `void disable_periodic_update_interrupt()`

Disables the periodic update interrupt.

## `bool read_periodic_update_interrupt_flag()`

Read the periodic update interrupt flag. Returns a boolean value.

## `void clear_periodic_update_interrupt_flag()`

Clear the periodic update interrupt flag.

## `uint32_t get_unix()`

Get the UNIX Time from the RTC

## `bool set_unix(uint32_t value)`

Set the UNIX Time on the RTC. Returns `true` if successful and `false` if something went wrong.

**IMPORTANT:** The Real Time and the UNIX time are independent of each other. Setting the UNIX time will not update the Real Time (Hours, minutes, seconds etc)

## `bool set_backup_switchover_mode(uint8_t val)`

Set the backup switchover mode of the RTC. Accepts values 0-3.

0 = Switchover disabled
1 = Direct Switching Mode
2 = Standby Mode
3 = Level Switching Mode

Function returns `true` if successful and `false` if something went wrong.



