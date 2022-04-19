import gc
import time
import math
from pimoroni import Button
from plasma import WS2812
from motor import Motor, MotorCluster, motor2040

"""
An example of applying a wave pattern to a group of motors and the LED.

Press "Boot" to exit the program.

NOTE: MotorCluster and Plasma WS2812 use the RP2040's PIO system,
and as such may have problems when running code multiple times.
If you encounter issues, try resetting your board.
"""

SPEED = 5             # The speed that the LEDs will cycle at
BRIGHTNESS = 0.4      # The brightness of the LEDs
UPDATES = 50          # How many times to update LEDs and Motors per second
MOTOR_EXTENT = 1.0    # How far from zero to move the motors

# Free up hardware resources ahead of creating a new MotorCluster
gc.collect()

# Create a motor cluster for pins 0 to 7, using PIO 0 and State Machine 0
# motors = MotorCluster(pio=0, sm=0, pins=list(range(START_PIN, END_PIN + 1)))
MOTOR_PINS = [ motor2040.MOTOR_1, motor2040.MOTOR_2, motor2040.MOTOR_3, motor2040.MOTOR_4]
motors = [Motor(pins) for pins in MOTOR_PINS]

# Create the LED, using PIO 1 and State Machine 0
led = WS2812(motor2040.NUM_LEDS, 1, 0, motor2040.LED_DATA)

# Create the user button
user_sw = Button(motor2040.USER_SW)

# Start updating the LED
led.start()


offset = 0.0

# Make waves until the user button is pressed
while user_sw.raw() is not True:

    offset += SPEED / 1000.0

    # Update the LED
    led.set_hsv(0, offset / 2, 1.0, BRIGHTNESS)

    # Update all the MOTORs
    #for i in range(motors.count()):
    for i in range(len(motors)):
        angle = ((i / len(motors)) + offset) * math.pi
        motors[i].speed(math.sin(angle) * MOTOR_EXTENT)
        
    time.sleep(1.0 / UPDATES)

# Stop all the motors
for m in motors:
    m.disable()

# Turn off the LED bar
led.clear()
