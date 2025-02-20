# Inventor 2040 W C++ Examples <!-- omit in toc -->

- [Function Examples](#function-examples)
  - [Read ADCs](#read-adcs)
  - [Read GPIOs](#read-gpios)
  - [Read Encoders](#read-encoders)
  - [LED Rainbow](#led-rainbow)
  - [Reset Inventor](#reset-inventor)
- [Motor Examples](#motor-examples)
  - [Single Motor](#single-motor)
  - [Dual Motors](#dual-motors)
  - [Motor Wave](#motor-wave)
  - [Position Control](#position-control)
  - [Velocity Control](#velocity-control)
  - [Position on Velocity Control](#position-on-velocity-control)
  - [Reactive Encoder](#reactive-encoder)
  - [Position Wave](#position-wave)
  - [Driving Sequence](#driving-sequence)
- [Motor Tuning](#motor-tuning)
  - [Motor Profiler](#motor-profiler)
  - [Position Tuning](#position-tuning)
  - [Velocity Tuning](#velocity-tuning)
  - [Position on Velocity Tuning](#position-on-velocity-tuning)
- [Servo Examples](#servo-examples)
  - [Single Servos](#single-servo)
  - [Multiple Servos](#multiple-servos)
  - [Simple Easing](#simple-easing)
  - [Servo Wave](#servo-wave)
  - [Calibration](#calibration)
- [Audio Examples](#audio-examples)
  - [Tone Song](#tone-song)
  - [Motor Song](#motor-song)

## Function Examples

### Read ADCs
[inventor2040w_read_adcs.cpp](inventor2040w_read_adcs.cpp)

Shows how to initialise and read the 3 ADC headers of Inventor 2040 W.


### Read GPIOs
[inventor2040w_read_gpios.cpp](inventor2040w_read_gpios.cpp)

Shows how to initialise and read the 6 GPIO headers of Inventor 2040 W.


### Read Encoders
[inventor2040w_read_encoders.cpp](inventor2040w_read_encoders.cpp)

Demonstrates how to read the angles of Inventor 2040 W's two encoders.


### LED Rainbow
[inventor2040w_led_rainbow.cpp](inventor2040w_led_rainbow.cpp)

Displays a rotating rainbow pattern on Inventor 2040 W's onboard LED bars.


## Motor Examples

### Single Motor
[motors/inventor2040w_single_motor.cpp](motors/inventor2040w_single_motor.cpp)

Demonstrates how to control a motor on Inventor 2040 W.


### Dual Motors
[motors/inventor2040w_dual_motors.cpp](motors/inventor2040w_dual_motors.cpp)

Demonstrates how to control both motors on Inventor 2040 W.


### Motor Wave
[motors/inventor2040w_motor_waver.cpp](motors/inventor2040w_motor_wave.cpp)

An example of applying a wave pattern to Inventor 2040 W's motors and LEDs.


### Position Control
[motors/inventor2040w_position_control.cpp](motors/inventor2040w_position_control.cpp)

An example of how to move a motor smoothly between random positions, with the help of it's attached encoder and PID control.


### Velocity Control
[motors/inventor2040w_velocity_control.cpp](motors/inventor2040w_velocity_control.cpp)

An example of how to drive a motor smoothly between random speeds, with the help of it's attached encoder and PID control.


### Position on Velocity Control
[motors/inventor2040w_position_on_velocity_control.cpp](motors/inventor2040w_position_on_velocity_control.cpp)

An example of how to move a motor smoothly between random positions, with velocity limits, with the help of it's attached encoder and PID control.


### Reactive Encoder
[motors/inventor2040w_reactive_encoder.cpp](motors/inventor2040w_reactive_encoder.cpp)

A demonstration of how a motor with an encoder can be used as a programmable rotary encoder for user input, with force-feedback for arbitrary detents and end stops.


### Position Wave
[motors/inventor2040w_position_wave.cpp](motors/inventor2040w_position_wave.cpp)

A demonstration of driving both of Inventor 2040 W's motor outputs between positions, with the help of their attached encoders and PID control.


### Driving Sequence
[motors/inventor2040w_driving_sequence.cpp](motors/inventor2040w_driving_sequence.cpp)

A demonstration of driving both of Inventor 2040 W's motor outputs through a sequence of velocities, with the help of their attached encoders and PID control.


## Motor Tuning

### Motor Profiler
[motors/tuning/inventor2040w_motor_profiler.cpp](motors/tuning/inventor2040w_motor_profiler.cpp)

A program that profiles the speed of a motor across its PWM duty cycle range using the attached encoder for feedback.


### Position Tuning
[motors/tuning/inventor2040w_position_tuning.cpp](motors/tuning/inventor2040w_position_tuning.cpp)

A program to aid in the discovery and tuning of motor PID values for position control. It does this by commanding the motor to move repeatedly between two setpoint angles and plots the measured response.


### Velocity Tuning
[motors/tuning/inventor2040w_velocity_tuning.cpp](motors/tuning/inventor2040w_velocity_tuning.cpp)

A program to aid in the discovery and tuning of motor PID values for velocity control. It does this by commanding the motor to drive repeatedly between two setpoint speeds and plots the measured response.


### Position on Velocity Tuning
[motors/tuning/inventor2040w_position_on_velocity_tuning.cpp](motors/tuning/inventor2040w_position_on_velocity_tuning.cpp)

A program to aid in the discovery and tuning of motor PID values for position on velocity control. It does this by commanding the motor to move repeatedly between two setpoint angles and plots the measured response.


## Servo Examples

### Single Servo
[servos/inventor2040w_single_servo.cpp](servos/inventor2040w_single_servo.cpp)

Demonstrates how to control a single servo on Inventor 2040 W.


### Multiple Servos
[servos/inventor2040w_multiple_servos.cpp](servos/inventor2040w_multiple_servos.cpp)

Demonstrates how to control all of the servos on Inventor 2040 W.


### Simple Easing
[servos/inventor2040w_simple_easing.cpp](servos/inventor2040w_simple_easing.cpp)

An example of how to move a servo smoothly between random positions.


### Servo Wave
[servos/inventor2040w_servo_wave.cpp](servos/inventor2040w_servo_wave.cpp)

An example of applying a wave pattern to a group of servos and the LEDs.


### Calibration
[servos/inventor2040w_calibration.cpp](servos/inventor2040w_calibration.cpp)

Shows how to configure Inventor 2040 W's servos with different common calibrations, as well as a completely custom one.


## Audio Examples

### Tone Song
[audio/inventor2040w_tone_song.cpp](audio/inventor2040w_tone_song.cpp)

This example shows you how you can use Inventor 2040 W's audio output with a speaker to play different notes and string them together into a bleepy tune.


### Motor Song
[audio/inventor2040w_motor_song.cpp](audio/inventor2040w_motor_song.cpp)

A fun example of how to change a motor's frequency to have it play a song.
