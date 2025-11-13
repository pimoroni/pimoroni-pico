import time
from pimoroni import PID, REVERSED_DIR
from inventor import Inventor, MOTOR_A, MOTOR_B, NUM_MOTORS, NUM_LEDS, GP0
from aye_arr.nec import NECReceiver
from aye_arr.nec.remotes import PimoroniRemote


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
UPDATES = 100                           # How many times to update the motor per second
UPDATE_RATE = 1 / UPDATES
PRINT_DIVIDER = 4                       # How many of the updates should be printed (i.e. 2 would be every other update)

# Motor Constnats
GEAR_RATIO = 50                         # The gear ratio of the motors
SPEED_SCALE = 5.4                       # The scaling to apply to each motor's speed to match its real-world speed
DRIVING_SPEED = 1.0                     # The speed to drive the wheels at, for forward-backward. From 0.0 to SPEED_SCALE
TURNING_SPEED = 0.5                     # The speed to drive the wheels at, for left-right. From 0.0 to SPEED_SCALE

# Motor PID values
VEL_KP = 30.0                           # Velocity proportional (P) gain
VEL_KI = 0.0                            # Velocity integral (I) gain
VEL_KD = 0.4                            # Velocity derivative (D) gain

# Servo Constants (These values will be unique for each servo)
GRIPPER_OPEN_PULSE = 1960               # The pulse width (in microseconds) to put the gripper in the open position
GRIPPER_CLOSED_PULSE = 1040             # The pulse width (in microseconds) to put the gripper in the closed position
GRIPPER_STEP = 0.1                      # The percentage to move the gripper by when the button is pressed

# LED Constants
BRIGHTNESS = 0.5                        # The brightness of the LEDs, from 0 to 1
RED = 255, 0, 0
GREEN = 0, 255, 0
BLUE = 0, 0, 255
CYAN = 0, 255, 255
MAGENTA = 255, 0, 255
YELLOW = 255, 255, 0
WARM_WHITE = 255, 192, 96
WHITE = 255, 255, 255
COOL_WHITE = 96, 192, 255
BLACK = 0, 0, 0


# Create a new Inventor object
board = Inventor(motor_gear_ratio=GEAR_RATIO)

# Set the speed scale of the motors
board.motors[LEFT].speed_scale(SPEED_SCALE)
board.motors[RIGHT].speed_scale(SPEED_SCALE)

# Reverse the direction of the left motor and encoder
board.motors[LEFT].direction(REVERSED_DIR)
board.encoders[LEFT].direction(REVERSED_DIR)

# Create PID objects for velocity control
vel_pids = [PID(VEL_KP, VEL_KI, VEL_KD, UPDATE_RATE) for i in range(NUM_MOTORS)]

# Set up the first servo for the gripper
gripper = board.servos[0]
cal = gripper.calibration()
cal.apply_two_pairs(GRIPPER_CLOSED_PULSE, GRIPPER_OPEN_PULSE, 0, 1)
gripper.calibration(cal)


# Functions for driving in common directions
def drive_fwd_back(speed):
    vel_pids[LEFT].setpoint = speed
    vel_pids[RIGHT].setpoint = speed


def turn_left_right(speed):
    vel_pids[LEFT].setpoint = -speed
    vel_pids[RIGHT].setpoint = speed


def stop_driving():
    vel_pids[LEFT].setpoint = 0
    vel_pids[RIGHT].setpoint = 0


# Function for operating the gripper servo
def move_gripper(step):
    val = max(min(gripper.value() + step, 1), 0)
    gripper.value(val)


# Functions for setting LED colours
def set_all_leds(colour):
    r = int(colour[0] * BRIGHTNESS)
    g = int(colour[1] * BRIGHTNESS)
    b = int(colour[2] * BRIGHTNESS)
    for i in range(NUM_LEDS):
        board.leds.set_rgb(i, r, g, b)


def set_rainbow():
    for i in range(NUM_LEDS):
        hue = float(i) / NUM_LEDS
        board.leds.set_hsv(i, hue, 1.0, BRIGHTNESS)


# Bind functions to each of the Pimoroni remote's buttons.
# Releasing a direction button will the robot to stop driving
remote = PimoroniRemote()
remote.bind("UP", (drive_fwd_back, DRIVING_SPEED), on_release=stop_driving)
remote.bind("DOWN", (drive_fwd_back, -DRIVING_SPEED), on_release=stop_driving)
remote.bind("LEFT", (turn_left_right, TURNING_SPEED), on_release=stop_driving)
remote.bind("RIGHT", (turn_left_right, -TURNING_SPEED), on_release=stop_driving)
remote.bind("ANTICLOCK", (move_gripper, GRIPPER_STEP))
remote.bind("CLOCKWISE", (move_gripper, -GRIPPER_STEP))
remote.bind("OK/STOP", (set_all_leds, BLACK))
remote.bind("1/RED", (set_all_leds, RED))
remote.bind("2/GREEN", (set_all_leds, GREEN))
remote.bind("3/BLUE", (set_all_leds, BLUE))
remote.bind("4/CYAN", (set_all_leds, CYAN))
remote.bind("5/MAGENTA", (set_all_leds, MAGENTA))
remote.bind("6/YELLOW", (set_all_leds, YELLOW))
remote.bind("7/WARM", (set_all_leds, WARM_WHITE))
remote.bind("8/WHITE", (set_all_leds, WHITE))
remote.bind("9/COOL", (set_all_leds, COOL_WHITE))
remote.bind("0/RAINBOW", set_rainbow)


# Set up the IR receiver on GP0, using PIO 1 and SM 0
receiver = NECReceiver(GP0, 1, 0)
receiver.bind(remote)

# Variables
captures = [None] * NUM_MOTORS

try:
    # Enable the motors and servo to get started
    for m in board.motors:
        m.enable()
    gripper.enable()

    # Start listening for IR remote signals
    receiver.start()

    # Continually move the motor until the user switch is pressed
    while not board.switch_pressed():

        # Decode any received IR signals from since we last checked
        receiver.decode()

        # Capture the state of all the encoders
        for i in range(NUM_MOTORS):
            captures[i] = board.encoders[i].capture()

        for i in range(NUM_MOTORS):
            # Calculate the acceleration to apply to the motor to move it closer to the velocity setpoint
            accel = vel_pids[i].calculate(captures[i].revolutions_per_second)

            # Accelerate or decelerate the motor
            board.motors[i].speed(board.motors[i].speed() + (accel * UPDATE_RATE))

        time.sleep(UPDATE_RATE)

finally:
    # Stop the motors and servo
    for m in board.motors:
        m.disable()
    gripper.disable()

    # Turn off the LED bars
    board.leds.clear()

    # Stop the IR receiver
    receiver.stop()
