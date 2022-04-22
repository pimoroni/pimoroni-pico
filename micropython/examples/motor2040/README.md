# Motor 2040 Micropython Examples <!-- omit in toc -->

- [Motor Examples](#motor-examples)
  - [Single Motor](#single-motor)
  - [Multiple Motors](#multiple-motors)
  - [Motor Cluster](#motor-cluster)
  - [Motor Wave](#motor-wave)
  - [Turn Off Motors](#turn-off-motors)
- [Encoder Examples](#encoder-examples)
  - [Motor Angles](#motor-angles)
  - [Motor Profiler](#motor-profiler)
- [Function Examples](#function-examples)
  - [Read Sensors](#read-sensors)
  - [Sensor Feedback](#sensor-feedback)
  - [Current Meter](#current-meter)
  - [LED Rainbow](#led-rainbow)
  - [Turn Off LEDs](#turn-off-leds)
- [Control Examples](#control-examples)
  - [Position Control](#position-control)
  - [Velocity Control](#velocity-control)
  - [Position on Velocity Control](#position-on-velocity-control)
  - [Reactive Encoder](#reactive-encoder)
- [Tuning Examples](#tuning-examples)
  - [Position Tuning](#position-tuning)
  - [Velocity Tuning](#velocity-tuning)
  - [Position on Velocity Tuning](#position-on-velocity-tuning)


## Motor Examples

### Single Motor
[single_motor.py](single_motor.py)

Demonstrates how to create a Motor object and control it.


### Multiple Motors
[multiple_motors.py](multiple_motors.py)

Demonstrates how to create multiple Motor objects and control them together.


### Motor Cluster
[motor_cluster.py](motor_cluster.py)

Demonstrates how to create a MotorCluster object to control multiple motors at once.


### Motor Wave
[motor_wave.py](motor_wave.py)

An example of applying a wave pattern to a group of motors and the LEDs.


### Turn Off Motors
[turn_off_motors.py](turn_off_motors.py)

A simple program that turns off the motors.


## Encoder Examples

### Motor Angles
[motor_angles.py](motor_angles.py)

Demonstrates how to read the angles of Motor 2040's four encoders.


### Motor Profiler
[motor_profiler.py](motor_profiler.py)

A program that profiles the speed of a motor across its PWM
duty cycle range using the attached encoder for feedback.


## Function Examples

### Read Sensors
[read_sensors.py](read_sensors.py)
TODO
Shows how to initialise and read the 6 external and 2 internal sensors of Motor 2040.


### Sensor Feedback
[sensor_feedback.py](sensor_feedback.py)
TODO
Show how to read the 6 external sensors and display their values on the neighbouring LEDs.


### Current Meter
[current_meter.py](current_meter.py)
TODO
An example of how to use Motor 2040's current measuring ability and display the value on the onboard LED bar.


### LED Rainbow
[led_rainbow.py](led_rainbow.py)

Displays a rotating rainbow pattern on the Motor 2040's onboard LED.


### Turn Off LED
[turn_off_led.py](turn_off_led.py)

A simple program that turns off the onboard LED.


## Control Examples

### Position Control
[position_control.py](position_control.py)

An example of how to move a motor smoothly between random positions, with the help of it's attached encoder and PID control.


### Velocity Control
[velocity_control.py](velocity_control.py)

An example of how to drive a motor smoothly between random speeds, with the help of it's attached encoder and PID control.


### Position on Velocity Control
[position_on_velocity_control.py](position_on_velocity_control.py)

An example of how to move a motor smoothly between random positions, with velocity limits, with the help of it's attached encoder and PID control.


### Reactive Encoder
[reactive_encoder.py](reactive_encoder.py)

A demonstration of how a motor with an encoder can be used as a programmable rotary encoder for user input, with force-feedback for arbitrary detents and end stops.


## Tuning Examples

### Position Tuning
[position_tuning.py](position_tuning.py)

A program to aid in the discovery and tuning of motor PID values for position control. It does this by commanding the motor to move repeatedly between two setpoint angles and plots the measured response.


### Velocity Tuning
[velocity_tuning.py](velocity_tuning.py)

A program to aid in the discovery and tuning of motor PID values for velocity control. It does this by commanding the motor to drive repeatedly between two setpoint speeds and plots the measured response.


### Position on Velocity Tuning
[position_on_velocity_tuning.py](position_on_velocity_tuning.py)

A program to aid in the discovery and tuning of motor PID values for position on velocity control. It does this by commanding the motor to move repeatedly between two setpoint angles and plots the measured response.
