import gc
import time
from motor import Motor, motor2040
from encoder import Encoder, MMME_CPR
from pimoroni import Button, PID, NORMAL_DIR  # , REVERSED_DIR

"""
A program to aid in the discovery and tuning of motor PID
values for position on velocity control. It does this by
commanding the motor to move repeatedly between two setpoint
angles and plots the measured response.

Press "Boot" to exit the program.
"""

MOTOR_PINS = motor2040.MOTOR_A          # The pins of the motor being profiled
ENCODER_PINS = motor2040.ENCODER_A      # The pins of the encoder attached to the profiled motor
GEAR_RATIO = 50                         # The gear ratio of the motor
COUNTS_PER_REV = MMME_CPR * GEAR_RATIO  # The counts per revolution of the motor's output shaft

DIRECTION = NORMAL_DIR                  # The direction to spin the motor in. NORMAL_DIR (0), REVERSED_DIR (1)
SPEED_SCALE = 5.4                       # The scaling to apply to the motor's speed to match its real-world speed

UPDATES = 100                           # How many times to update the motor per second
UPDATE_RATE = 1 / UPDATES
PRINT_WINDOW = 1.0                      # The time (in seconds) after a new setpoint, to display print out motor values
MOVEMENT_WINDOW = 2.0                   # The time (in seconds) between each new setpoint being set
PRINT_DIVIDER = 4                       # How many of the updates should be printed (i.e. 2 would be every other update)

# Multipliers for the different printed values, so they appear nicely on the Thonny plotter
ACC_PRINT_SCALE = 1                     # Acceleration multiplier
SPD_PRINT_SCALE = 20                    # Driving Speed multipler

POSITION_EXTENT = 180                   # How far from zero to move the motor, in degrees
MAX_SPEED = 2.0                         # The maximum speed to move the motor at, in revolutions per second

# PID values
POS_KP = 0.025                          # Position proportional (P) gain
POS_KI = 0.0                            # Position integral (I) gain
POS_KD = 0.0                            # Position derivative (D) gain

VEL_KP = 30.0                           # Velocity proportional (P) gain
VEL_KI = 0.0                            # Velocity integral (I) gain
VEL_KD = 0.4                            # Velocity derivative (D) gain


# Free up hardware resources ahead of creating a new Encoder
gc.collect()

# Create a motor and set its speed scale
m = Motor(MOTOR_PINS, direction=DIRECTION, speed_scale=SPEED_SCALE)

# Create an encoder, using PIO 0 and State Machine 0
enc = Encoder(0, 0, ENCODER_PINS, direction=DIRECTION, counts_per_rev=COUNTS_PER_REV, count_microsteps=True)

# Create the user button
user_sw = Button(motor2040.USER_SW)

# Create PID objects for both position and velocity control
pos_pid = PID(POS_KP, POS_KI, POS_KD, UPDATE_RATE)
vel_pid = PID(VEL_KP, VEL_KI, VEL_KD, UPDATE_RATE)

# Enable the motor to get started
m.enable()

# Set the initial setpoint position
pos_pid.setpoint = POSITION_EXTENT


update = 0
print_count = 0

# Continually move the motor until the user button is pressed
while not user_sw.raw():

    # Capture the state of the encoder
    capture = enc.capture()

    # Calculate the velocity to move the motor closer to the position setpoint
    vel = pos_pid.calculate(capture.degrees, capture.degrees_per_second)

    # Limit the velocity between user defined limits, and set it as the new setpoint of the velocity PID
    vel_pid.setpoint = max(min(vel, MAX_SPEED), -MAX_SPEED)

    # Calculate the acceleration to apply to the motor to move it closer to the velocity setpoint
    accel = vel_pid.calculate(capture.revolutions_per_second)

    # Accelerate or decelerate the motor
    m.speed(m.speed() + (accel * UPDATE_RATE))

    # Print out the current motor values and their setpoints,
    # but only for the first few updates and only every multiple
    if update < (PRINT_WINDOW * UPDATES) and print_count == 0:
        print("Pos =", capture.degrees, end=", ")
        print("Pos SP =", pos_pid.setpoint, end=", ")
        print("Vel =", capture.revolutions_per_second * SPD_PRINT_SCALE, end=", ")
        print("Vel SP =", vel_pid.setpoint * SPD_PRINT_SCALE, end=", ")
        print("Accel =", accel * ACC_PRINT_SCALE, end=", ")
        print("Speed =", m.speed() * SPD_PRINT_SCALE)

    # Increment the print count, and wrap it
    print_count = (print_count + 1) % PRINT_DIVIDER

    update += 1     # Move along in time

    # Have we reached the end of this time window?
    if update >= (MOVEMENT_WINDOW * UPDATES):
        update = 0  # Reset the counter

        # Set the new position setpoint to be the inverse of the current setpoint
        pos_pid.setpoint = 0.0 - pos_pid.setpoint

    time.sleep(UPDATE_RATE)

# Disable the motor
m.disable()
