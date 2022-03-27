#include <cstdio>
#include "pico/stdlib.h"

#include "servo2040.hpp"

/*
Shows how to create servos with different common
calibrations, modify a servo's existing calibration,
and create a servo with a custom calibration.
*/

using namespace servo;

// Function for printing out the details of a Calibration object
void print_calibration(const Calibration &calib) {
  printf("Calibration(");

  uint size = calib.size();
  printf("size = %d", size);

  printf(", pairs = {");
  for(uint i = 0; i < size; i++) {
      const Calibration::Pair &pair = calib.pair(i);
      printf("{%f, %f}",pair.pulse, pair.value);
      if(i < size - 1)
          printf(", ");
  }
  printf("}, limit_lower = %s", calib.has_lower_limit() ? "true" : "false");
  printf(", limit_upper = %s", calib.has_upper_limit() ? "true" : "false");
  printf(")\n\n");
}

int main() {
  stdio_init_all();

  // Sleep 8 seconds to give enough time to connect up a terminal
  sleep_ms(8000);

  // -----------------------------------------------------
  // Create and modify the calibration of an angular servo
  // -----------------------------------------------------

  // Create an angular servo on pin 0. By default its value ranges from -90 to +90
  Servo angular_servo = Servo(servo2040::SERVO_1, ANGULAR);

  // Get a reference to the calibration and print it out
  Calibration &acal = angular_servo.calibration();
  printf("Angular Servo: ");
  print_calibration(acal);

  // The range we want the anglular servo to cover
  constexpr float WIDE_ANGLE_RANGE = 270.0f;

  // Lets modify the calibration to increase its range
  acal.first_value(-WIDE_ANGLE_RANGE / 2);
  acal.last_value(WIDE_ANGLE_RANGE / 2);

  // As the calibration was a reference, the servo has already
  // been updated, but lets access it again to confirm it worked
  printf("Wide Angle Servo: ");
  print_calibration(angular_servo.calibration());


  // ---------------------------------------------------
  // Create and modify the calibration of a linear servo
  // ---------------------------------------------------

  // The range we want the linear servo to cover
  constexpr float LINEAR_RANGE = 50.0f;

  // Create a linear servo on pin 1. By default its value ranges from 0.0 to 1.0
  Servo linear_servo = Servo(servo2040::SERVO_2, LINEAR);

  // Update the linear servo so its max value matches the real distance travelled
  Calibration &lcal = linear_servo.calibration();
  lcal.last_value(LINEAR_RANGE);

  // As the calibration was a reference, the servo has already
  // been updated, but lets access it again to confirm it worked
  printf("Linear Servo: ");
  print_calibration(linear_servo.calibration());


  // ----------------------------------------------------------------
  // Create and modify the calibration of a continuous rotation servo
  // ----------------------------------------------------------------

  // The speed we want the continuous servo to cover
  constexpr float CONTINUOUS_SPEED = 10.0f;

  // Create a continous rotation servo on pin 2. By default its value ranges from -1.0 to +1.0
  Servo continuous_servo = Servo(servo2040::SERVO_3, CONTINUOUS);

  // Update the continuous rotation servo so its value matches its real speed
  Calibration &ccal = continuous_servo.calibration();
  ccal.first_value(-CONTINUOUS_SPEED);
  ccal.last_value(CONTINUOUS_SPEED);

  // As the calibration was a reference, the servo has already
  // been updated, but lets access it again to confirm it worked
  printf("Continuous Servo: ");
  print_calibration(continuous_servo.calibration());


  // ------------------------------------------------------
  // Create a custom calibration and build a servo using it
  // ------------------------------------------------------

  // Create an empty calibration
  Calibration ecal = Calibration();

  // Give it a range of -45 to 45 degrees, corresponding to pulses of 1000 and 2000 microseconds
  ecal.apply_two_pairs(1000, 2000, -45, 45);

  // Turn off the lower and upper limits, so the servo can go beyond 45 degrees
  ecal.limit_to_calibration(false, false);

  // Create a servo on pin 3 using the custom calibration and confirmed it worked
  Servo custom_servo = Servo(servo2040::SERVO_4, ecal);
  printf("Custom Servo: ");
  print_calibration(custom_servo.calibration());
}
