import time
from machine import Pin
from pimoroni import Analog, AnalogMux, Button
from plasma import WS2812
from servo import servo2040

"""
Show how to read the 6 external sensors and
display their values on the neighbouring LEDs.

Press "Boot" to exit the program.

NOTE: Plasma WS2812 uses the RP2040's PIO system, and as
such may have problems when running code multiple times.
If you encounter issues, try resetting your board.
"""

BRIGHTNESS = 0.4      # The brightness of the LEDs
UPDATES = 50          # How many times to update LEDs and Servos per second

# Set up the shared analog inputs
sen_adc = Analog(servo2040.SHARED_ADC)

# Set up the analog multiplexer, including the pin for controlling pull-up/pull-down
mux = AnalogMux(servo2040.ADC_ADDR_0, servo2040.ADC_ADDR_1, servo2040.ADC_ADDR_2,
                muxed_pin=Pin(servo2040.SHARED_ADC))

# Set up the sensor addresses and have them pulled down by default
sensor_addrs = list(range(servo2040.SENSOR_1_ADDR, servo2040.SENSOR_6_ADDR + 1))
for addr in sensor_addrs:
    mux.configure_pull(addr, Pin.PULL_DOWN)

# Create the LED bar, using PIO 1 and State Machine 0
led_bar = WS2812(servo2040.NUM_LEDS, 1, 0, servo2040.LED_DATA)

# Create the user button
user_sw = Button(servo2040.USER_SW)

# Start updating the LED bar
led_bar.start()


# Read sensors until the user button is pressed
while not user_sw.raw():

    # Read each sensor in turn and print its voltage
    for i in range(len(sensor_addrs)):
        mux.select(sensor_addrs[i])
        sensor_voltage = sen_adc.read_voltage()

        # Calculate the LED's hue, with Green for high voltages and Blue for low
        hue = (2.0 - (sensor_voltage / 3.3)) * 0.333
        led_bar.set_hsv(i, hue, 1.0, BRIGHTNESS)

        print("S", i + 1, " = ", round(sensor_voltage, 3), sep="", end=", ")
    print()

    time.sleep(1.0 / UPDATES)
