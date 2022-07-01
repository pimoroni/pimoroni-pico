# Inventor 2040 W Micropython Examples <!-- omit in toc -->

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

## Function Examples

### Read ADCs
[read_adcs.py](read_adcs.py)

Shows how to initialise and read the 3 ADC headers of Inventor 2040 W.


### Read GPIOs
[read_gpios.py](read_gpios.py)

Shows how to initialise and read the 6 GPIO headers of Inventor 2040 W.


### Read Encoders
[read_encoders.py](read_encoders.py)

Demonstrates how to read the angles of Inventor 2040 W's two encoders.


### LED Rainbow
[led_rainbow.py](led_rainbow.py)

Displays a rotating rainbow pattern on Inventor 2040 W's onboard LED bars.


### Reset Inventor
[reset_inventor.py](reset_inventor.py)

A simple program that resets Inventor 2040 W, turning off its LEDs, Motors, Servos, and Audio.


## Motor Examples

### Single Motor
[motors/single_motor.py](motors/single_motor.py)

Demonstrates how to control a motor on Inventor 2040 W.


### Dual Motors
[motors/dual_motors.py](motors/dual_motors.py)

Demonstrates how to control both motors on Inventor 2040 W.


### Motor Wave
[motors/motor_wave.py](motors/motor_wave.py)

An example of applying a wave pattern to Inventor 2040 W's motors and LEDs.


### Position Control
[motors/position_control.py](motors/position_control.py)

An example of how to move a motor smoothly between random positions, with the help of it's attached encoder and PID control.


### Velocity Control
[motors/velocity_control.py](motors/velocity_control.py)

An example of how to drive a motor smoothly between random speeds, with the help of it's attached encoder and PID control.


### Position on Velocity Control
[motors/position_on_velocity_control.py](motors/position_on_velocity_control.py)

An example of how to move a motor smoothly between random positions, with velocity limits, with the help of it's attached encoder and PID control.


### Reactive Encoder
[motors/reactive_encoder.py](motors/reactive_encoder.py)

A demonstration of how a motor with an encoder can be used as a programmable rotary encoder for user input, with force-feedback for arbitrary detents and end stops.


### Position Wave
[motors/position_wave.py](motors/position_wave.py)

A demonstration of driving both of Inventor 2040 W's motor outputs between positions, with the help of their attached encoders and PID control.


### Driving Sequence
[motors/driving_sequence.py](motors/driving_sequence.py)

A demonstration of driving both of Inventor 2040 W's motor outputs through a sequence of velocities, with the help of their attached encoders and PID control.


## Motor Tuning

### Motor Profiler
[motors/tuning/motor_profiler.py](motors/tuning/motor_profiler.py)

A program that profiles the speed of a motor across its PWM duty cycle range using the attached encoder for feedback.


### Position Tuning
[motors/tuning/position_tuning.py](motors/tuning/position_tuning.py)

A program to aid in the discovery and tuning of motor PID values for position control. It does this by commanding the motor to move repeatedly between two setpoint angles and plots the measured response.


### Velocity Tuning
[motors/tuning/velocity_tuning.py](motors/tuning/velocity_tuning.py)

A program to aid in the discovery and tuning of motor PID values for velocity control. It does this by commanding the motor to drive repeatedly between two setpoint speeds and plots the measured response.


### Position on Velocity Tuning
[motors/tuning/position_on_velocity_tuning.py](motors/tuning/position_on_velocity_tuning.py)

A program to aid in the discovery and tuning of motor PID values for position on velocity control. It does this by commanding the motor to move repeatedly between two setpoint angles and plots the measured response.


## Servo Examples

### Single Servo
[servos/single_servo.py](servos/single_servo.py)

Demonstrates how to control a single servo on Inventor 2040 W.


### Multiple Servos
[servos/multiple_servos.py](servos/multiple_servos.py)

Demonstrates how to control all of the servos on Inventor 2040 W.


### Simple Easing
[servos/simple_easing.py](servos/simple_easing.py)

An example of how to move a servo smoothly between random positions.


### Servo Wave
[servos/servo_wave.py](servos/servo_wave.py)

An example of applying a wave pattern to a group of servos and the LEDs.


### Calibration
[servos/calibration.py](servos/calibration.py)

Shows how to configure Inventor 2040 W's servos with different common calibrations, as well as a completely custom one.


## Audio Examples

### Tone Song
[audio/tone_song.py](audio/tone_song.py)

This example shows you how you can use Inventor 2040 W's audio output with a speaker to play different notes and string them together into a bleepy tune.

### Motor Song
[audio/motor_song.py](audio/motor_song.py)

A fun example of how to change a motor's frequency to have it play a song.