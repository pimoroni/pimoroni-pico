from motor import Motor, motor2040

"""
A simple program that turns off the motors.
"""

# Create four motor objects.
# This will initialise the pins, stopping any
# previous movement that may be stuck on
m1 = Motor(motor2040.MOTOR_1)
m2 = Motor(motor2040.MOTOR_2)
m3 = Motor(motor2040.MOTOR_3)
m4 = Motor(motor2040.MOTOR_4)
