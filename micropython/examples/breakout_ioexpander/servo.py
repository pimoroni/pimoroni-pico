import time
import math
from pimoroni_i2c import PimoroniI2C
from breakout_ioexpander import BreakoutIOExpander

PINS_BREAKOUT_GARDEN = {"sda": 4, "scl": 5}
PINS_PICO_EXPLORER = {"sda": 20, "scl": 21}

ioe_servo_pin = 1

# Settings to produce a 50Hz output from the 24MHz clock.
# 24,000,000 Hz / 8 = 3,000,000 Hz
# 3,000,000 Hz / 60,000 Period = 50 Hz
divider = 8
period = 60000
cycle_time = 5.0
servo_range = 1000.0    # Between 1000 and 2000us (1-2ms)

i2c = PimoroniI2C(**PINS_BREAKOUT_GARDEN)
ioe = BreakoutIOExpander(i2c, address=0x18)

ioe.set_pwm_period(period)
ioe.set_pwm_control(divider)

ioe.set_mode(ioe_servo_pin, BreakoutIOExpander.PIN_PWM)

t = 0

while True:
    s = math.sin((t * math.pi * 2.0) / cycle_time) / 2.0
    servo_us = 1500.0 + (s * servo_range)

    duty_per_microsecond = period / (20 * 1000)    # Default is 3 LSB per microsecond

    duty_cycle = round(servo_us * duty_per_microsecond)
    print("Cycle Time: ", t % cycle_time, ", Pulse: ", servo_us, ", Duty Cycle: ", duty_cycle, sep="")
    ioe.output(ioe_servo_pin, duty_cycle)

    time.sleep(0.02)
    t += 0.02
