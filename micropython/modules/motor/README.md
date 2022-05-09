# Motors and Motor 2040 <!-- omit in toc -->

The Motor library lets you drive DC motors from a Raspberry Pi Pico or any other RP2040-based board via connected h-bridge drivers.
An easy way to add an h-bridge driver to the Pico is to attach a [Pimoroni Pico Motor Shim](https://pimoroni.com/picomotorshim).
Alternatively, an RP2040-based board with integrated drivers could be used, such as the [Pimoroni Motor 2040](https://pimoroni.com/motor2040) (coming soon).

This library offers two motor implementations:
* a `Motor` class that uses hardware PWM to drive a single motor, with support for up to 8 motors.
* a `MotorCluster` class that uses Programmable IO (PIO) hardware to drive up to 15 motors.


## Table of Content
- [Pico Motor Shim](#pico-motor-shim)
  - [Reading the User Button](#reading-the-user-button)
  - [Pin Constants](#pin-constants)
    - [Motor Pins](#motor-pins)
    - [I2C Pins](#i2c-pins)
    - [Button Pin](#button-pin)
  - [Other Constants](#other-constants)

- [Motor 2040](#motor-2040)
  - [Reading the User Button](#reading-the-user-button-1)
  - [Reading the Sensors](#reading-the-sensors)
    - [Configuring Pulls](#configuring-pulls)
    - [Fault Sensing](#fault-sensing)
  - [Reading the Encoders](#reading-the-encoders)
  - [Controlling the LED](#controlling-the-led)
  - [Pin Constants](#pin-constants-1)
    - [Motor Pins](#motor-pins-1)
    - [Encoder Pins](#encoder-pins-1)
    - [LED Pin](#led-pin)
    - [I2C Pins](#i2c-pins-1)
    - [Button Pin](#button-pin-1)
    - [Address Pins](#address-pins)
    - [ADC Pins](#adc-pins)
  - [Other Constants](#other-constants-1)
    - [Counts](#counts)
    - [Addresses](#addresses)
    - [Sensing](#sensing)
- [Motor](#motor)
  - [Getting Started](#getting-started)
  - [Control by Speed](#control-by-speed)
    - [Full Speed](#full-speed)
    - [Stopping](#stopping)
    - [Calibration](#calibration)
  - [Control by Percent](#control-by-percent)
  - [Control by Duty Cycle](#control-by-duty-cycle)
    - [Duty Deadzone](#duty-deadzone)
  - [Frequency Control](#frequency-control)
  - [Configuration](#configuration)
    - [Direction](#direction)
    - [Decay Mode](#decay-mode)
    - [Driver Type](#driver-type)
  - [Function Reference](#function-reference)
  - [Constants Reference](#constants-reference)
  - [PWM Limitations](#pwm-limitations)
- [MotorCluster](#motorcluster)
  - [Getting Started](#getting-started-1)
  - [Control by Speed](#control-by-speed-1)
  - [Control by Percent](#control-by-percent-1)
  - [Control by Duty Cycle](#control-by-duty-cycle-1)
  - [Frequency Control](#frequency-control-1)
  - [Phase Control](#phase-control)
  - [Configuration](#configuration-1)
  - [Delayed Loading](#delayed-loading)
  - [Function Reference](#function-reference-1)
  - [PIO Limitations](#pio-limitations)


## Pico Motor Shim

Pico Motor Shim (or Motor Shim for Pico, if you prefer) is a small board featuring a DRV8833 dual h-bridge motor driver IC, a user button, and a Qw/st connector. It attaches to the underside of a Raspberry Pi Pico towards the USB end, and connects to two motors with Motor Connector Shims (MCS) attached via 2 pin JST-ZH cables.

For more information on this board and its accessories, check out the store page: [pimoroni.com/picomotorshim](https://pimoroni.com/picomotorshim).


### Reading the User Button

The Pico Motor Shim has a handy button onboard, offering a tactile way to interact with your program. To simplify the use of this and other buttons, the `pimoroni` module contains a `Button` class that handles button debounce and auto-repeat.

```python
Button(button, invert=True, repeat_time=200, hold_time=1000)
```

To set up the user button, first import the `Button` class from the `pimoroni` module and the pin constant for the button from `motor`:
```python
from pimoroni import Button
from motor import picomotorshim
```

Then create an instance of `Button` for the user button:
```python
button_a = Button(picomotorshim.BUTTON_A)
```

To get the button state, call `.read()`. If the button is held down, then this will return `True` at the interval specified by `repeat_time` until `hold_time` is reached, at which point it will return `True` every `repeat_time / 3` milliseconds. This is useful for rapidly increasing/decreasing values:

```python
state = button_a.read()
```

It is also possible to read the raw button state without the repeat feature, if you prefer:
```python
state = button_a.raw()
```


### Pin Constants

The `motor` module contains a `picomotorshim` sub module with constants for the motor and button pins.


#### Motor Pin Tuples

* `MOTOR_1` = `(6, 7)`
* `MOTOR_2` = `(27, 26)`


#### I2C Pins

* `SDA` = `4`
* `SCL` = `5`


#### Button Pin

* `BUTTON_A` = `2`


### Other Constants

The `picomotorshim` sub module also contains a constant for the number of motors on Pico Motor Shim:

* `NUM_MOTORS` = `2`


## Motor 2040

Motor 2040 is a **standalone motor controller** for driving DC motors with encoder feedback. It has JST-SH connectors for plugging in up to 4 motors, with additional bells and whistles like sensor headers, current and voltage monitoring, an RGB LED, and a user button that make it ideal for many robotics projects!

For more information on this board, check out the store page: [pimoroni.com/motor2040](https://pimoroni.com/motor2040).


### Reading the User Button

As is the case for many of Pimoroni's RP2040-based boards, the boot button on Motor 2040 that is used for programming also acts as a user button once a program is running. To simplify the use of this and other buttons, the `pimoroni` module contains a `Button` class that handles button debounce and auto-repeat.

```python
Button(button, invert=True, repeat_time=200, hold_time=1000)
```

To set up the user button, first import the `Button` class from the `pimoroni` module and the pin constant for the button from `motor`:
```python
from pimoroni import Button
from motor import motor2040
```

Then create an instance of `Button` for the user button:
```python
user_sw = Button(motor2040.USER_SW)
```

To get the button state, call `.read()`. If the button is held down, then this will return `True` at the interval specified by `repeat_time` until `hold_time` is reached, at which point it will return `True` every `repeat_time / 3` milliseconds. This is useful for rapidly increasing/decreasing values:

```python
state = user_sw.read()
```

It is also possible to read the raw button state without the repeat feature, if you prefer:
```python
state = user_sw.raw()
```


### Reading the Sensors

On the right-hand edge of Motor 2040 are two analog inputs, with neighbouring 3.3V and GND. These let you connect up sensors to enable your mechanical creations to sense how they are interacting with the world. For example, a pair of analog proximity sensors could be hooked up for wall avoidance or line following, or they could have microswitches wired to report when a motor driven mechanism has reached an end-stop.

Motor 2040 also has six internal sensors:
* A voltage sensor, letting you measure the supply voltage to the motors.
* Four current sensors, letting you measure how much current each motor is drawing.
* A fault sensor, letting you know if there is an issue with one of more of your motors
These could be used just for monitoring, or as the trigger to turn off motors safely when voltage gets too low or current draw gets too high.

To allow for all of these inputs, Motor 2040 has an onboard analog multiplexer that expands a single analog pin into eight, by use of three digital address pins that select a single input at a time. As such, the setup for these sensors is more involved than it would be to just read eight analog pins directly.

To begin reading these inputs, first import the `Analog` and `AnalogMux` classes from `pimoroni` and the pin, address, and gain constants from `motor`:

```python
from pimoroni import Analog
from motor import motor2040
```

Then set up three instances of `Analog` for the sensor and fault, voltage, and current sensing:

```python
sen_adc = Analog(motor2040.SHARED_ADC)
vol_adc = Analog(motor2040.SHARED_ADC, motor2040.VOLTAGE_GAIN)
cur_adc = Analog(motor2040.SHARED_ADC, motor2040.CURRENT_GAIN,
                 motor2040.SHUNT_RESISTOR, motor2040.CURRENT_OFFSET)
```

You may notice, all three of these use the same `SHARED_ADC` pin. This is intentional as it is just a single pin that is being used for all three different functions, only the gains differ.

The next step is to set up the analog multiplexer, by providing it with the three address pins:
```python
mux = AnalogMux(motor2040.ADC_ADDR_0, motor2040.ADC_ADDR_1, motor2040.ADC_ADDR_2)
```
Note that the `AnalogMux` does not know about any of the `Analog` classes that were created before.

With the multiplexer now configured, reading each sensor is simply a case of 'selecting' the sensor on the multiplexer then reading the value from one of the three `Analog` classes created at the start.

To read the two sensor headers:
```python
for addr in range(motor2040.NUM_SENSORS):
    mux.select(addr + motor2040.SENSOR_1_ADDR)
    print("Sensor", addr + 1, "=", sen_adc.read_voltage())
```

To read the voltage sense:
```python
mux.select(motor2040.VOLTAGE_SENSE_ADDR)
print("Voltage =", vol_adc.read_voltage(), "V")
```

To read the current draw in amps (A):
```python
for addr in range(motor2040.NUM_MOTORS):
    mux.select(addr + motor2040.CURRENT_SENSE_A_ADDR)
    print("Current", addr + 1, "=", cur_adc.read_current(), "A")
```


#### Configuring Pulls

For the internal fault sensor, as well as some external sensors, you may need to have the input be pulled high or low before taking a reading. To support this there is an optional `muxed_pin` parameter that can be passed into the `AnalogMux` when creating it, which gives the multiplexer access to the pin to control the pulls.

```python
mux = AnalogMux(motor2040.ADC_ADDR_0, motor2040.ADC_ADDR_1, motor2040.ADC_ADDR_2,
                muxed_pin=Pin(motor2040.SHARED_ADC))
```

From there the pull state of each of the multiplexer's addresses can be configured independently by calling `.configure_pull()`, with the address and the pull state (either `Pin.PULL_UP`, `Pin.PULL_DOWN`, or `None`).

The below example shows how to set both sensor addresses to have pull-downs:
```python
for addr in range(motor2040.NUM_SENSORS):
    mux.configure_pull(addr + motor2040.SENSOR_1_ADDR, Pin.PULL_DOWN)
```

#### Fault Sensing

The drivers on Motor 2040 can detect when there is a fault with their connected motors and shut themselves off for safety. When this occurs they also signal the fault by pulling a line to ground, but requires that the line be pulled up by default.

The `AnalogMux` can be set with a pull up on the fault sensor line, by doing the following:

```python
mux.configure_pull(motor2040.FAULT_SENSE_ADDR, Pin.PULL_UP)
```

Then the fault can be read as a digital value calling `.read()` on the `AnalogMux` itself, with the value being inverted with `not` to give `True` for when a fault has occurred:
```python
mux.select(motor2040.FAULT_SENSE_ADDR)
print("Fault =", not mux.read())
```
This internally reads the value of the `Pin` provided to `muxed_pin` during the creation of the `mux` object, so could be used to read any digital sensors attached to the external sensor headers of Motor 2040 too.


### Reading the Encoders

On the top edge of Motor 2040 are four JST-SH 6 pin connectors for N20 style motors with magnetic encoders. With these encoders you can measure the position and speed or your motors, opening up many advanced possibilities!

To start using encoders, you will need to first import the `Encoder` class.
```python
from encoder import Encoder
```

To create your encoder, specify the PIO, PIO state-machine and GPIO pins it will be connected to, and pass them into `Encoder`. For this example we will use one of the handy constants of the `motor2040`.
```python
from encoder import MMME_CPR
from motor import motor2040
enc = Encoder(0, 0, motor2040.ENCODER_A, counts_per_rev=MMME_CPR, count_microsteps=True)
```
Motor encoders often have a different number of counts per revolution than a traditional rotary encoder. For our MMME shims the `MMME_CPR` constant is provided. Also, as motor encoders lack the tactile steps of rotary encoders, their counting resolution can be increased by a factor of four by including each microstep.

With the created encoder class, the current position can be read by calling `.revolutions()`, `.degrees()` or `.radians()`.

For full details on encoders, including how to read speeds, please refer to the [Encoder Library](https://github.com/pimoroni/pimoroni-pico/tree/main/micropython/modules/encoder).


### Controlling the LED

Between Motor 2040's four motor connectors is a single addressable RGB LEDs. This works using the same chainable 1-wire signalling as WS2812 LED's, commonly known as Neopixels. As such, it can be controlled using the Plasma Library, as used by the [Pimoroni Plasma 2040](https://shop.pimoroni.com/products/plasma-2040).

To set up the LED, first import the `WS2812` class from the `plasma` module and the pin constants for the LED from `motor`:
```python
from plasma import WS2812
from motor import motor2040
```

Then construct a new `WS2812` instance, specifying the number of LEDs, PIO, PIO state-machine and GPIO pin.
```python
led = WS2812(motor2040.NUM_LEDS, 1, 0, motor2040.LED_DATA)
```

Finally, start the LED by calling `start`:
```python
led.start()
```

For more information on how to control the LED, please refer to the [Plasma Library](https://github.com/pimoroni/pimoroni-pico/tree/main/micropython/modules/plasma).


### Pin Constants

The `motor` module contains a `motor2040` sub module with constants for the motor, encoder, LED, sensor and button pins.


#### Motor Pins

* `MOTOR_A` = `(4, 5)`
* `MOTOR_B` = `(6, 7)`
* `MOTOR_C` = `(8, 9)`
* `MOTOR_D` = `(10, 11)`


#### Encoder Pins

* `ENCODER_A` = `(0, 1)`
* `ENCODER_B` = `(2, 3)`
* `ENCODER_C` = `(12, 13)`
* `ENCODER_D` = `(14, 15)`


#### UART/ULTRASOUND Pins

* `TX_TRIG` = `16`
* `RX_ECHO` = `17`


#### LED Pin

* `LED_DATA` = `18`


#### I2C Pins

* `INT` = `19`
* `SDA` = `20`
* `SCL` = `21`


#### Button Pin

* `USER_SW` = `23`


#### Address Pins

* `ADC_ADDR_0` = `22`
* `ADC_ADDR_1` = `24`
* `ADC_ADDR_2` = `25`


#### ADC Pins

* `ADC0` = `26`
* `ADC1` = `27`
* `ADC2` = `28`
* `SHARED_ADC` = `29`


### Other Constants

The `motor2040` sub module also contains other constants to help with the control of Motor 2040's various features:


#### Counts

* `NUM_MOTORS` = `4`
* `NUM_ENCODERS` = `4`
* `NUM_SENSORS` = `2`
* `NUM_LEDS` = `1`


#### Addresses

* `CURRENT_SENSE_A_ADDR` = `0b000`
* `CURRENT_SENSE_B_ADDR` = `0b001`
* `CURRENT_SENSE_C_ADDR` = `0b010`
* `CURRENT_SENSE_D_ADDR` = `0b011`
* `VOLTAGE_SENSE_ADDR` = `0b100`
* `FAULT_SENSE_ADDR` = `0b101`
* `SENSOR_1_ADDR` = `0b110`
* `SENSOR_2_ADDR` = `0b111`


#### Sensing

* `VOLTAGE_GAIN` = `0.28058`
* `SHUNT_RESISTOR` = `0.47`
* `CURRENT_GAIN` = `1`
* `CURRENT_OFFSET` = `-0.005`


## Motor

### Getting Started

To start using motors with your Motor 2040, you will need to first import the `Motor` class.
```python
from motor import Motor, motor2040
```
If you are using another RP2040 based board, then `motor2040` can be omitted from the above line.

To create your motor, choose which GPIO pins it will be connected to, and pass that into `Motor`. For this example we will use one of the handy constants of the `motor2040`.
```python
m = Motor(motor2040.MOTOR_A)
```

You now have a `Motor` class called `m` that will control the physical motor connected to `MOTOR_A`. To start using this motor, simply enable it using:
```python
m.enable()
```

This activates the motor and sets it to its last known speed. Since this is the first time enabling the motor, there is no last known speed, so instead it will be zero.

Once you have finished with the motor, it can be disabled by calling:
```python
m.disable()
```

From here the motor can be controlled in several ways. These are covered in more detail in the following sections.


### Control by Speed

The most intuitive way of controlling a motor is by speed. Speed can be any number that has a real-world meaning for that type of motor, for example revolutions per minute, or the linear or angular speed of the mechanism it is driving. By default the speed is a value ranging from `-1.0` to `1.0` but this can be changed by setting a new `speed_scale`. See [Calibration](#calibration) for more details.

The speed of a motor can be set by calling `.speed(speed)`, which takes a float as its `speed` input. If the motor is disabled, this will enable it. The resulting duty cycle will also be stored.

To read back the current speed of the motor, call `.speed()` without any input. If the motor is disabled, this will be the last speed that was provided when enabled.


#### Full Speed

To simplify certain motion patterns, a motor can be commanded to its full negative, and full positive speeds. These are performed by calling `.full_negative()`, and `.full_positive()`, respectively. If the motor is disabled, these will enable it.

The value of the full negative and full positive speed can be read back using `.speed_scale()`. This can be useful as an input to equations that provide numbers directly to `.speed(speed)`, for example.


#### Stopping

The easiest way to stop a motor is by calling `.stop()`. This is equivalent to calling `.speed(0.0)` and stops the motor using the currently assigned decay mode of the `Motor` object. See [Decay Modes](#decay-modes) for more details.

It is also possible to explicitly have the motor coast or brake to a stop by calling `.coast()` or `.brake()`.

If the motor is disabled, these will enable it.


### Calibration

It is very rare for a motor to perfectly drive at the speed we want them to. As such, the `Motor` class offers two parameters for adjusting how the value provided to `.speed(speed)` is converted to the PWM duty cycle that is actually sent to the motor, a speed scale, and a zeropoint.

Speed scale, as the name implies, is a value that scalues the duty cycle up or down to better reflect the measured speed of the motor when driving at `.full_negative()` or `.full_positive()`. This can be set by calling `.speed_scale(speed_scale)`, which accepts a value greater than `0.0`. The current speed scale can also be read by calling `.speed_scale()`.

Zeropoint is a value that sets what duty cycle should count as the zero speed of the motor. By default this is `0.0` and usually it is fine to leave it at that, but there are cases at low speeds where the expected speed does not match the measured speed, which small adjustments to the zeropoint will fix. This can be set by calling `.zeropoint(zeropoint)`, which accepts a value from `0.0` to less than `1.0`. The current zeropoint can also be read by calling `.zeropoint()`.

Both parameters can also be provided during the creation of a new `Motor` object.


### Control by Percent

Sometimes there are projects where a motor needs to drive based on the reading from a sensor or another device, but the numbers given out are not easy to convert to speeds the motor accepts. To overcome this the library lets you drive the motor at a percent between its negative and positive speeds, or two speeds provided, based on that input.

With an input between `-1.0` and `1.0`, a motor can be set to a percent between its negative and positive speeds using `.to_percent(in)`.

With an input between a provided min and max, a motor can be set to a percent between its negative and positive speeds using `.to_percent(in, in_min, in_max)`.

With an input between a provided min and max, a motor can be set to a percent between two provided speeds using `.to_percent(in, in_min, value_min, value_max)`.

If the motor is disabled, these will enable it.


### Control by Duty Cycle

Motor drivers accept pulse width modulated (PWM) signals to control the speed and direction of their connected motors. The percentage of time that these signals are active for is know as their duty cycle. This is typically measured as a value between `0.0` and `1.0`, but as motors use two pins for their control signals, here negative values are added to denote the reverse direction.

The duty cycle of a motor can be set by calling `.duty(duty)`, which takes a float from `-1.0` to `1.0` as its `duty` input. If the motor is disabled this will enable it. This function will also recalculate the related speed.

To read back the current duty cycle of the motor, call `.duty()` without any input. If the motor is disabled, this will be the last duty that was provided when enabled.


#### Duty Deadzone

Most motors have a duty cycle value below which their is too much friction for them to move. This may not be a concern, except for when running motors at audable frequencies, where the buzzing of the motor trying to move without success can get annoying.

To overcome this, a duty cycle deadzone can be set on a per motor basis by calling `.deadzone(deadzone)`, which accepts a float from `0.0` to less than `1.0`. Whenever a duty cycle is set, either directly or via speed or percent functions, it will only be output to the motor if it is greater than or equal to the deadzone. If it's below, the motor will be stopped instead. By default the deadzone is `0.05`.

To read back the current duty deadzone of the motor, call `.deadzone()` without any input.

Note that the motor keeps track of its duty cycle, so if the deadzone is changed the check will be performed again, and may either start a motor that was previously stopped or visa versa.


### Frequency Control

Motors can be driven at a variety of frequencies, with common values being above the range of human hearing. As such this library uses 25KHz as its default, but this can easily be changed.

The frequency (in Hz) of a motor can be set by calling `.frequency(freq)`, which takes a float as its `freq` input. The supported range of this input is `10` Hz to `400` KHz, though not all motor drivers can handle the very high frequencies.

To read back the current frequency (in Hz) of the motor, call `.frequency()` without any input.

Note that changing the frequency does not change the duty cycle or speed sent to the motors, only how frequently pulses are sent.

### Configuration

### Direction

The driving direction of a motor can be changed either by providing `direction=REVERSED_DIR` when creating the `Motor` object, or by calling `.direction(REVERSED_DIR)` at any time in code. The `REVERSED_DIR` constant comes from the `pimoroni` module. There is also a `NORMAL_DIR` constant, though this is the default.


### Decay Mode

If you have ever had a motor directly connected to a power source and turned the power off, or disconnected the wire, you may have noticed that the motor continues to spin for a second or two before it reaches a stop. This is because the magnetic field the power source was generating has decayed away quickly, so the only thing slowing the motor down is friction. This results in the motor coasting to a stop.

By contrast, if you were to wire your circuit up such that instead of disconnecting the power, the off position joined the two ends of the motor together, it would take longer for the magnetic field to decay away. This has the effect of braking the motor, causing it to stop quicker than with friction alone.

These examples describe the two decay modes supported by the `Motor` class, `FAST_DECAY`, and `SLOW_DECAY`, respectively. Generally slow decay offers better motor performance, particularly with low speeds, so this is the default when creating a new `Motor`.

If fast decay is wanted then it can either be changed by providing `mode=FAST_DECAY` during the class creation or by calling `.decay_mode(FAST_DECAY)`. The current decay mode can also be read with `.decay_mode()`.

For more information about motor decay modes, it's highly recommended that you check out the Adafruit Learn Guide titled [Improve Brushed DC Motor Performance](https://learn.adafruit.com/improve-brushed-dc-motor-performance)


### Driver Type

There are two common types of DC motor drivers, based on the signals they expect for controlling the motor.
* Dual PWMs, where both pins control the speed, direction, and decay mode of the motor.
* Phase/Enable, where a single PWM pin controls speed and a single GPIO pin controls direction. There is no decay mode control.

By default all `Motor` objects are initialised for dual PWM drivers, but if you are using the other type of driver this can be configured by providing `ph_en_driver=True` during object creation.


### Function Reference

Here is the complete list of functions available on the `Motor` class:
```python
Motor(pins, direction=NORMAL_DIR, speed_scale=1.0, zeropoint=0.0, deadzone=0.05, freq=25000, mode=SLOW_DECAY, ph_en_driver=False)
pins()
enable()
disable()
is_enabled()
duty()
duty(duty)
speed()
speed(speed)
frequency()
frequency(freq)
stop()
coast()
brake()
full_negative()
full_positive()
to_percent(in)
to_percent(in, in_min, in_max)
to_percent(in, in_min, in_max, speed_min, speed_max)
direction()
direction(direction)
speed_scale()
speed_scale(speed_scale)
zeropoint()
zeropoint(zeropoint)
deadzone()
deadzone(deadzone)
decay_mode()
decay_mode(mode)
```

### Constants Reference

Here is the complete list of constants on the `motor` module:

* `FAST_DECAY` = `0`
* `SLOW_DECAY` = `1`

Here are useful constants from the `pimoroni` module:

* `NORMAL_DIR` = `0`
* `REVERSED_DIR` = `1`


### PWM Limitations

Although the RP2040 is capable of outputting up to 16 PWM signals, there are limitations of which pins can be used together:
* The PWMs output from pins 0 to 15 are repeated for pins 16 to 29, meaning that those pins share the same signals if PWM is enabled on both. For example if you used pin 3 for PWM and then tried to use pin 19, they would both output the same signal and it would not be possible to control them independently.
* The 16 PWM channels are grouped into 8 PWM slices, each containing A and B sub channels that are synchronised with each other. This means that parameters such as frequency are shared, which can cause issues if you want one motor to operate at a different frequency to it's pin neighbour or to drive an LED with PWM at a high frequency.

The official [RP2040 datasheet](https://datasheets.raspberrypi.com/rp2040/rp2040-datasheet.pdf), includes the handy _Table 525_ that details the pwm channel for each GPIO pin. This is shown below for convenience:

| GPIO        | 0  | 1  | 2  | 3  | 4  | 5  | 6  | 7  | 8  | 9  | 10 | 11 | 12 | 13 | 14 | 15 |
|-------------|----|----|----|----|----|----|----|----|----|----|----|----|----|----|----|----|
| PWM Channel | 0A | 0B | 1A | 1B | 2A | 2B | 3A | 3B | 4A | 4B | 5A | 5B | 6A | 6B | 7A | 7B |

| GPIO        | 16 | 17 | 18 | 19 | 20 | 21 | 22 | 23 | 24 | 25 | 26 | 27 | 28 | 29 |
|-------------|----|----|----|----|----|----|----|----|----|----|----|----|----|----|
| PWM Channel | 0A | 0B | 1A | 1B | 2A | 2B | 3A | 3B | 4A | 4B | 5A | 5B | 6A | 6B |


## MotorCluster

### Getting Started

An alternative way of controlling motors with your Motor 2040 is to use the `MotorCluster` class. This can be imported as follows:

```python
from motor import MotorCluster, motor2040
```
(If you are using another RP2040 based board, then `motor2040` can be omitted from the above line)

The next step is to choose which GPIO pins the cluster will be connected to and store them in a `list`. For example, using the handy constants of the `motor2040`, the below line creates the list `[ (4, 5), (6, 7), (8, 9), (10, 11) ]`
```python
pins = [ motor2040.MOTOR_A, motor2040.MOTOR_B, motor2040.MOTOR_C, motor2040.MOTOR_D ]
```

To create your motor cluster, specify the PIO, PIO state-machine and GPIO pins you chose a moment ago, and pass those into `MotorCluster`.

```python
cluster = MotorCluster(0, 0, pins)
```

You now have a `MotorCluster` class called `cluster` that will control the physical motors connected to `MOTOR_A`, `MOTOR_B`, `MOTOR_C`, and `MOTOR_D`. To start using these motors, simply enable them using:
```python
cluster.enable_all()
```
or
```python
cluster.enable(motor)
```
where `motor` is the motor's number within the cluster from `0` to `cluster.count() - 1`.

These functions activate the motors and drives them at their last known speed. Since this is the first time enabling the motors, there are no last known speeds, so instead they will be set to zero instead.

Once you have finished with the motors, they can be disabled by calling:
```python
cluster.disable_all()
```
or
```python
cluster.disable(motor)
```
where `motor` is the motor's number within the cluster from `0` to `cluster.count() - 1`.

From here the motors can be controlled in several ways. These are covered in more detail in the following sections.


### Control by Speed

The most intuitive way of controlling a motor is by speed. Speed can be any number that has a real-world meaning for that type of motor, for example revolutions per minute, or the linear or angular speed of the mechanism it is driving. By default the speed is a value ranging from `-1.0` to `1.0` but this can be changed by setting a new `speed_scale`. See [Configuration](#configuration-1) for more details.

The speed of a motor on a cluster can be set calling `.speed(motor, speed)` or `.all_at_speed(speed)`, which take a float as their `speed` input. If a motor is disabled, these will enable it. The resulting duty cycle will also be stored.

To read back the current speed of a motor on the cluster, call `.speed(motor)`. If the motor is disabled, this will be the last speed that was provided when enabled.


#### Common Speeds

To simplify certain motion patterns, motors on a cluster can be commanded to three common speeds. These are, full negative, full positive, and stopped. These are performed by calling `.full_negative(motor)`, `.full_positive(motor)`, and `.stop(servo)`, respectively. If the motor is disabled, these will enable it. There are also `.all_full_negative()`, `.all_full_positive()`, and `.stop_all()` for having all the motors on the cluster drive at once.

The full negative and full positive speed of each motor on a cluster can be read back using `.speed_scale(motor)`. This can be useful as an input to equations that provide numbers directly to `.speed(motor, speed)`, for example.


### Control by Percent

Sometimes there are projects where motors need to move based on the readings from sensors or another devices, but the numbers given out are not easy to convert to speeds the motors accept. To overcome this the library lets you drive the motors on a cluster at a percent between their negative and positive speeds, or two speeds provided, based on that input.

With an input between `-1.0` and `1.0`, a motor on a cluster can be driven at a percent between its negative and positive speeds using `.at_percent(motor, in)`, or all motors using `.all_at_percent(in)`.

With an input between a provided min and max, a motor on a cluster can be driven at a percent between its negative and postive speeds using `.at_percent(motor, in, in_min, in_max)`, or all motors using `.all_at_percent(in, in_min, in_max)`.

With an input between a provided min and max, a motor on a cluster can be driven at a percent between two provided speeds using `.at_percent(motor, in, in_min, speed_min, speed_max)`, or all motors using `.all_at_percent(in, in_min, speed_min, speed_max)`.

If the motor is disabled, these will enable it.


### Control by Duty Cycle

At a hardware level DC motors operate by receiving a voltage across their two terminals, with positive causing a motion in one direction and negative causing a motion in the other. To avoid needing a negative voltage supply, motor drivers employ a H-Bridge arrangement of transistors or mosfets to flip which side of the motor is connected to ground and which is connected to power. By rapidly turing these transistors or mosfets on and off both the speed and direction of the motor can be varied. The common way this is achieved is by using a pair of pulse width modulated signals, where the duty cycle of the active signal controls the speed, and which signal is active controls the direction. Braking can also be controlled (see //TODO)

The duty cycle of a motor on a cluster can be set by calling `.duty(motor, duty)` or `.all_at_duty(duty)`, which take a float as their `duty` input. If a motor is disabled, these will enable it. These functions will also recalculate the related speed.

To read back the current duty cycle of a motor on a cluster, call `.duty(motor)`. If the motor is disabled, this will be the last duty that was provided when enabled.


### Frequency Control

Motors can be driven at a variety of frequencies, with a common values being above the range of human hearing. As such this library uses 25KHz as its default, but this can easily be changed.

The frequency (in Hz) of all the motors on a cluster can be set by calling `.frequency(freq)`, which takes a float as its `freq` input. //TODO The supported range of this input is `10` to `350` Hz. Due to how `MotorCluster` works, there is no way to set independent frequencies for each motor.

To read back the current frequency (in Hz) of all the motors on a cluster, call `.frequency()` without any input.

Note that changing the frequency does not change the duty cycle sent to the motors, only how frequently pulses are sent.

Also, be aware that currently the frequency changes immediately, even if part-way through outputting a pulse. It is therefore recommended to disable all motors first before changing the frequency.


### Phase Control

When dealing with many servos, there can often be large current draw spikes caused by them all responding to pulses at the same time. To minimise this, the ServoCluster class allows for the start time of each servo's pulses to be delayed by a percentage of the available time period. This is called their phase.

The phase of a servo on a cluster can be set by calling `.phase(servo, phase)` or `.all_to_phase(phase)`, which take a float between `0.0` and `1.0` as their `phase` input.

To read back the current phase of a servo on a cluster, call `.phase(servo)`.

By default all servos on a cluster will start with different phases unless `auto_phase=False` is provided when creating the `ServoCluster`.


### Calibration

There are different types of servos, with `ANGULAR`, `LINEAR`, and `CONTINUOUS` being common. To support these different types, each `ServoCluster` class contains calibration objects for each of its servos that store the specific value to pulse mappings needed for their types. A copy of a servo's calibration on a cluster can be accessed using `.calibration(servo)`. It is also possible to provide a servo on a cluster with a new calibration using `.calibration(servo, calibration)`.


### Delayed Loading

To match behaviour with the regular `Motor` class, `MotorCluster` automatically applies each change to its motor's states immediately. However, sometimes this may not be wanted, and instead you want all motors to receive updated duty cycles at the same time, regardless of how long the code ran that calculated the update.

For this purpose, all the functions that modify a motor state on a cluster include an optional parameter `load`, which by default is `True`. To avoid this "loading" include `load=False` in the relevant function calls. Then either the last call can include `load=True`, or a specific call to `.load()` can be made.


### Function Reference

Here is the complete list of functions available on the `MotorCluster` class:

```python
//TODO
MotorCluster(pio, sm, pins, calibration=ANGULAR, freq=50, auto_phase=True)    # calibration can either be an integer or a Calibration class
count()
pins(motor)
enable(motor, load=True)
enable_all(load=True)
disable(motor, load=True)
disable_all(load=True)
is_enabled(motor)
duty(motor)
duty(motor, duty, load=True)
all_at_duty(motor, load=True)
speed(motor)
speed(motor, speed, load=True)
all_at_speed(speed, load=True)
phase(motor)
phase(motor, phase, load=True)
all_to_phase(phase, load=True)
frequency()
frequency(freq)
full_negative(motor, load=True)
all_full_negative(load=True)
full_positive(motor, load=True)
all_full_positive(load=True)
stop(motor, load=True)
stop_all(load=True)
coast(motor, load=True)
coast_all(load=True)
brake(motor, load=True)
brake_all(load=True)
at_percent(motor, in, load=True)
at_percent(motor, in, in_min, in_max, load=True)
at_percent(motor, in, in_min, in_max, value_min, value_max, load=True)
all_at_percent(in, load=True)
all_at_percent(in, in_min, in_max, load=True)
all_at_percent(in, in_min, in_max, value_min, value_max, load=True)
load()
```


### PIO Limitations

The RP2040 features two PIOs with four state machines each. This places a hard limit on how many MotorClusters can be created. As this class is capable of driving all 30 GPIO pins, the only time this limit will be of concern is when motors with different frequencies are wanted, as all the outputs a MotorCluster controls share the same frequency. Relating this to the hardware PWM, think of it as a single PWM slice with up to 30 sub channels, A, B, C, D etc.

When creating a MotorCluster, in most cases you'll use `0` for PIO and `0` for PIO state-machine. You should change these though if you plan on running multiple clusters, or using a cluster alongside something else that uses PIO, such as our [Plasma library](https://github.com/pimoroni/pimoroni-pico/tree/main/micropython/modules/plasma).


## Configuration

After using servos for a while, you may notice that some don't quite go to the positions you would expect. Or perhaps you are giving values to a continuous rotation servo in degrees when it would make more sense to use a speed or rpm. To compensate for these cases, each `Servo` class or servo within a `ServoCluster` has an individual `Calibration` class. This class contains a set of pulse-value pairs that are used by the `.value(value)` functions (and those similar) to convert real-world numbers into pulses each servo understand.

### Common Types

There are three common `type`s of servo's supported:
* `ANGULAR` = `0` - A servo with a value that ranges from -90 to +90 degrees.
* `LINEAR` = `1` - A servo with a value that ranges from 0 to +1.0.
* `CONTINUOUS` = `2` - A servo with a value that ranges from -1.0 to +1.0.

By default all `Servo` classes or servo within a `ServoCluster` are `ANGULAR`. This can be changed by providing one of the other types as a parameter during their creation, as shown below:
```python
angular = Servo(servo2040.SERVO_1)  # ANGULAR is the default so does not need to be specified here
linear = Servo(servo2040.SERVO_2, LINEAR)
continuous = Servo(servo2040.SERVO_3, CONTINUOUS)
```


### Custom Calibration

As well as the common types, a custom calibration can also be provided to one or more servos during creation. Below is an example that creates an angular servo that can only travel from -45 degrees to 45 degrees.

```python
cal = Calibration()
cal.apply_two_pairs(1000, 2000, -45, 45)
s = Servo(servo2040.SERVO_1, cal)
```

This could be useful for example if the servo turning beyond those values would cause damage to whatever mechanism it is driving, since it would not be possible to go to angles beyond these unless limits were disabled (see [Limits](#limits)). Also it lets the exact pulse widths matching the angles be set (the `1000` and `2000` in the example). Discovering these values can take some trial and error, and will offen be different for each servo.



# Modifying a Calibration

It is also possible to access and modify the calibration of a `Servo` or a servo on a `ServoCluster` after their creation. This is done by first getting a copy of the servo's calibration using `.calibration()` or `.calibration(servo)`, modifying its pulses or values, then applying the modified calibration back onto to the servo.

Below, an angular servo is modified to increase its reported rotation range from 180 degrees to 270 degrees.
```python
wide_angle = Servo(servo2040.SERVO_1)
cal = wide_angle.calibration()
cal.first_value(-135)
cal.last_value(+135)
wide_angle.calibration(cal)
```


### Movement Limits

As well as providing a mapping between pulses and values, the calibration class also limits a servo from going beyond its minimum and maximum values. In some cases this may not be wanted, so the state of the limits can be modified by calling `.limit_to_calibration(lower, upper)` where `lower` and `upper` are booleans. Additionally, the current state of these limits can be queried by calling `.has_lower_limit()` and `.has_upper_limit()`, respectively.

A case where you may want to disable limits is if you want a servo to go to a value (e.g. 90 degrees), but are not physically able to get a pulse measurement for that but can do another value instead (e.g. 60 degrees).

Note, even with limits disables, servos still have hard limits of `400` and `2600` microsecond pulse widths. These are intended to protect servos from receiving pulses that are too far beyond their expected range. These can vary from servo to servo though, with some hitting a physical end-stop before getting to the typical `500` and `2500` associated with -90 and +90 degrees.


### Populating a Calibration

To aid in populating a `Calibration` class, there are five helper functions that fill the class with pulse-value pairs:
* `apply_blank_pairs(size)` - Fills the calibration with the specified number of zero pairs
* `apply_two_pairs(min_pulse, max_pulse, min_value, max_value)` - Fills the calibration with two pairs using the min and max numbers provided
* `apply_three_pairs(min_pulse, mid_pulse, max_pulse, min_value, mid_value, max_value)` - Fills the calibration with three pairs using the min, mid and max numbers provided
* `apply_uniform_pairs(size, min_pulse, max_pulse, min_value, max_value)` - Fills the calibration with the specified number of pairs, interpolated from the min to max numbers provided
* `apply_default_pairs(type)` - Fills the calibration with the pairs of one of the common types

Once a `Calibration` class contains pairs (as checked `.size() > 0`), these can then be accessed by calling `.pair(index)` and can then be modified by calling `.pair(index, pair)`. The former function returns a list containing the pulse and value of the pair, and the latter accepts a list or tuple containing the pulse and value. For situations when only a single element of each pair is needed, `.pulse(index)` and `.value(index)` will return the current numbers, and `.pulse(index, pulse)` and `.value(index, value)` will override them.

For further convenience there are functions for accessing and modifying the `.first()` and `.last()` pair/pulse/value of the calibration.


### Viewing the Mapping

To aid in visualising a calibration's pulse-value mapping, the pulse for any given value can be queried by calling `.value_to_pulse(value)`. Similarly, the value for any given pulse can be queried by calling `.pulse_to_value(pulse)`. These are the same functions used by `Servo` and `ServoCluster` when controlling their servos.


### Function Reference

Here is the complete list of functions available on the `Calibration` class:
```python
Calibration()
Calibration(type)
apply_blank_pairs(size)
apply_two_pairs(min_pulse, max_pulse, min_value, max_value)
apply_three_pairs(min_pulse, mid_pulse, max_pulse, min_value, mid_value, max_value)
apply_uniform_pairs(size, min_pulse, max_pulse, min_value, max_value)
apply_default_pairs(type)
size()
pair(index)
pair(index, pair)
pulse(index)
pulse(index, pulse)
value(index)
value(index, value)
first()
first(pair)
first_pulse()
first_pulse(pulse)
first_value()
first_value(value)
last()
last(pair)
last_pulse()
last_pulse(pulse)
last_value()
last_value(value)
has_lower_limit()
has_upper_limit()
limit_to_calibration(lower, upper)
value_to_pulse(value)
pulse_to_value(pulse)
```
