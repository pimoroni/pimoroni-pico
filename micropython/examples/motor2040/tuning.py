import gc
import time
import math
from pimoroni import Button
from motor import Motor, motor2040
from encoder import Encoder, MMME_CPR

"""
A program that profiles the speed of a motor across its PWM
duty cycle range using the attached encoder for feedback.
"""

MOTOR_PINS = motor2040.MOTOR_A          # The pins of the motor being profiled
ENCODER_PINS = motor2040.ENCODER_A      # The pins of the encoder attached to the profiled motor
GEAR_RATIO = 50                         # The gear ratio of the motor
COUNTS_PER_REV = MMME_CPR * GEAR_RATIO  # The counts per revolution of the motor's output shaft

DIRECTION = 0                           # The direction to spin the motor in. NORMAL (0), REVERSED (1)
SPEED_SCALE = 5.4                       # The scaling to apply to the motor's speed
                                        # Set this to the maximum measured speed

UPDATES = 100                           # How many times to update the motor per second
TIME_FOR_EACH_MOVE = 0.25                 # The time to travel between each random value
UPDATES_PER_MOVE = TIME_FOR_EACH_MOVE * UPDATES
FURTHER_DELAY = 2                  # The time to travel between each random value
FURTHER_UPDATES = FURTHER_DELAY * UPDATES

MOTOR_EXTENT = 90                      # How far from zero to move the motor

# Free up hardware resources ahead of creating a new Encoder
gc.collect()

# Create a motor and set its speed scale
m = Motor(MOTOR_PINS, direction=DIRECTION, speed_scale=SPEED_SCALE, deadzone=0.1)

# Create an encoder, using PIO 0 and State Machine 0
enc = Encoder(0, 0, ENCODER_PINS, direction=DIRECTION, counts_per_rev=COUNTS_PER_REV, count_microsteps=True)

# Create the user button
user_sw = Button(motor2040.USER_SW)


update = 0
target = MOTOR_EXTENT
error_sum = 0

# Values for 50Hz update
# kP = 0.08
# kI = 0.000
# kD = 0.06

# Values for 100Hz update
kP = 0.14
kI = 0.000
kD = 0.2

# Enable the motor to get started
m.enable()

last_angle = 0

# Continually move the servo until the user button is pressed
while user_sw.raw() is not True:

    capture = enc.capture()
    error = target - capture.degrees
    error_sum += (kI * error)
    motor_speed = (error * kP) + error_sum - (kD * capture.degrees_delta)

    m.speed(motor_speed)

    if update < UPDATES_PER_MOVE:
        # Print out the target value the motor will move towards
        print("Current = ", round(capture.degrees, 3), ", Target = ", round(target, 3), ", Speed x10= ", round(m.speed() * 10, 3),sep="")

    # Move along in time
    update += 1

    # Have we reached the end of this movement?
    if update >= UPDATES_PER_MOVE + FURTHER_UPDATES:
        # Reset the counter
        update = 0

        target = -target

    time.sleep(1.0 / UPDATES)

# Disable the servo
m.disable()