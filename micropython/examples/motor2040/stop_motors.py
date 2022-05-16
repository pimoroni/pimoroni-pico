from motor import Motor, motor2040

"""
A simple program that stops the motors.
"""

# Create four motor objects.
# This will initialise the pins, stopping any
# previous movement that may be stuck on
mA = Motor(motor2040.MOTOR_A)
mB = Motor(motor2040.MOTOR_B)
mC = Motor(motor2040.MOTOR_C)
mD = Motor(motor2040.MOTOR_D)
