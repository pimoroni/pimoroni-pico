from inventor import Inventor2040W, SERVO_1, SERVO_2, SERVO_3, SERVO_4
from servo import Calibration, ANGULAR, LINEAR, CONTINUOUS

"""
Shows how to configure Inventor 2040 W's servos with different
common calibrations, as well as a completely custom one.
"""

# Create a new Inventor2040W and access four of its servos
board = Inventor2040W()
angular_servo = board.servos[SERVO_1]
linear_servo = board.servos[SERVO_2]
continuous_servo = board.servos[SERVO_3]
custom_servo = board.servos[SERVO_4]


# -----------------------------------------------------
# Modify the calibration of an angular servo
# -----------------------------------------------------

# Access the calibration of the first servo and print it out
cal = angular_servo.calibration()
print("Angular Servo:", cal, end="\n\n")


WIDE_ANGLE_RANGE = 270  # The range we want the anglular servo to cover

# Lets modify the calibration to increase its range
cal.apply_default_pairs(ANGULAR)
cal.first_value(-WIDE_ANGLE_RANGE / 2)
cal.last_value(WIDE_ANGLE_RANGE / 2)

# Now apply the modified calibration to the servo and confirm it worked
angular_servo.calibration(cal)
print("Wide Angle Servo:", angular_servo.calibration(), end="\n\n")


# ---------------------------------------------------
# Create and modify the calibration of a linear servo
# ---------------------------------------------------

LINEAR_RANGE = 50   # The range we want the linear servo to cover

# Update the linear servo so its max value matches the real distance travelled
cal = linear_servo.calibration()
cal.apply_default_pairs(LINEAR)
cal.last_value(LINEAR_RANGE)

# Apply the modified calibration to the servo and confirm it worked
linear_servo.calibration(cal)
print("Linear Servo:", linear_servo.calibration(), end="\n\n")


# ----------------------------------------------------------------
# Create and modify the calibration of a continuous rotation servo
# ----------------------------------------------------------------

CONTINUOUS_SPEED = 10   # The speed we want the continuous servo to cover

# Update the continuous rotation servo so its value matches its real speed
cal = continuous_servo.calibration()
cal.apply_default_pairs(CONTINUOUS)
cal.first_value(-CONTINUOUS_SPEED)
cal.last_value(CONTINUOUS_SPEED)

# Apply the modified calibration to the servo and confirm it worked
continuous_servo.calibration(cal)
print("Continuous Servo:", continuous_servo.calibration(), end="\n\n")


# ------------------------------------------------------
# Create a custom calibration and build a servo using it
# ------------------------------------------------------

# Create an empty calibration
cal = Calibration()

# Give it a range of -45 to 45 degrees, corresponding to pulses of 1000 and 2000 microseconds
cal.apply_two_pairs(1000, 2000, -45, 45)

# Turn off the lower and upper limits, so the servo can go beyond 45 degrees
cal.limit_to_calibration(False, False)

# Create a servo on pin 3 using the custom calibration and confirmed it worked
custom_servo.calibration(cal)
print("Custom Servo:", custom_servo.calibration(), end="\n\n")
