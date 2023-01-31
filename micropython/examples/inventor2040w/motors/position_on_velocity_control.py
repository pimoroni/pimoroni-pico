import time
import math
import random
from pimoroni import PID, NORMAL_DIR  # , REVERSED_DIR
from inventor import Inventor2040W, MOTOR_A

"""
An example of how to move a motor smoothly between random positions,
with velocity limits, with the help of it's attached encoder and PID control.

Press "User" to exit the program.
"""

GEAR_RATIO = 50                         # The gear ratio of the motor

DIRECTION = NORMAL_DIR                  # The direction to spin the motor in. NORMAL_DIR (0), REVERSED_DIR (1)
SPEED_SCALE = 5.4                       # The scaling to apply to the motor's speed to match its real-world speed

UPDATES = 100                           # How many times to update the motor per second
UPDATE_RATE = 1 / UPDATES
TIME_FOR_EACH_MOVE = 1                  # The time to travel between each random value, in seconds
UPDATES_PER_MOVE = TIME_FOR_EACH_MOVE * UPDATES
PRINT_DIVIDER = 4                       # How many of the updates should be printed (i.e. 2 would be every other update)

# Multipliers for the different printed values, so they appear nicely on the Thonny plotter
ACC_PRINT_SCALE = 2                     # Acceleration multiplier
SPD_PRINT_SCALE = 40                    # Driving Speed multipler

POSITION_EXTENT = 180                   # How far from zero to move the motor, in degrees
MAX_SPEED = 1.0                         # The maximum speed to move the motor at, in revolutions per second
INTERP_MODE = 0                         # The interpolating mode between setpoints. STEP (0), LINEAR (1), COSINE (2)

# PID values
POS_KP = 0.025                          # Position proportional (P) gain
POS_KI = 0.0                            # Position integral (I) gain
POS_KD = 0.0                            # Position derivative (D) gain

VEL_KP = 30.0                           # Velocity proportional (P) gain
VEL_KI = 0.0                            # Velocity integral (I) gain
VEL_KD = 0.4                            # Velocity derivative (D) gain


# Create a new Inventor2040W and get a motor and encoder from it
board = Inventor2040W(motor_gear_ratio=GEAR_RATIO)
m = board.motors[MOTOR_A]
enc = board.encoders[MOTOR_A]

# Set the motor's speed scale
m.speed_scale(SPEED_SCALE)

# Set the motor and encoder's direction
m.direction(DIRECTION)
enc.direction(DIRECTION)

# Create PID objects for both position and velocity control
pos_pid = PID(POS_KP, POS_KI, POS_KD, UPDATE_RATE)
vel_pid = PID(VEL_KP, VEL_KI, VEL_KD, UPDATE_RATE)

# Enable the motor to get started
m.enable()


update = 0
print_count = 0

# Set the initial value and create a random end value between the extents
start_value = 0.0
end_value = random.uniform(-POSITION_EXTENT, POSITION_EXTENT)

# Continually move the motor until the user button is pressed
while not board.switch_pressed():

    # Capture the state of the encoder
    capture = enc.capture()

    # Calculate how far along this movement to be
    percent_along = min(update / UPDATES_PER_MOVE, 1.0)

    if INTERP_MODE == 0:
        # Move the motor instantly to the end value
        pos_pid.setpoint = end_value
    elif INTERP_MODE == 2:
        # Move the motor between values using cosine
        pos_pid.setpoint = (((-math.cos(percent_along * math.pi) + 1.0) / 2.0) * (end_value - start_value)) + start_value
    else:
        # Move the motor linearly between values
        pos_pid.setpoint = (percent_along * (end_value - start_value)) + start_value

    # Calculate the velocity to move the motor closer to the position setpoint
    vel = pos_pid.calculate(capture.degrees, capture.degrees_per_second)

    # Limit the velocity between user defined limits, and set it as the new setpoint of the velocity PID
    vel_pid.setpoint = max(min(vel, MAX_SPEED), -MAX_SPEED)

    # Calculate the acceleration to apply to the motor to move it closer to the velocity setpoint
    accel = vel_pid.calculate(capture.revolutions_per_second)

    # Accelerate or decelerate the motor
    m.speed(m.speed() + (accel * UPDATE_RATE))

    # Print out the current motor values and their setpoints, but only on every multiple
    if print_count == 0:
        print("Pos =", capture.degrees, end=", ")
        print("Pos SP =", pos_pid.setpoint, end=", ")
        print("Vel =", capture.revolutions_per_second * SPD_PRINT_SCALE, end=", ")
        print("Vel SP =", vel_pid.setpoint * SPD_PRINT_SCALE, end=", ")
        print("Accel =", accel * ACC_PRINT_SCALE, end=", ")
        print("Speed =", m.speed() * SPD_PRINT_SCALE)

    # Increment the print count, and wrap it
    print_count = (print_count + 1) % PRINT_DIVIDER

    update += 1     # Move along in time

    # Have we reached the end of this movement?
    if update >= UPDATES_PER_MOVE:
        update = 0  # Reset the counter

        # Set the start as the last end and create a new random end value
        start_value = end_value
        end_value = random.uniform(-POSITION_EXTENT, POSITION_EXTENT)

    time.sleep(UPDATE_RATE)

# Disable the motor
m.disable()
