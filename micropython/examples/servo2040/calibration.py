from servo import Calibration, Servo, servo2040, ANGULAR, LINEAR, CONTINUOUS

"""
Shows how to create servos with different common
calibrations, modify a servo's existing calibration,
and create a servo with a custom calibration.
"""

# -----------------------------------------------------
# Create and modify the calibration of an angular servo
# -----------------------------------------------------

# Create an angular servo on pin 0. By default its value ranges from -90 to +90
angular_servo = Servo(servo2040.SERVO_1, ANGULAR)

# Access its calibration and print it out
cal = angular_servo.calibration()
print("Angular Servo:", cal, end="\n\n")


WIDE_ANGLE_RANGE = 270  # The range we want the anglular servo to cover

# Lets modify the calibration to increase its range
cal.first_value(-WIDE_ANGLE_RANGE / 2)
cal.last_value(WIDE_ANGLE_RANGE / 2)

# Now apply the modified calibration to the servo and confirm it worked
angular_servo.calibration(cal)
print("Wide Angle Servo:", angular_servo.calibration(), end="\n\n")


# ---------------------------------------------------
# Create and modify the calibration of a linear servo
# ---------------------------------------------------

LINEAR_RANGE = 50   # The range we want the linear servo to cover

# Create a linear servo on pin 1. By default its value ranges from 0.0 to 1.0
linear_servo = Servo(servo2040.SERVO_2, LINEAR)

# Update the linear servo so its max value matches the real distance travelled
cal = linear_servo.calibration()
cal.last_value(LINEAR_RANGE)

# Apply the modified calibration to the servo and confirm it worked
linear_servo.calibration(cal)
print("Linear Servo:", linear_servo.calibration(), end="\n\n")


# ----------------------------------------------------------------
# Create and modify the calibration of a continuous rotation servo
# ----------------------------------------------------------------

CONTINUOUS_SPEED = 10   # The speed we want the continuous servo to cover

# Create a continous rotation servo on pin 2. By default its value ranges from -1.0 to +1.0
continuous_servo = Servo(servo2040.SERVO_3, CONTINUOUS)

# Update the continuous rotation servo so its value matches its real speed
cal = continuous_servo.calibration()
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
custom_servo = Servo(servo2040.SERVO_4, cal)
print("Custom Servo:", custom_servo.calibration(), end="\n\n")
