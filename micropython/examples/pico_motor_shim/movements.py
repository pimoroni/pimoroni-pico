import time
from motor import Motor, pico_motor_shim
from pimoroni import NORMAL_DIR, REVERSED_DIR

"""
An example of how to perform simple movements of a 2-wheeled driving robot.
"""

SPEED_SCALE = 5.4               # The scaling to apply to each motor's speed to match its real-world speed
DRIVING_SPEED = SPEED_SCALE     # The speed to drive the wheels at, from 0.0 to SPEED_SCALE

# Create the left and right motors with a given speed scale
# Swap the numbers and directions if this is different to your setup
left = Motor(pico_motor_shim.MOTOR_1, direction=NORMAL_DIR, speed_scale=SPEED_SCALE)
right = Motor(pico_motor_shim.MOTOR_2, direction=REVERSED_DIR, speed_scale=SPEED_SCALE)


# Helper functions for driving in common directions
def forward(speed=DRIVING_SPEED):
    left.speed(speed)
    right.speed(speed)


def backward(speed=DRIVING_SPEED):
    left.speed(-speed)
    right.speed(-speed)


def turn_left(speed=DRIVING_SPEED):
    left.speed(-speed)
    right.speed(speed)


def turn_right(speed=DRIVING_SPEED):
    left.speed(speed)
    right.speed(-speed)


def curve_forward_left(speed=DRIVING_SPEED):
    left.speed(0.0)
    right.speed(speed)


def curve_forward_right(speed=DRIVING_SPEED):
    left.speed(speed)
    right.speed(0.0)


def curve_backward_left(speed=DRIVING_SPEED):
    left.speed(0.0)
    right.speed(-speed)


def curve_backward_right(speed=DRIVING_SPEED):
    left.speed(-speed)
    right.speed(0.0)


def stop():
    left.stop()
    right.stop()


def coast():
    left.coast()
    right.coast()


# Demo each of the move methods
forward()
time.sleep(1)

backward()
time.sleep(1)

curve_forward_right()
time.sleep(1)

curve_forward_left()
time.sleep(1)

turn_right()
time.sleep(1)

forward(0.5 * DRIVING_SPEED)  # Half speed
time.sleep(1)

turn_left(0.5 * DRIVING_SPEED)  # Half speed
time.sleep(1)

curve_backward_right(0.75 * DRIVING_SPEED)  # Three quarters speed
time.sleep(1)

forward()  # Full speed
time.sleep(0.5)

coast()  # Come to a halt gently
time.sleep(1)

forward()
time.sleep(0.5)

stop()  # Apply the brakes
time.sleep(1.0)


# Disable the motors
left.disable()
right.disable()
