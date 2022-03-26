# Servo 2040 C++ Examples <!-- omit in toc -->

- [Servo Examples](#servo-examples)
  - [Single Servo](#single-servo)
  - [Multiple Servos](#multiple-servos)
  - [Servo Cluster](#servo_cluster)
  - [Simple Easing](#simple-easing)
  - [Servo Wave](#servo-wave)
  - [Calibration](#calibration)
- [Function Examples](#function-examples)
  - [Read Sensors](#read-sensors)
  - [Sensor Feedback](#sensor-feedback)
  - [Current Meter](#current-meter)
  - [LED Rainbow](#led-rainbow)


## Servo Examples

### Single Servo
[servo2040_single_servo.cpp](servo2040_single_servo.cpp)

Demonstrates how to create a Servo object and control it.


### Multiple Servos
[servo2040_multiple_servos.cpp](servo2040_multiple_servos.cpp)

Demonstrates how to create multiple Servo objects and control them together.


### Servo Cluster
[servo2040_servo_cluster.cpp](servo2040_servo_cluster.cpp)

Demonstrates how to create a ServoCluster object to control multiple servos at once.


### Simple Easing
[servo2040_simple_easing.cpp](servo2040_simple_easing.cpp)

An example of how to move a servo smoothly between random positions.


### Servo Wave
[servo2040_servo_wave.cpp](servo2040_servo_wave.cpp)

An example of applying a wave pattern to a group of servos and the LEDs.


### Calibration
[servo2040_calibration.cpp](servo2040_calibration.cpp)

Shows how to create servos with different common calibrations, modify a servo's existing calibration, and create a servo with a custom calibration.


## Function Examples

### Read Sensors
[servo2040_read_sensors.cpp](servo2040_read_sensors.cpp)

Shows how to initialise and read the 6 external and 2 internal sensors of Servo 2040.


### Sensor Feedback
[servo2040_sensor_feedback.cpp](servo2040_sensor_feedback.cpp)

Show how to read the 6 external sensors and display their values on the neighbouring LEDs.


### Current Meter
[servo2040_current_meter.cpp](servo2040_current_meter.cpp)

An example of how to use Servo 2040's current measuring ability and display the value on the onboard LED bar.


### LED Rainbow
[servo2040_led_rainbow.cpp](servo2040_led_rainbow.cpp)

Displays a rotating rainbow pattern on the Servo 2040's onboard LED bar.
