# Motor 2040 C++ Examples <!-- omit in toc -->

- [Motor Examples](#motor-examples)
  - [Single Motor](#single-motor)
  - [Multiple Motors](#multiple-motors)
  - [Motor Cluster](#motor-cluster)
  - [Motor Wave](#motor-wave)
- [Function Examples](#function-examples)
  - [Read Sensors](#read-sensors)
  - [Read Encoders](#read-encoders)
  - [Motor Profiler](#motor-profiler)
  - [LED Rainbow](#led-rainbow)
  - [Turn Off LEDs](#turn-off-leds)
- [Control Examples](#control-examples)
  - [Position Control](#position-control)
  - [Velocity Control](#velocity-control)
  - [Position on Velocity Control](#position-on-velocity-control)
  - [Reactive Encoder](#reactive-encoder)
  - [Quad Position Wave](#quad-position-wave)
  - [Quad Velocity Sequence](#quad-velocity-sequence)
- [Tuning Examples](#tuning-examples)
  - [Position Tuning](#position-tuning)
  - [Velocity Tuning](#velocity-tuning)
  - [Position on Velocity Tuning](#position-on-velocity-tuning)


## Motor Examples

### Single Motor
[motor2040_single_motor.cpp](motor2040_single_motor.cpp)

Demonstrates how to create a Motor object and control it.


### Multiple Motors
[motor2040_multiple_motors.cpp](motor2040_multiple_motors.cpp)

Demonstrates how to create multiple Motor objects and control them together.


### Motor Cluster
[motor2040_motor_cluster.cpp](motor2040_motor_cluster.cpp)

Demonstrates how to create a MotorCluster object to control multiple motors at once.


### Motor Wave
[motor2040_motor_wave.cpp](motor2040_motor_wave.cpp)

An example of applying a wave pattern to a group of motors and the LEDs.


## Function Examples

### Read Sensors
[motor2040_read_sensors.cpp](motor2040_read_sensors.cpp)

Shows how to initialise and read the 2 external and 6 internal sensors of Motor 2040.


### Read Encoders
[motor2040_read_encoders.cpp](motor2040_read_encoders.cpp)

Demonstrates how to read the angles of Motor 2040's four encoders.


### Motor Profiler
[motor2040_motor_profiler.cpp](motor2040_motor_profiler.cpp)

A program that profiles the speed of a motor across its PWM
duty cycle range using the attached encoder for feedback.


### LED Rainbow
[motor2040_led_rainbow.cpp](motor2040_led_rainbow.cpp)

Displays a rotating rainbow pattern on the Motor 2040's onboard LED.


## Control Examples

### Position Control
[motor2040_position_control.cpp](motor2040_position_control.cpp)

An example of how to move a motor smoothly between random positions, with the help of it's attached encoder and PID control.


### Velocity Control
[motor2040_velocity_control.cpp](motor2040_velocity_control.cpp)

An example of how to drive a motor smoothly between random speeds, with the help of it's attached encoder and PID control.


### Position on Velocity Control
[motor2040_position_on_velocity_control.cpp](motor2040_position_on_velocity_control.cpp)

An example of how to move a motor smoothly between random positions, with velocity limits, with the help of it's attached encoder and PID control.


### Reactive Encoder
[motor2040_reactive_encoder.cpp](motor2040_reactive_encoder.cpp)

A demonstration of how a motor with an encoder can be used as a programmable rotary encoder for user input, with force-feedback for arbitrary detents and end stops.


### Quad Position Wave
[motor2040_quad_position_wave.cpp](motor2040_quad_position_wave.cpp)

A demonstration of driving all four of Motor 2040's motor outputs between positions, with the help of their attached encoders and PID control.


### Quad Velocity Sequence
[motor2040_quad_velocity_sequence.cpp](motor2040_quad_velocity_sequence.cpp)

A demonstration of driving all four of Motor 2040's motor outputs through a sequence of velocities, with the help of their attached encoders and PID control.


## Tuning Examples

### Position Tuning
[motor2040_position_tuning.cpp](motor2040_position_tuning.cpp)

A program to aid in the discovery and tuning of motor PID values for position control. It does this by commanding the motor to move repeatedly between two setpoint angles and plots the measured response.


### Velocity Tuning
[motor2040_velocity_tuning.cpp](motor2040_velocity_tuning.cpp)

A program to aid in the discovery and tuning of motor PID values for velocity control. It does this by commanding the motor to drive repeatedly between two setpoint speeds and plots the measured response.


### Position on Velocity Tuning
[motor2040_position_on_velocity_tuning.cpp](motor2040_position_on_velocity_tuning.cpp)

A program to aid in the discovery and tuning of motor PID values for position on velocity control. It does this by commanding the motor to move repeatedly between two setpoint angles and plots the measured response.
