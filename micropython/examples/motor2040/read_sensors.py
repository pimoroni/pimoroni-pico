import time
from machine import Pin
from pimoroni import Analog, AnalogMux, Button
from motor import motor2040

"""
Shows how to initialise and read the 2 external
and 6 internal sensors of Motor 2040.

Press "Boot" to exit the program.
"""

# Set up the shared analog inputs
sen_adc = Analog(motor2040.SHARED_ADC)
vol_adc = Analog(motor2040.SHARED_ADC, motor2040.VOLTAGE_GAIN)
cur_adc = Analog(motor2040.SHARED_ADC, motor2040.CURRENT_GAIN,
                 motor2040.SHUNT_RESISTOR, motor2040.CURRENT_OFFSET)

# Set up the analog multiplexer, including the pin for controlling pull-up/pull-down
mux = AnalogMux(motor2040.ADC_ADDR_0, motor2040.ADC_ADDR_1, motor2040.ADC_ADDR_2,
                muxed_pin=Pin(motor2040.SHARED_ADC))

# Set up the sensor addresses and have them pulled down by default
sensor_addrs = list(range(motor2040.SENSOR_1_ADDR, motor2040.SENSOR_2_ADDR + 1))
for addr in sensor_addrs:
    mux.configure_pull(addr, Pin.PULL_DOWN)

# Set up the pull-up for the fault sense
mux.configure_pull(motor2040.FAULT_SENSE_ADDR, Pin.PULL_UP)

# Create the user button
user_sw = Button(motor2040.USER_SW)


# Read sensors until the user button is pressed
while not user_sw.raw():

    # Read each sensor in turn and print its voltage
    for i in range(len(sensor_addrs)):
        mux.select(sensor_addrs[i])
        print("S", i + 1, " = ", round(sen_adc.read_voltage(), 3), sep="", end=", ")

    # Read the voltage sense and print the value
    mux.select(motor2040.VOLTAGE_SENSE_ADDR)
    print("Voltage =", round(vol_adc.read_voltage(), 4), end=", ")

    # Read the current sense's of each motor and print the value
    for i in range(motor2040.NUM_MOTORS):
        mux.select(i + motor2040.CURRENT_SENSE_A_ADDR)
        print("C", i + 1, " = ", cur_adc.read_current(), sep="", end=", ")

    # Read the fault sense and print the value
    mux.select(motor2040.FAULT_SENSE_ADDR)
    print("Fault =", not mux.read())

    time.sleep(0.5)
