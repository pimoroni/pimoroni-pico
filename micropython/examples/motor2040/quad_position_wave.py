import gc
import time
import math
from plasma import WS2812
from motor import Motor, motor2040
from encoder import Encoder, MMME_CPR
from pimoroni import Button, PID, REVERSED_DIR

"""
A demonstration of driving all four of Motor 2040's motor outputs between
positions, with the help of their attached encoders and PID control.

Press "Boot" to exit the program.
"""

GEAR_RATIO = 50                         # The gear ratio of the motors
COUNTS_PER_REV = MMME_CPR * GEAR_RATIO  # The counts per revolution of each motor's output shaft

SPEED_SCALE = 5.4                       # The scaling to apply to each motor's speed to match its real-world speed

UPDATES = 100                           # How many times to update the motor per second
UPDATE_RATE = 1 / UPDATES
TIME_FOR_EACH_MOVE = 2                  # The time to travel between each value
UPDATES_PER_MOVE = TIME_FOR_EACH_MOVE * UPDATES
PRINT_DIVIDER = 4                       # How many of the updates should be printed (i.e. 2 would be every other update)

# LED constant
BRIGHTNESS = 0.4      # The brightness of the RGB LED

# PID values
POS_KP = 0.14                           # Position proportional (P) gain
POS_KI = 0.0                            # Position integral (I) gain
POS_KD = 0.0022                         # Position derivative (D) gain


# Free up hardware resources ahead of creating a new Encoder
gc.collect()

# Create a list of motors with a given speed scale
MOTOR_PINS = [motor2040.MOTOR_A, motor2040.MOTOR_B, motor2040.MOTOR_C, motor2040.MOTOR_D]
motors = [Motor(pins, speed_scale=SPEED_SCALE) for pins in MOTOR_PINS]

# Create a list of encoders, using PIO 0, with the given counts per revolution
ENCODER_PINS = [motor2040.ENCODER_A, motor2040.ENCODER_B, motor2040.ENCODER_C, motor2040.ENCODER_D]
ENCODER_NAMES = ["A", "B", "C", "D"]
encoders = [Encoder(0, i, ENCODER_PINS[i], counts_per_rev=COUNTS_PER_REV, count_microsteps=True) for i in range(motor2040.NUM_MOTORS)]

# Reverse the direction of the B and D motors and encoders
motors[1].direction(REVERSED_DIR)
motors[3].direction(REVERSED_DIR)
encoders[1].direction(REVERSED_DIR)
encoders[3].direction(REVERSED_DIR)

# Create the LED, using PIO 1 and State Machine 0
led = WS2812(motor2040.NUM_LEDS, 1, 0, motor2040.LED_DATA)

# Create the user button
user_sw = Button(motor2040.USER_SW)

# Create PID objects for position control
pos_pids = [PID(POS_KP, POS_KI, POS_KD, UPDATE_RATE) for i in range(motor2040.NUM_MOTORS)]

# Start updating the LED
led.start()

# Enable all motors
for m in motors:
    m.enable()


update = 0
print_count = 0

# Set the initial and end values
start_value = 0.0
end_value = 270.0

captures = [None] * motor2040.NUM_MOTORS

# Continually move the motor until the user button is pressed
while not user_sw.raw():

    # Capture the state of all the encoders
    for i in range(motor2040.NUM_MOTORS):
        captures[i] = encoders[i].capture()

    # Calculate how far along this movement to be
    percent_along = min(update / UPDATES_PER_MOVE, 1.0)

    for i in range(motor2040.NUM_MOTORS):
        # Move the motor between values using cosine
        pos_pids[i].setpoint = (((-math.cos(percent_along * math.pi) + 1.0) / 2.0) * (end_value - start_value)) + start_value

        # Calculate the velocity to move the motor closer to the position setpoint
        vel = pos_pids[i].calculate(captures[i].degrees, captures[i].degrees_per_second)

        # Set the new motor driving speed
        motors[i].speed(vel)

    # Update the LED
    led.set_hsv(0, percent_along, 1.0, BRIGHTNESS)

    # Print out the current motor values and their setpoints, but only on every multiple
    if print_count == 0:
        for i in range(motor2040.NUM_MOTORS):
            print(ENCODER_NAMES[i], "=", captures[i].degrees, end=", ")
        print()

    # Increment the print count, and wrap it
    print_count = (print_count + 1) % PRINT_DIVIDER

    update += 1     # Move along in time

    # Have we reached the end of this movement?
    if update >= UPDATES_PER_MOVE:
        update = 0  # Reset the counter

        # Swap the start and end values
        temp = start_value
        start_value = end_value
        end_value = temp

    time.sleep(UPDATE_RATE)

# Stop all the motors
for m in motors:
    m.disable()

# Turn off the LED bar
led.clear()
