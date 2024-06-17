# RV3028 Breakout (MicroPython)

An ultra-low-power ( ~100 nA), highly accurate real-time clock breakout. The RV3028 RTC breakout is perfect for adding timekeeping to your project and, thanks to the tiny on-board battery, it'll keep time when your device is powered off. Like all the best timepieces, it's Swiss-made! 

Available here: https://shop.pimoroni.com/products/rv3028-real-time-clock-rtc-breakout

# **Example Program**

Example program shows how to setup the RV3028, set the date and time and output it once a second.

```
from pimoroni_i2c import PimoroniI2C
from breakout_rtc import BreakoutRTC

PINS_BREAKOUT_GARDEN = {"sda": 4, "scl": 5}  # i2c pins 4, 5 for Breakout Garden

i2c = PimoroniI2C(**PINS_BREAKOUT_GARDEN)
rtc = BreakoutRTC(i2c)

rtc.setup()
rtc.enable_periodic_update_interrupt(True)

rtc.set_time(10, 10, 10, 5, 1, 1, 2001)

while True:
    
    if (rtc.read_periodic_update_interrupt_flag()):
        rtc.clear_periodic_update_interrupt_flag()
        
        rtc.update_time()
        rtc_time = rtc.string_time()
        print(rtc_time)

```

# **Functions**

## `.setup()`

Sets the RTC to 24 Hour Mode, disables the Trickle Charge and sets Level Switching Mode to 3.

## `.set_time(second, minute, hour, weekday, day, month, year)`

Set the time and date on the RTC. Returns **True** if successful and **False** if something went wrong.

**Example:**

```
>>> rtc.set_time(0, 52, 8, 4, 5, 6, 2024)
True
>>> rtc.update_time()
True
>>> print(rtc.string_time(), rtc.string_date())
08:52:13 05/06/2024
```

You can also set a time or date element individually with:

`.set_seconds()`, `.set_minutes()`, `.set_hours()`, `.set_weekday()`, `.set_date()`, `.set_month()` and `.set_year()`


## `.set_24_hour()`

Set the RTC to output 0-23 Hours. This function will convert any current hour to 2 Hour Mode.

## `.set_12_hour()`

Set the RTC to output 1-12 Hours. This function will convert any current hour to 12 Hour Mode.

## `.is_12_hour()`

Returns **True** if the RV3028 has been configured for 12 Hour mode.

```
>>> rtc.is_12_hour()
False
>>> rtc.set_12_hour()
>>> rtc.is_12_hour()
True
>>> 
```

### `.is_pm()`

Returns **True** if the RTC has the PM bit set and the 12 Hour bit set

**Example:**
```
>>> rtc.is_pm()
False
```

## `.update_time()`

This function moves the data from the RV3028 registers to an array. You need to call this function before printing the time and date. The function returns **True** if successful and **False** if something went wrong.

**Example**:

```
while True:
    
    rtc.update_time()
    rtc_time = rtc.string_time()
    print(rtc_time)

    time.sleep(1)
```
**Output**:

```
10:10:25
10:10:26
10:10:27
```

## `.string_time()`

Returns the time as a formatted String. You will need to call `.update_time()` to get the current time from the RTC before using this function.

**Example:**
```
>>> rtc.string_time()
'05:05:05'
```

## `.string_date()`

Returns the date as a formatted String. You will need to call `.update_time()` to get the current date from the RTC before using this function.

**Example:**
```
>>> rtc.string_date()
'05/05/2005'
```

## `.enable_periodic_update_interrupt(True)`

Enables the periodic update interrupt. Triggers onces every second.

## `.disable_periodic_update_interrupt()`

Disables the periodic update interrupt.

## `.read_periodic_update_interrupt_flag()`

Read the periodic update interrupt flag. Returns a boolean value.

**Example:**

```
>>> rtc.read_periodic_update_interrupt_flag()
True
```

## `.clear_periodic_update_interrupt_flag()`

Clear the periodic update interrupt flag.

**Example:**
```
>>> rtc.clear_periodic_update_interrupt_flag()
>>> rtc.read_periodic_update_interrupt_flag()
False
```

## `.get_unix()`

Get the UNIX Time from the RTC

**IMPORTANT:** The Real Time and the UNIX time are independent of each other. Retrieving the UNIX Time will not give you the Real Time stored by the RTC.

**Example:**
```
>>> rtc.get_unix()
3084
```

## `.set_unix()`

Set the UNIX Time on the RTC. Returns **True** if successful and **False** if something went wrong.

**IMPORTANT:** The Real Time and the UNIX time are independent of each other. Setting the UNIX time will not update the Real Time (Hours, minutes, seconds etc)

**Example:**
```
>>> rtc.set_unix(1717508760)
True
>>> rtc.get_unix()
1717508765
```

## `.set_backup_switchover_mode(int value)`

Set the backup switchover mode of the RTC. Accepts values 0-3.

0 = Switchover disabled
1 = Direct Switching Mode
2 = Standby Mode
3 = Level Switching Mode

Function returns **True** if successful and **False** if something went wrong.



