import gc
import time
import math
from pimoroni import Button
from plasma import WS2812
from motor import Motor, motor2040

"""
An example of applying a wave pattern to a group of motors and the LED.

Press "Boot" to exit the program.

NOTE: Plasma WS2812 uses the RP2040's PIO system, and as
such may have problems when running code multiple times.
If you encounter issues, try resetting your board.
"""

SPEED = 5             # The speed that the LEDs and motors will cycle at
BRIGHTNESS = 0.4      # The brightness of the LEDs
UPDATES = 50          # How many times to update LEDs and motors per second
SPEED_EXTENT = 1.0    # How far from zero to drive the motors

# Free up hardware resources ahead of creating a new MotorCluster
gc.collect()

# Create a list of motors
MOTOR_PINS = [motor2040.MOTOR_A, motor2040.MOTOR_B, motor2040.MOTOR_C, motor2040.MOTOR_D]
motors = [Motor(pins) for pins in MOTOR_PINS]

# Create the LED, using PIO 1 and State Machine 0
led = WS2812(motor2040.NUM_LEDS, 1, 0, motor2040.LED_DATA)

# Create the user button
user_sw = Button(motor2040.USER_SW)

# Start updating the LED
led.start()


offset = 0.0

# Make waves until the user button is pressed
while not user_sw.raw():

    offset += SPEED / 1000.0

    # Update the LED
    led.set_hsv(0, offset / 2, 1.0, BRIGHTNESS)

    # Update all the motors
    for i in range(len(motors)):
        angle = ((i / len(motors)) + offset) * math.pi
        motors[i].speed(math.sin(angle) * SPEED_EXTENT)

    time.sleep(1.0 / UPDATES)

# Stop all the motors
for m in motors:
    m.disable()

# Turn off the LED
led.clear()
