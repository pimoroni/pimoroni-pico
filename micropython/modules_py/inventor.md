# Pimoroni Inventor 2040/2350 W - Library Reference <!-- omit in toc -->

This is the library reference for the [Pimoroni Inventor 2040/2350 W](https://shop.pimoroni.com/products/inventor-2350-w), an all-in-one board for making battery powered contraptions that can move, (optionally) make noise, and talk to the internet!

## Table of Content <!-- omit in toc -->
- [Getting Started](#getting-started)
- [Reading the User Button](#reading-the-user-button)
- [Controlling Motors](#controlling-motors)
- [Reading Encoders](#reading-encoders)
- [Controlling Servos](#controlling-servos)
- [Controlling RGB LEDs](#controlling-rgb-leds)
  - [RGB](#rgb)
  - [HSV](#hsv)
- [Connecting Qw/ST Devices](#connecting-qwst-devices)
- [Playing Audio](#playing-audio)
  - [Play Tone](#play-tone)
  - [Play Silence](#play-silence)
  - [Stop Playing](#stop-playing)
  - [Volume Control](#volume-control)
  - [Muting/Unmuting](#mutingunmuting)
- [Program Lifecycle](#program-lifecycle)
- [Notes](#notes)
  - [Initialisation](#initialisation)
  - [Gear Ratios](#gear-ratios)
- [`inventor` Module Reference](#inventor-module-reference)
  - [Constants](#constants)
    - [IO Pin](#io-pin)
    - [Index](#index)
    - [Count](#count)
- [`Inventor` Class Reference](#inventor-class-reference)
  - [Constants](#constants-1)
  - [Variables](#variables)
  - [Functions](#functions)


## Getting Started

To start coding your Inventor board, add the following lines to the top of your code file.

```python
from inventor import Inventor

board = Inventor()
```

This will create an `Inventor` object called `board` that initialises all the features of Inventor. This will be used in the rest of the examples going forward.


## Reading the User Button

Inventor has one user button, labelled **User**. This can be read using the `switch_pressed()` function:

```python
state = board.switch_pressed()
```

A common use for this would be to control your program's main loop, so that a press of the button causes it to exit:

```python
while not board.switch_pressed():
    # Do some stuff here

# Shut the program down cleanly
```


## Controlling Motors

Inventor features a DRV8833 dual-channel motor driver that is suitable for driving small motors, either via the two 6-pin connectors or the 0.1" header. These motor channels are set up as `Motor` objects during `Inventor` creation (unless `init_motors=False` was supplied).

:information_source: **For details of what `Motor` objects can do, refer to the [Motor Library](/micropython/modules/motor/README.md#motor)**

The created `Motor` objects can be accessed in two ways, either through the `motor_a` and `motor_b` properties, or by the `motors` list.

```python
# Access the first motor by property
motor = board.motor_a
```

```python
# Access the first motor by list
motor = board.motors[0]
```

```python
# Access the first motor by list and constant
from inventor import MOTOR_A
motor = board.motors[MOTOR_A]
```

Accessing by property is convenient when you know the specific motor you wish to perform a motion, whereas accessing by list can be useful if you need to perform operations across both motors within a loop.

Here are examples of how the list may be looped through:

**For**
```python
# Loop through all motors
for motor in board.motors:
    # Do something with each motor here
```

**Range**
```python
# Loop through all motors via an index
from inventor import NUM_MOTORS
for i in range(NUM_MOTORS):
    motor = board.motors[i]
    # Do something with each motor that relates to its index here
```

**Enumerate**
```python
# Loop through all motors using enumerate
for i, motor in enumerate(board.motors):
    # Do something with each motor that relates to its index here
```

## Reading Encoders

Inventor features two 6-pin connectors for attaching Micro-Metal Motors with Encoders ([MMME]((https://shop.pimoroni.com/search?q=MMME))), allowing for precise position and velocity control. The encoders are set up as `Encoder` objects during `Inventor` creation (unless `init_encoders=False` was supplied).

:information_source: **For details of what `Encoder` objects can do, refer to the [Encoder Library](/micropython/modules/encoder/README.md#encoder)**

The created `Encoder` objects can be accessed in two ways, either through the `encoder_a` and `encoders_b` properties, or by the `encoders` list.

```python
# Access the first encoder by property
encoder = board.encoder_a
```

```python
# Access the first encoder by list
encoder = board.encoders[0]
```

```python
# Access the first encoder by list and constant
from inventor import MOTOR_A
encoder = board.encoders[MOTOR_A]
```

Accessing by property is convenient when you know the specific encoder you wish to read, whereas accessing by list can be useful if you need to perform operations on motor and encoder pairs.

Here are examples of how the list may be looped through:

**For**
```python
# Loop through all encoders
for encoder in board.encoders:
    # Do something with each encoder here
```

**Range**
```python
# Loop through all motors and encoders via an index
from inventor import NUM_MOTORS
for i in range(NUM_MOTORS):
    motor = board.motors[i]
    encoder = board.encoders[i]
    # Do something with each motor using its related encoder
```

**Enumerate**
```python
# Loop through all encoders using enumerate
for i, encoder in enumerate(board.encoders):
    motor = board.motors[i]
    # Do something with each motor using its related encoder
```


## Controlling Servos

There are six headers on Inventor 2040/2350 W for connecting 3-pin servos. These headers are set up as `Servo` objects during `Inventor` creation (unless `init_servos=False` was supplied).

:information_source: **For details of what `Servo` objects can do, refer to the [Servo Library](/micropython/modules/servo/README.md#servo)**

The created `Servo` objects can be accessed in two ways, either through the `servo1`, `servo2`, `servo3`, `servo4`, `servo5`, and `servo6` properties, or by the `servos` list.

```python
# Access the first servo by property
servo = board.servo1
```

```python
# Access the first servo by list
servo = board.servos[0]
```

```python
# Access the first servo by list and constant
from inventor import SERVO_1
servo = board.servos[SERVO_1]
```

Accessing by property is convenient when you know the specific servo you wish to perform a motion, whereas accessing by list can be useful if you need to perform operations across all servos within a loop.

Here are examples of how the list may be looped through:

**For**
```python
# Loop through all servos
for servo in board.servos:
    # Do something with each servo here
```

**Range**
```python
# Loop through all servos via an index
from inventor import NUM_SERVOS
for i in range(NUM_SERVOS):
    servo = board.servos[i]
    # Do something with each servo that relates to its index here
```

**Enumerate**
```python
# Loop through all servos using enumerate
for i, servo in enumerate(board.servos):
    # Do something with each servo that relates to its index here
```


## Controlling RGB LEDs

Inventor 2040/2350 W has twelve addressable RGB LEDs, positioned alongside its servo and GPIO/ADC pins. These make use of the `WS2812` class from our [`Plasma` Library](/micropython/modules/plasma/README.md#ws2812), which is set up automatically during `Inventor` creation.

The `WS2812` object can be accessed via `board.leds`. From this, you can set the colour of each LED in either the RGB colourspace, or HSV (Hue, Saturation, Value). 

### RGB
```python
# Set LED zero to full white
board.leds.set_rgb(0, 255, 255, 255)
```
The first parameter is the index of the LED (from `0` to `11`), and the rest are the R, G, and B components (from `0` to `255`).

### HSV
```python
# Set LED four to full green
board.leds.set_hsv(4, 0.333, 1.0, 1.0)
```
The first parameter is the index of the LED (from `0` to `11`), and the rest are the Hue, Saturation, and Value components (from `0.0` to `1.0`).

HSV is useful for creating rainbow patterns, like so:

```python
from inventor import NUM_LEDS
for i in range(NUM_LEDS):
    hue = float(i) / NUM_LEDS
    board.leds.set_hsv(i, hue, SATURATION, BRIGHTNESS)
```

## Connecting Qw/ST Devices

Inventor 2040/2350 W features two Qw/ST connectors for attaching I2C-based sensors and devices. These connectors are automatically initialised during class creation (unless `init_i2c=False` was supplied). The I2C bus can then be accessed by calling `board.i2c`.

Here is an example of setting up our [BME280 breakout](https://shop.pimoroni.com/products/bme280-breakout) on Inventor.

```python
from inventor import Inventor
from breakout_bme280 import BreakoutBME280

# Constants
ADDRESS = 0x76     # Or 0x77 if the trace on rear of the breakout has been cut

# Variables
board = Inventor()
bme = BreakoutBME280(board.i2c, ADDRESS)
```


## Playing Audio

Inventor 2040/2350 W features an audio amplifier and speaker output for adding chirps and tones to your project.

### Play Tone
```python
# Play a 2kHz tone
board.play_tone(2000)
```

### Play Silence
```python
# Silence the speaker by playing an inaudible tone (avoids popping sounds)
board.play_silence()
```

### Stop Playing
```python
# Stop playing the last tone (can cause a popping sound)
board.stop_playing()
```

### Volume Control
```python
# Get the last volume level (between 0.0 and 1.0)
value = board.volume()

# Set the new volume level (between 0.0 and 1.0)
board.volume(0.7)
```

### Muting/Unmuting
```python
# Mute the speaker
board.mute_audio()
```
```python
# Unmute the speaker
board.unmute_audio()
```

## Program Lifecycle

When writing a program for Inventor 2040/2350 W, there are a number of steps that should be included to make best use of the board's capabilities.


```python
# Perform system level imports here
# e.g. import math

from inventor import Inventor

"""
This is a boilerplate example for Inventor 2040/2350 W. Use it as a base for your own programs.

Press "User" to exit the program.
"""

# Constants
# e.g. SLEEP_TIME = 0.1

# Variables
board = Inventor()    # Create a new Inventor object. These optional keyword parameters are supported:
                      # `init_motors`, `init_servos`, `init_encoders`, and `init_i2c`. All are True by default

# Pull out any variables for easy access. E.g
motor = board.motor_a       # or board.motors[MOTOR_A]
encoder = board.encoder_a   # or board.encoders[MOTOR_A]
servo = board.servo1        # or board.servos[SERVO_1]


# Wrap the code in a try block, to catch any exceptions (including KeyboardInterrupt)
try:
    # Set up your motors, servos, LEDs, and audio here

    # Loop until the "User" button is pressed
    while not board.switch_pressed():
        #######################
        # Put your program here
        #######################
        pass

finally:
    # Stop your motors, servos, LEDs, and audio here
```


## Notes

### Initialisation

To make the starting experience of Inventor easier, all the board's features are set up when creating an `Inventor` object. For some advanced uses this may not be wanted, so optional parameters can be provided to skip steps of the set up.

```python
board = Inventor(init_motors=True, init_servos=True, init_encoders=True, init_i2c=True)
```

**Example uses:**
* `init_motors=False` - Lets you drive different kinds of magnetic loads, such as solenoids or stepper motors (with the appropriate code), via the new 1x4 header of Inventor 2350 W.
* `init_servos=False` - Lets you use the 6 servo headers as additional digital GPIO. Note these pins are not 5V tolerant so be careful when using the neighbouring 5V pins for powering devices.
* `init_encoders=False` - Lets you use the 4 encoder pins as additional digital GPIO. These are handily broken out to pads on the new Inventor 2350 W.
* `init_i2c=False` - Lets you use the 2 Qw/ST pins as additional digital GPIO for UART or similar.


### Gear Ratios

By default the `Inventor` class sets itself up for motors with encoders that have a 50:1 gear ratio, matching the pre-assembled [Micro Metal Motor Encoders](https://shop.pimoroni.com/products/micro-metal-gearmotor-with-micro-metal-motor-encoder) that we sell.

If you are soldering up your own motors with encoders that have a different gear ratio, then add the `motor_gear_ratio` parameter during class creation, like so:

```python
board = Inventor(motor_gear_ratio=<new ratio here>)
```

If you are using motors without encoders, then the `motor_gear_ratio` is not used and can be left at its default.

:information_source: The Inventor class assumes the connected motor's encoders have the same 12 counts per revolution as our MMME's. If yours are different you will need adjust your ratio to account for this.


## `inventor` Module Reference

### Constants

These are accessible by `from inventor import *`, where star is one or more of the below values, separated by commas.

#### IO Pin

```python
GP0 = 0
GP1 = 1
GP2 = 2

A0 = 26
A1 = 27
A2 = 28

GPIOS = (GP0, GP1, GP2, A0, A1, A2)
ADCS = (A0, A1, A2)
```

#### Index

```python
MOTOR_A = 0
MOTOR_B = 1

SERVO_1 = 0
SERVO_2 = 1
SERVO_3 = 2
SERVO_4 = 3
SERVO_5 = 4
SERVO_6 = 5

LED_GP0 = 0
LED_GP1 = 1
LED_GP2 = 2
LED_A0 = 3
LED_A1 = 4
LED_A2 = 5
LED_SERVO_1 = 6
LED_SERVO_2 = 7
LED_SERVO_3 = 8
LED_SERVO_4 = 9
LED_SERVO_5 = 10
LED_SERVO_6 = 11
```

#### Count

```python
NUM_GPIOS = 6
NUM_ADCS = 3
NUM_MOTORS = 2
NUM_ENCODERS = 2
NUM_SERVOS = 6
NUM_LEDS = 12
```


## `Inventor` Class Reference

### Constants

These are accessible after having imported `Inventor` class from the `inventor` module. E.g. `Inventor.SERVO_1_PIN`.

```python
AMP_EN_PIN = 3
I2C_SDA_PIN = 4
I2C_SCL_PIN = 5
MOTOR_A_PINS = (6, 7)
MOTOR_B_PINS = (8, 9)
ENCODER_A_PINS = (19, 18)
ENCODER_B_PINS = (17, 16)

SERVO_1_PIN = 10
SERVO_2_PIN = 11
SERVO_3_PIN = 12
SERVO_4_PIN = 13
SERVO_5_PIN = 14
SERVO_6_PIN = 15

LED_DATA_PIN = 20
PWM_AUDIO_PIN = 21
USER_SW_PIN = 22

AMP_CORRECTION = 4
DEFAULT_VOLUME = 0.2
```


### Variables
```python
# If init_motors was True
motors: list[Motor]

# If init_encoders was True
encoders: list[Encoder]

# If init_servos was True
servos: list[Servo]

# If init_i2c was True
i2c: PimoroniI2C

audio_pwm: PWM

leds: WS2812
```


### Functions

```python
# Initialisation
Inventor(motor_gear_ratio: int|float=50,
         init_motors: bool=True,
         init_servos: bool=True,
         init_encoders: bool=True,
         init_i2c: bool=True)

# Interaction
switch_pressed() -> bool

# Motor Access (only usable if init_motors was True)
@property
motor_a -> Motor
@property
motor_b -> Motor

# Encoder Access (only usable if init_encoders was True)
@property
encoder_a -> Encoder
@property
encoder_b -> Encoder

# Servo Access (only usable if init_servos was True)
@property
servo1 -> Servo
@property
servo2 -> Servo
@property
servo3 -> Servo
@property
servo4 -> Servo
@property
servo5 -> Servo
@property
servo6 -> Servo

# Audio
play_tone(frequency: float):
play_silence() -> None
stop_playing() -> None
volume() -> float
volume(volume: float) -> None
mute_audio() -> None
unmute_audio() -> None
```
