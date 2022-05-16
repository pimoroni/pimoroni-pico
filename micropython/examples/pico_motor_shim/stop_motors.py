from motor import Motor, pico_motor_shim

"""
A simple program that stops the motors.
"""

# Create two motor objects.
# This will initialise the pins, stopping any
# previous movement that may be stuck on
m1 = Motor(pico_motor_shim.MOTOR_1)
m2 = Motor(pico_motor_shim.MOTOR_2)
