import machine
from pimoroni_i2c import PimoroniI2C
from pcf85063a import PCF85063A, MONDAY
import time


led = machine.Pin(6, machine.Pin.OUT)
led.high()

vsys_en = machine.Pin(2, machine.Pin.OUT)
vsys_en.high()

print("\n\n")
print("PCF85063A rtc demo!")
print("===================")
time.sleep(0.5)

print("initialise rtc.. ", end="")

PINS = {"sda": 4, "scl": 5}  # i2c pins 4, 5
i2c = PimoroniI2C(**PINS)
rtc = PCF85063A(i2c)

print("done!")
print()

print("check clock function")
print()

print("- setting date and time")
print("  > 30/5/2022 08:07:12")
now = (2022, 5, 30, 8, 7, 12, MONDAY)
rtc.datetime(now)

print()
print("- sleeping for 3 seconds")
time.sleep(3)

print()
print("- reading date and time")
now = rtc.datetime()
print("  >", now)
print()

print("check alarm function")
print()

print("- setting alarm for in 6 seconds")
print("  > 08:07:21")
rtc.set_alarm(21, 7, 8)

print()
print("- waiting for alarm flag")

while not rtc.read_alarm_flag():
    pass

print()
print("- alarm triggered at")
now = rtc.datetime()
print("  >", now)
print()

print("- clear alarm flag")
rtc.clear_alarm_flag()
print()


print("check timer function")
print()

print("- setting timer for in 5 seconds")
rtc.set_timer(5)

print()
print("- waiting for timer flag")

while not rtc.read_timer_flag():
    pass

print()
print("- timer triggered at")
now = rtc.datetime()
print("  >", now)
print()

print("- clear timer flag")
rtc.clear_timer_flag()

print()
print("tests complete")
