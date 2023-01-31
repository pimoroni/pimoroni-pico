import time
from pimoroni import PID, NORMAL_DIR  # , REVERSED_DIR
from inventor import Inventor2040W, MOTOR_A

"""
A program to aid in the discovery and tuning of motor PID
values for position control. It does this by commanding the
motor to move repeatedly between two setpoint angles and
plots the measured response.

Press "User" to exit the program.
"""

GEAR_RATIO = 50                         # The gear ratio of the motor

DIRECTION = NORMAL_DIR                  # The direction to spin the motor in. NORMAL_DIR (0), REVERSED_DIR (1)
SPEED_SCALE = 5.4                       # The scaling to apply to the motor's speed to match its real-world speed

UPDATES = 100                           # How many times to update the motor per second
UPDATE_RATE = 1 / UPDATES
PRINT_WINDOW = 0.25                     # The time (in seconds) after a new setpoint, to display print out motor values
MOVEMENT_WINDOW = 2.0                   # The time (in seconds) between each new setpoint being set
PRINT_DIVIDER = 1                       # How many of the updates should be printed (i.e. 2 would be every other update)

# Multipliers for the different printed values, so they appear nicely on the Thonny plotter
SPD_PRINT_SCALE = 10                    # Driving Speed multipler

POSITION_EXTENT = 90                    # How far from zero to move the motor, in degrees

# PID values
POS_KP = 0.14                           # Position proportional (P) gain
POS_KI = 0.0                            # Position integral (I) gain
POS_KD = 0.0022                         # Position derivative (D) gain


# Create a new Inventor2040W and get a motor and encoder from it
board = Inventor2040W(motor_gear_ratio=GEAR_RATIO)
m = board.motors[MOTOR_A]
enc = board.encoders[MOTOR_A]

# Set the motor's speed scale
m.speed_scale(SPEED_SCALE)

# Set the motor and encoder's direction
m.direction(DIRECTION)
enc.direction(DIRECTION)

# Create PID object for position control
pos_pid = PID(POS_KP, POS_KI, POS_KD, UPDATE_RATE)

# Enable the motor to get started
m.enable()

# Set the initial setpoint position
pos_pid.setpoint = POSITION_EXTENT


update = 0
print_count = 0

# Continually move the motor until the user button is pressed
while not board.switch_pressed():

    # Capture the state of the encoder
    capture = enc.capture()

    # Calculate the velocity to move the motor closer to the position setpoint
    vel = pos_pid.calculate(capture.degrees, capture.degrees_per_second)

    # Set the new motor driving speed
    m.speed(vel)

    # Print out the current motor values and their setpoints,
    # but only for the first few updates and only every multiple
    if update < (PRINT_WINDOW * UPDATES) and print_count == 0:
        print("Pos =", capture.degrees, end=", ")
        print("Pos SP =", pos_pid.setpoint, end=", ")
        print("Speed = ", m.speed() * SPD_PRINT_SCALE)

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
