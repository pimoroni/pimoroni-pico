import time
from machine import Pin
from pimoroni import Analog, AnalogMux, Button
from servo import servo2040

"""
Shows how to initialise and read the 6 external
and 2 internal sensors of Servo 2040.

Press "Boot" to exit the program.
"""

# Set up the shared analog inputs
sen_adc = Analog(servo2040.SHARED_ADC)
vol_adc = Analog(servo2040.SHARED_ADC, servo2040.VOLTAGE_GAIN)
cur_adc = Analog(servo2040.SHARED_ADC, servo2040.CURRENT_GAIN,
                 servo2040.SHUNT_RESISTOR, servo2040.CURRENT_OFFSET)

# Set up the analog multiplexer, including the pin for controlling pull-up/pull-down
mux = AnalogMux(servo2040.ADC_ADDR_0, servo2040.ADC_ADDR_1, servo2040.ADC_ADDR_2,
                muxed_pin=Pin(servo2040.SHARED_ADC))

# Set up the sensor addresses and have them pulled down by default
sensor_addrs = list(range(servo2040.SENSOR_1_ADDR, servo2040.SENSOR_6_ADDR + 1))
for addr in sensor_addrs:
    mux.configure_pull(addr, Pin.PULL_DOWN)

# Create the user button
user_sw = Button(servo2040.USER_SW)


# Read sensors until the user button is pressed
while not user_sw.raw():

    # Read each sensor in turn and print its voltage
    for i in range(len(sensor_addrs)):
        mux.select(sensor_addrs[i])
        print("S", i + 1, " = ", round(sen_adc.read_voltage(), 3), sep="", end=", ")

    # Read the voltage sense and print the value
    mux.select(servo2040.VOLTAGE_SENSE_ADDR)
    print("Voltage =", round(vol_adc.read_voltage(), 4), end=", ")

    # Read the current sense and print the value
    mux.select(servo2040.CURRENT_SENSE_ADDR)
    print("Current =", round(cur_adc.read_current(), 4))

    time.sleep(0.5)
