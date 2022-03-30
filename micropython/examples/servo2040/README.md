# Servo 2040 Micropython Examples <!-- omit in toc -->

- [Servo Examples](#servo-examples)
  - [Single Servo](#single-servo)
  - [Multiple Servos](#multiple-servos)
  - [Servo Cluster](#servo-cluster)
  - [Simple Easing](#simple-easing)
  - [Servo Wave](#servo-wave)
  - [Calibration](#calibration)
- [Function Examples](#function-examples)
  - [Read Sensors](#read-sensors)
  - [Sensor Feedback](#sensor-feedback)
  - [Current Meter](#current-meter)
  - [LED Rainbow](#led-rainbow)
  - [Turn Off LEDs](#turn-off-leds)


## Servo Examples

### Single Servo
[single_servo.py](single_servo.py)

Demonstrates how to create a Servo object and control it.


### Multiple Servos
[multiple_servos.py](multiple_servos.py)

Demonstrates how to create multiple Servo objects and control them together.


### Servo Cluster
[servo_cluster.py](servo_cluster.py)

Demonstrates how to create a ServoCluster object to control multiple servos at once.


### Simple Easing
[simple_easing.py](simple_easing.py)

An example of how to move a servo smoothly between random positions.


### Servo Wave
[servo_wave.py](servo_wave.py)

An example of applying a wave pattern to a group of servos and the LEDs.


### Calibration
[calibration.py](calibration.py)

Shows how to create servos with different common calibrations, modify a servo's existing calibration, and create a servo with a custom calibration.


## Function Examples

### Read Sensors
[read_sensors.py](read_sensors.py)

Shows how to initialise and read the 6 external and 2 internal sensors of Servo 2040.


### Sensor Feedback
[sensor_feedback.py](sensor_feedback.py)

Show how to read the 6 external sensors and display their values on the neighbouring LEDs.


### Current Meter
[current_meter.py](current_meter.py)

An example of how to use Servo 2040's current measuring ability and display the value on the onboard LED bar.


### LED Rainbow
[led_rainbow.py](led_rainbow.py)

Displays a rotating rainbow pattern on the Servo 2040's onboard LED bar.


### Turn Off LEDs
[turn_off_leds.py](turn_off_leds.py)

A simple program that turns off the onboard LED bar.
