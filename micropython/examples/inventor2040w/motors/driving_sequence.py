import time
from pimoroni import PID, REVERSED_DIR
from inventor import Inventor2040W, MOTOR_A, MOTOR_B, NUM_MOTORS, NUM_LEDS

"""
A demonstration of driving both of Inventor 2040 W's motor outputs through a
sequence of velocities, with the help of their attached encoders and PID control.

Press "User" to exit the program.
"""

# Wheel friendly names
LEFT = MOTOR_A
RIGHT = MOTOR_B
NAMES = ["LEFT", "RIGHT"]

# Constants
GEAR_RATIO = 50                         # The gear ratio of the motors
SPEED_SCALE = 5.4                       # The scaling to apply to each motor's speed to match its real-world speed

UPDATES = 100                           # How many times to update the motor per second
UPDATE_RATE = 1 / UPDATES
TIME_FOR_EACH_MOVE = 2                  # The time to travel between each value
UPDATES_PER_MOVE = TIME_FOR_EACH_MOVE * UPDATES
PRINT_DIVIDER = 4                       # How many of the updates should be printed (i.e. 2 would be every other update)

DRIVING_SPEED = 1.0                     # The speed to drive the wheels at, from 0.0 to SPEED_SCALE

# PID values
VEL_KP = 30.0                           # Velocity proportional (P) gain
VEL_KI = 0.0                            # Velocity integral (I) gain
VEL_KD = 0.4                            # Velocity derivative (D) gain

# LED Constants
BRIGHTNESS = 0.4                        # The brightness of the LEDs
SPEED_TO_CYCLING = 0.02 / SPEED_SCALE   # The factor to convert between motor speed and LED cycle rate
BAR_GRADIENT = 0.125                    # The percentage of the colour spectrum to have the LEDs gradient over
HALF_LEDS = NUM_LEDS / 2


# Create a new Inventor2040W
board = Inventor2040W(motor_gear_ratio=GEAR_RATIO)

# Set the speed scale of the motors
board.motors[LEFT].speed_scale(SPEED_SCALE)
board.motors[RIGHT].speed_scale(SPEED_SCALE)

# Reverse the direction of the left motor and encoder
board.motors[LEFT].direction(REVERSED_DIR)
board.encoders[LEFT].direction(REVERSED_DIR)

# Create PID objects for position control
vel_pids = [PID(VEL_KP, VEL_KI, VEL_KD, UPDATE_RATE) for i in range(NUM_MOTORS)]


# LED Hue Variables
offset_l = 0.0
offset_r = 0.0


# Helper functions for driving in common directions
def drive_forward(speed):
    global offset_l
    global offset_r
    vel_pids[LEFT].setpoint = speed
    vel_pids[RIGHT].setpoint = speed
    offset_l += speed * SPEED_TO_CYCLING
    offset_r += speed * SPEED_TO_CYCLING


def turn_right(speed):
    global offset_l
    global offset_r
    vel_pids[LEFT].setpoint = speed
    vel_pids[RIGHT].setpoint = -speed
    offset_l += speed * SPEED_TO_CYCLING
    offset_r -= speed * SPEED_TO_CYCLING


def stop():
    vel_pids[LEFT].setpoint = 0
    vel_pids[RIGHT].setpoint = 0


# Enable the motor to get started
for m in board.motors:
    m.enable()


# Variables
update = 0
print_count = 0
sequence = 0
captures = [None] * NUM_MOTORS


# Continually move the motor until the user switch is pressed
while not board.switch_pressed():

    # Capture the state of all the encoders
    for i in range(NUM_MOTORS):
        captures[i] = board.encoders[i].capture()

    for i in range(NUM_MOTORS):
        # Calculate the acceleration to apply to the motor to move it closer to the velocity setpoint
        accel = vel_pids[i].calculate(captures[i].revolutions_per_second)

        # Accelerate or decelerate the motor
        board.motors[i].speed(board.motors[i].speed() + (accel * UPDATE_RATE))

    # Print out the current motor values, but only on every multiple
    if print_count == 0:
        for i in range(NUM_MOTORS):
            print(NAMES[i], "=", captures[i].revolutions_per_second, end=", ")
        print()

    # Increment the print count, and wrap it
    print_count = (print_count + 1) % PRINT_DIVIDER

    update += 1     # Move along in time

    # Have we reached the end of this movement?
    if update >= UPDATES_PER_MOVE:
        update = 0  # Reset the counter

        # Move on to the next part of the sequence
        sequence += 1

        # Loop the sequence back around
        if sequence >= 5:
            sequence = 0

    # Set the motor speeds, based on the sequence
    if sequence == 0:
        drive_forward(DRIVING_SPEED)
    elif sequence == 1:
        drive_forward(-DRIVING_SPEED)
    elif sequence == 2:
        turn_right(DRIVING_SPEED)
    elif sequence == 3:
        turn_right(-DRIVING_SPEED)
    elif sequence == 4:
        stop()

    if offset_l < 0.0:
        offset_l += 1.0

    if offset_r < 0.0:
        offset_r += 1.0

    # Update the LED bars
    for i in range(HALF_LEDS):
        hue = (i / HALF_LEDS) * BAR_GRADIENT
        board.leds.set_hsv(i, hue + offset_l, 1.0, BRIGHTNESS)
        board.leds.set_hsv(NUM_LEDS - i - 1, hue + offset_r, 1.0, BRIGHTNESS)

    time.sleep(UPDATE_RATE)

# Stop all the motors
for m in board.motors:
    m.disable()

# Turn off the LED bars
board.leds.clear()
