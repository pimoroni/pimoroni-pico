# Servos and Servo 2040 <!-- omit in toc -->

The Servo library lets you drive 3-pin hobby servo motors from a Raspberry Pi Pico or any other RP2040-based board, such as the [Pimoroni Servo 2040](https://pimoroni.com/servo2040).

This library offers two servo implementations:
* a `Servo` class that uses hardware PWM to drive a single servo, with support for up to 16 servos.
* a `ServoCluster` class that uses Programmable IO (PIO) hardware to drive up to 30 servos.

There is also a `Calibration` class for performing advanced tweaking of each servo's movement behaviour.


## Table of Content
- [Servo 2040](#servo-2040)
  - [Reading the User Button](#reading-the-user-button)
  - [Reading the Sensors](#reading-the-sensors)
    - [Configuring Pulls](#configuring-pulls)
  - [Controlling the LED Bar](#controlling-the-led-bar)
  - [Pin Constants](#pin-constants)
    - [Servo Pins](#servo-pins)
    - [LED Pin](#led-pin)
    - [I2C Pins](#i2c-pins)
    - [Button Pin](#button-pin)
    - [Address Pins](#address-pins)
    - [ADC Pins](#adc-pins)
  - [Other Constants](#other-constants)
    - [Counts](#counts)
    - [Addresses](#addresses)
    - [Sensing](#sensing)
- [Servo](#servo)
  - [Getting Started](#getting-started)
  - [Control by Value](#control-by-value)
    - [Common Values](#common-values)
  - [Control by Percent](#control-by-percent)
  - [Control by Pulse Width](#control-by-pulse-width)
  - [Frequency Control](#frequency-control)
  - [Calibration](#calibration)
  - [Function Reference](#function-reference)
  - [PWM Limitations](#pwm-limitations)
- [ServoCluster](#servocluster)
  - [Getting Started](#getting-started-1)
  - [Control by Value](#control-by-value-1)
    - [Common Values](#common-values-1)
  - [Control by Percent](#control-by-percent-1)
  - [Control by Pulse Width](#control-by-pulse-width-1)
  - [Frequency Control](#frequency-control-1)
  - [Phase Control](#phase-control)
  - [Calibration](#calibration-1)
  - [Delayed Loading](#delayed-loading)
  - [Function Reference](#function-reference-1)
  - [PIO Limitations](#pio-limitations)
- [Calibration](#calibration-2)
  - [Common Types](#common-types)
  - [Custom Calibration](#custom-calibration)
  - [Modifying a Calibration](#modifying-a-calibration)
  - [Movement Limits](#movement-limits)
  - [Populating a Calibration](#populating-a-calibration)
  - [Viewing the Mapping](#viewing-the-mapping)
  - [Function Reference](#function-reference-2)


## Servo 2040

Servo 2040 is a **standalone servo controller** for making things with lots of moving parts. It has pre-soldered pin headers for plugging in up to 18 servos, with additional bells and whistles like sensor headers, current monitoring, RGB LEDs, and a user button that make it ideal for many robotics projects!

For more information on this board, check out the store page: [pimoroni.com/servo2040](https://pimoroni.com/servo2040).


### Reading the User Button

As is the case for many of Pimoroni's RP2040-based boards, the boot button on Servo 2040 that is used for programming also acts as a user button once a program is running. To simplify the use of this and other buttons, the `pimoroni` module contains a `Button` class that handles button debounce and auto-repeat.

```python
Button(button, invert=True, repeat_time=200, hold_time=1000)
```

To set up the user button, first import the `Button` class from the `pimoroni` module and the pin constant for the button from `servo`:
```python
from pimoroni import Button
from servo import servo2040
```

Then create an instance of `Button` for the user button:
```python
user_sw = Button(servo2040.USER_SW)
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

On the right-hand edge of Servo 2040 are six analog inputs, with neighbouring 3.3V and GND. These let you connect up sensors to enable your mechanical creations to sense how they are interacting with the world. For example, a servo driven gripper with analog force sensors in its fingers could report how much pressure it is applying as it grabs an object, or the extra pin of an analog feedback servo could be wired up to report its actual angle.

Servo 2040 also has two internal sensors:
* A voltage sensor, letting you measure the supply voltage to the servos.
* A low-side current sensor, letting you measure how much current a set of servos is drawing.
Both of these could be used just for monitoring, or as the trigger to turn off servos safely when voltage gets too low or current draw gets too high.

To allow for all of these inputs, Servo 2040 has an onboard analog multiplexer that expands a single analog pin into eight, by use of three digital address pins that select a single input at a time. As such, the setup for these sensors is more involved than it would be to just read eight analog pins directly.

To begin reading these inputs, first import the `Analog` and `AnalogMux` classes from `pimoroni` and the pin, address, and gain constants from `servo`:

```python
from pimoroni import Analog
from servo import servo2040
```

Then set up three instances of `Analog` for the sensor, voltage, and current sensing:

```python
sen_adc = Analog(servo2040.SHARED_ADC)
vol_adc = Analog(servo2040.SHARED_ADC, servo2040.VOLTAGE_GAIN)
cur_adc = Analog(servo2040.SHARED_ADC, servo2040.CURRENT_GAIN,
                 servo2040.SHUNT_RESISTOR, servo2040.CURRENT_OFFSET)
```

You may notice, all three of these use the same `SHARED_ADC` pin. This is intentional as it is just a single pin that is being used for all three different functions, only the gains differ.

The next step is to set up the analog multiplexer, by providing it with the three address pins:
```python
mux = AnalogMux(servo2040.ADC_ADDR_0, servo2040.ADC_ADDR_1, servo2040.ADC_ADDR_2)
```
Note that the `AnalogMux` does not know about any of the `Analog` classes that were created before.

With the multiplexer now configured, reading each sensor is simply a case of 'selecting' the sensor on the multiplexer then reading the value from one of the three `Analog` classes created at the start.

To read the siz sensor headers:
```python
for addr in range(servo2040.NUM_SENSORS):
    mux.select(addr)
    print("Sensor", addr + 1, "=", sen_adc.read_voltage())
```

To read the voltage sense:
```python
mux.select(servo2040.VOLTAGE_SENSE_ADDR)
print("Voltage =", vol_adc.read_voltage(), "V")
```

To read the current draw in amps (A):
```python
mux.select(servo2040.CURRENT_SENSE_ADDR)
print("Current =", cur_adc.read_current(), "A")
```


#### Configuring Pulls

For some sensors you may need to have the input be pulled high or low before taking a reading. To support this there is an optional `muxed_pin` parameter that can be passed into the `AnalogMux` when creating it, which gives the multiplexer access to the pin to control the pulls.

```python
mux = AnalogMux(servo2040.ADC_ADDR_0, servo2040.ADC_ADDR_1, servo2040.ADC_ADDR_2,
                muxed_pin=Pin(servo2040.SHARED_ADC))
```

From there the pull state of each of the multiplexer's addresses can be configured independently by calling `.configure_pull()`, with the address and the pull state (either `Pin.PULL_UP`, `Pin.PULL_DOWN`, or `None`).

The below example shows how to set all 6 sensor addresses to have pull-downs:
```python
sensor_addrs = list(range(servo2040.SENSOR_1_ADDR, servo2040.SENSOR_6_ADDR + 1))
for addr in sensor_addrs:
    mux.configure_pull(addr, Pin.PULL_DOWN)
```


### Controlling the LED Bar

Alongside Servo 2040's six sensor headers are six addressable RGB LEDs. These work using the same chainable 1-wire signalling as WS2812 LED's, commonly known as Neopixels. As such, they can be controlled using the Plasma Library, as used by the [Pimoroni Plasma 2040](https://shop.pimoroni.com/products/plasma-2040).

To set up the LED bar, first import the `WS2812` class from the `plasma` module and the pin constants for the LEDs from `servo`:
```python
from plasma import WS2812
from servo import servo2040
```

Then construct a new `WS2812` instance, specifying the number of LEDs, PIO, PIO state-machine and GPIO pin.
```python
led_bar = WS2812(servo2040.NUM_LEDS, 1, 0, servo2040.LED_DATA)
```

Finally, start the LED bar by calling `start`:
```python
led_bar.start()
```

For more information on how to control the LEDs on the bar, please refer to the [Plasma Library](https://github.com/pimoroni/pimoroni-pico/tree/main/micropython/modules/plasma).


### Pin Constants

The `servo` module contains a `servo2040` sub module with constants for the servo, LED, sensor and button pins.


#### Servo Pins

* `SERVO_1` = `0`
* `SERVO_2` = `1`
* `SERVO_3` = `2`
* `SERVO_4` = `3`
* `SERVO_5` = `4`
* `SERVO_6` = `5`
* `SERVO_7` = `6`
* `SERVO_8` = `7`
* `SERVO_9` = `8`
* `SERVO_10` = `9`
* `SERVO_11` = `10`
* `SERVO_12` = `11`
* `SERVO_13` = `12`
* `SERVO_14` = `13`
* `SERVO_15` = `14`
* `SERVO_16` = `15`
* `SERVO_17` = `16`
* `SERVO_18` = `17`


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

The `servo2040` sub module also contains other constants to help with the control of Servo 2040's various features:


#### Counts

* `NUM_SERVOS` = `18`
* `NUM_SENSORS` = `6`
* `NUM_LEDS` = `6`


#### Addresses

* `SENSOR_1_ADDR` = `0b000`
* `SENSOR_2_ADDR` = `0b001`
* `SENSOR_3_ADDR` = `0b010`
* `SENSOR_4_ADDR` = `0b011`
* `SENSOR_5_ADDR` = `0b100`
* `SENSOR_6_ADDR` = `0b101`
* `VOLTAGE_SENSE_ADDR` = `0b110`
* `CURRENT_SENSE_ADDR` = `0b111`


#### Sensing

* `VOLTAGE_GAIN` = `0.28058`
* `SHUNT_RESISTOR` = `0.003`
* `CURRENT_GAIN` = `69`
* `CURRENT_OFFSET` = `-0.02`


## Servo

### Getting Started

To start using servos with your Servo 2040, you will need to first import the `Servo` class.
```python
from servo import Servo, servo2040
```
If you are using another RP2040 based board, then `servo2040` can be omitted from the above line.

To create your servo, choose which GPIO pin it will be connected to, and pass that into `Servo`. For this example we will use one of the handy constants of the `servo2040`.
```python
s = Servo(servo2040.SERVO_1)
```

You now have a `Servo` class called `s` that will control the physical servo connected to `SERVO_1`. To start using this servo, simply enable it using:
```python
s.enable()
```

This activates the servo and moves it to it's last known position. Since this is the first time enabling the servo, there is no last known position, so instead it will move to the middle of its movement range instead.

Once you have finished with the servo, it can be disabled by calling:
```python
s.disable()
```

From here the servo can be controlled in several ways. These are covered in more detail in the following sections.


### Control by Value

The most intuitive way of controlling a servo is by value. Value can be any number that has a real-world meaning for that type of servo, for example its angle in degrees if it's a regular angular servo, or its speed as a percentage if it's a continuous rotation servo. See [Calibration](#calibration) for more details.

The value of a servo can be set by calling `.value(value)`, which takes a float as its `value` input. If the servo is disabled, this will enable it. The resulting pulse width will also be stored.

To read back the current value of the servo, call `.value()` without any input. If the servo is disabled, this will be the last value that was provided when enabled.


#### Common Values

To simplify certain motion patterns, a servo can be commanded to three common values. These are, their minimum, middle, and maximum. These are performed by calling `.to_min()`, `.to_mid()`, and `.to_max()`, respectively. If the servo is disabled, these will enable it.

It is also possible to read back the values each of those three commands is using internally, using `.min_value()`, `.mid_value()`, and `.max_value()`. These can be useful as inputs to equations that provide numbers directly to `.value(value)`, for example.


### Control by Percent

Sometimes there are projects where a servo needs to move based on the reading from a sensor or another device, but the numbers given out are not easy to convert to values the servo accepts. To overcome this the library lets you move the servo to a percent between its minimum and maximum values, or two values provided, based on that input.

With an input between `-1.0` and `1.0`, a servo can be moved to a percent between its minimum and maximum values using `.to_percent(in)`.

With an input between a provided min and max, a servo can be moved to a percent between its minimum and maximum values using `.to_percent(in, in_min, in_max)`.

With an input between a provided min and max, a servo can be moved to a percent between two provided values using `.to_percent(in, in_min, value_min, value_max)`.

If the servo is disabled, these will enable it.


### Control by Pulse Width

At a hardware level servos operate by receiving a digital signal with specific pulse widths. Typically pulses are between 500 microseconds and 2500 microseconds in length, and are usually repeated every 20 milliseconds (50Hz). These functions let you interact with pulse widths directly.

The pulse width (in microseconds) of a servo can be set by calling `.pulse(pulse)`, which takes a float as its `pulse` input. If the servo is disabled this will enable it, except for the case of `0` where instead the servo will be disabled. This function will also recalculate the related value.

To read back the current pulse width (in microseconds) of the servo, call `.pulse()` without any input. If the servo is disabled, this will be the last pulse that was provided when enabled.


### Frequency Control

The vast majority of servos expect to receive pulses with a frequency of 50Hz, so this library uses that as its default. However, there may be cases where this value needs to be changed, such as when using servos that operate up to frequencies of 333Hz.

The frequency (in Hz) of a servo can be set by calling `.frequency(freq)`, which takes a float as its `freq` input. The supported range of this input is `10` to `350` Hz.

To read back the current frequency (in Hz) of the servo, call `.frequency()` without any input.

Note that changing the frequency does not change the pulse widths sent to the servos, only how frequently they are sent. This is why `350` is the upper limit, as any higher and the maximum pulse would be longer than the time period. If you encounter any servos where this behaviour is not what they expect, please raise it as a Github issue.


### Calibration

There are different types of servos, with `ANGULAR`, `LINEAR`, and `CONTINUOUS` being common. To support these different types, each `Servo` class contains a calibration object that stores the specific value to pulse mapping needed for its type. A copy of a servo's calibration can be accessed using `.calibration()`. It is also possible to provide a servo with a new calibration using `.calibration(calibration)`.


### Function Reference

Here is the complete list of functions available on the `Servo` class:
```python
Servo(pin, calibration=ANGULAR, freq=50)        # calibration can either be an integer or a Calibration class
pin()
enable()
disable()
is_enabled()
pulse()
pulse(pulse)
value()
value(value)
frequency()
frequency(freq)
min_value()
mid_value()
max_value()
to_min()
to_mid()
to_max()
to_percent(in)
to_percent(in, in_min, in_max)
to_percent(in, in_min, in_max, value_min, value_max)
calibration()
calibration(calibration)
```


### PWM Limitations

Although the RP2040 is capable of outputting up to 16 PWM signals, there are limitations of which pins can be used together:
* The PWMs output from pins 0 to 15 are repeated for pins 16 to 29, meaning that those pins share the same signals if PWM is enabled on both. For example if you used pin 3 for PWM and then tried to use pin 19, they would both output the same signal and it would not be possible to control them independently.
* The 16 PWM channels are grouped into 8 PWM slices, each containing A and B sub channels that are synchronised with each other. This means that parameters such as frequency are shared, which can cause issues if you want one servo to operate at a different frequency to it's pin neighbour or to drive an LED with PWM at a high frequency.

The official [RP2040 datasheet](https://datasheets.raspberrypi.com/rp2040/rp2040-datasheet.pdf), includes the handy _Table 525_ that details the pwm channel for each GPIO pin. This is shown below for convenience:

| GPIO        | 0  | 1  | 2  | 3  | 4  | 5  | 6  | 7  | 8  | 9  | 10 | 11 | 12 | 13 | 14 | 15 |
|-------------|----|----|----|----|----|----|----|----|----|----|----|----|----|----|----|----|
| PWM Channel | 0A | 0B | 1A | 1B | 2A | 2B | 3A | 3B | 4A | 4B | 5A | 5B | 6A | 6B | 7A | 7B |

| GPIO        | 16 | 17 | 18 | 19 | 20 | 21 | 22 | 23 | 24 | 25 | 26 | 27 | 28 | 29 |
|-------------|----|----|----|----|----|----|----|----|----|----|----|----|----|----|
| PWM Channel | 0A | 0B | 1A | 1B | 2A | 2B | 3A | 3B | 4A | 4B | 5A | 5B | 6A | 6B |


## ServoCluster

### Getting Started

An alternative way of controlling servos with your Servo 2040 is to use the `ServoCluster` class. This can be imported as follows:

```python
from servo import ServoCluster, servo2040
```
(If you are using another RP2040 based board, then `servo2040` can be omitted from the above line)

The next step is to choose which GPIO pins the cluster will be connected to and store them in a `list`. For a consecutive set of pins the easiest way to do this is to use a `range` to create each pin number, and convert that to a `list`. For example, using the handy constants of the `servo2040`, the below line creates the list `[0, 1, 2, 3]`
```python
pins = list(range(servo2040.SERVO_1, servo2040.SERVO_4 + 1))
```

To create your servo cluster, specify the PIO, PIO state-machine and GPIO pins you chose a moment ago, and pass those into `ServoCluster`.

```python
cluster = ServoCluster(0, 0, pins)
```

You now have a `ServoCluster` class called `cluster` that will control the physical servos connected to `SERVO_1`, `SERVO_2`, `SERVO_3`, and `SERVO_4`. To start using these servos, simply enable them using:
```python
cluster.enable_all()
```
or
```python
cluster.enable(servo)
```
where `servo` is the servo's number within the cluster from `0` to `cluster.count() - 1`.

These functions activate the servos and move them to their last known positions. Since this is the first time enabling the servos, there are no last known positions, so instead they will move to the middle of their movement ranges instead.

Once you have finished with the servos, they can be disabled by calling:
```python
cluster.disable_all()
```
or
```python
cluster.disable(servo)
```
where `servo` is the servo's number within the cluster from `0` to `cluster.count() - 1`.

From here the servos can be controlled in several ways. These are covered in more detail in the following sections.


### Control by Value

The most intuitive way of controlling the servos is by their value. Value can be any number that has a real-world meaning for that type of servo, for example its angle in degrees if it's a regular angular servo, or its speed as a percentage if it's a continuous rotation servo. See [Calibration](#calibration) for more details.

The value of a servo on a cluster can be set calling `.value(servo, value)` or `.all_to_value(value)`, which take a float as their `value` input. If a servo is disabled, these will enable it. The resulting pulse width will also be stored.

To read back the current value of a servo on the cluster, call `.value(servo)`. If the servo is disabled, this will be the last value that was provided when enabled.


#### Common Values

To simplify certain motion patterns, servos on a cluster can be commanded to three common values. These are, their minimum, middle, and maximum. These are performed by calling `.to_min(servo)`, `.to_mid(servo)`, and `.to_max(servo)`, respectively. If the servo is disabled, these will enable it. There are also `.all_to_min()`, `.all_to_mid()`, and `.all_to_max()` for having all the servos on the cluster move at once.

It is also possible to read back the values each of those three commands is using internally, using `.min_value(servo)`, `.mid_value(servo)`, and `.max_value(servo)`. These can be useful as inputs to equations that provide numbers directly to `.value(servo, value)`, for example.


### Control by Percent

Sometimes there are projects where servos need to move based on the readings from sensors or another devices, but the numbers given out are not easy to convert to values the servos accept. To overcome this the library lets you move the servos on a cluster to a percent between their minimum and maximum values, or two values provided, based on that input.

With an input between `-1.0` and `1.0`, a servo on a cluster can be moved to a percent between its minimum and maximum values using `.to_percent(servo, in)`, or all servos using `.all_to_percent(in)`.

With an input between a provided min and max, a servo on a cluster can be moved to a percent between its minimum and maximum values using `.to_percent(servo, in, in_min, in_max)`, or all servos using `s.all_to_percent(in, in_min, in_max)`.

With an input between a provided min and max, a servo on a cluster can be moved to a percent between two provided values using `.to_percent(servo, in, in_min, value_min, value_max)`, or all servos using `.all_to_percent(in, in_min, value_min, value_max)`.

If the servo is disabled, these will enable it.


### Control by Pulse Width

At a hardware level servos operate by receiving a digital signal with specific pulse widths. Typically pulses are between 500 microseconds and 2500 microseconds in length, and are usually repeated every 20 milliseconds (50Hz). These functions let you interact with pulse widths directly.

The pulse width (in microseconds) of a servo on a cluster can be set by calling `.pulse(servo, pulse)` or `.all_to_pulse(pulse)`, which take a float as their `pulse` input. If a servo is disabled, these will enable it, except for the case of `0` where instead the servo will be disabled. These functions will also recalculate the related value.

To read back the current pulse width (in microseconds) of a servo on a cluster, call `.pulse(servo)`. If the servo is disabled, this will be the last pulse that was provided when enabled.


### Frequency Control

The vast majority of servos expect to receive pulses with a frequency of 50Hz, so this library uses that as its default. However, there may be cases where this value needs to be changed, such as when using servos that operate up to frequencies of 333Hz. Un

The frequency (in Hz) of all the servos on a cluster can be set by calling `.frequency(freq)`, which takes a float as its `freq` input. The supported range of this input is `10` to `350` Hz. Due to how `ServoCluster` works, there is no way to set independent frequencies for each servo.

To read back the current frequency (in Hz) all the servos on a cluster, call `.frequency()` without any input.

Note that changing the frequency does not change the pulse widths sent to the servos, only how frequently they are sent. This is why `350` is the upper limit, as any higher and the maximum pulse would be longer than the time period. If you encounter any servos where this behaviour is not what they expect, please raise it as a Github issue.

Also, be aware that currently the frequency changes immediately, even if part-way through outputting a pulse. It is therefore recommended to disable all servos first before changing the frequency.


### Phase Control

When dealing with many servos, there can often be large current draw spikes caused by them all responding to pulses at the same time. To minimise this, the ServoCluster class allows for the start time of each servo's pulses to be delayed by a percentage of the available time period. This is called their phase.

The phase of a servo on a cluster can be set by calling `.phase(servo, phase)` or `.all_to_phase(phase)`, which take a float between `0.0` and `1.0` as their `phase` input.

To read back the current phase of a servo on a cluster, call `.phase(servo)`.

By default all servos on a cluster will start with different phases unless `auto_phase=False` is provided when creating the `ServoCluster`.


### Calibration

There are different types of servos, with `ANGULAR`, `LINEAR`, and `CONTINUOUS` being common. To support these different types, each `ServoCluster` class contains calibration objects for each of its servos that store the specific value to pulse mappings needed for their types. A copy of a servo's calibration on a cluster can be accessed using `.calibration(servo)`. It is also possible to provide a servo on a cluster with a new calibration using `.calibration(servo, calibration)`.


### Delayed Loading

To match behaviour with the regular `Servo` class, `ServoCluster` automatically applies each change to its servo's states immediately. However, sometimes this may not be wanted, and instead you want all servos to receive updated pulses at the same time, regardless of how long the code ran that calculated the update.

For this purpose, all the functions that modify a servo state on a cluster include an optional parameter `load`, which by default is `True`. To avoid this "loading" include `load=False` in the relevant function calls. Then either the last call can include `load=True`, or a specific call to `.load()` can be made.


### Function Reference

Here is the complete list of functions available on the `ServoCluster` class:

```python
ServoCluster(pio, sm, pins, calibration=ANGULAR, freq=50, auto_phase=True)    # calibration can either be an integer or a Calibration class
count()
pin(servo)
enable(servo, load=True)
enable_all(load=True)
disable(servo, load=True)
disable_all(servo, load=True)
is_enabled(servo)
pulse(servo)
pulse(servo, pulse, load=True)
all_to_pulse(pulse, load=True)
value(servo)
value(servo, value, load=True)
all_to_value(value, load=True)
phase(servo)
phase(servo, phase, load=True)
all_to_phase(phase, load=True)
frequency()
frequency(freq)
min_value(servo)
mid_value(servo)
max_value(servo)
to_min(servo, load=True)
all_to_min(load=True)
to_mid(servo, load=True)
all_to_mid(load=True)
to_max(servo, load=True)
all_to_max(load=True)
to_percent(servo, in, load=True)
to_percent(servo, in, in_min, in_max, load=True)
to_percent(servo, in, in_min, in_max, value_min, value_max, load=True)
all_to_percent(in, load=True)
all_to_percent(in, in_min, in_max, load=True)
all_to_percent(in, in_min, in_max, value_min, value_max, load=True)
calibration(servo)
calibration(servo, calibration)
load()
```


### PIO Limitations

The RP2040 features two PIOs with four state machines each. This places a hard limit on how many ServoClusters can be created. As this class is capable of driving all 30 GPIO pins, the only time this limit will be of concern is when servos with different frequencies are wanted, as all the outputs a ServoCluster controls share the same frequency. Relating this to the hardware PWM, think of it as a single PWM slice with up to 30 sub channels, A, B, C, D etc.

When creating a ServoCluster, in most cases you'll use `0` for PIO and `0` for PIO state-machine. You should change these though if you plan on running multiple clusters, or using a cluster alongside something else that uses PIO, such as our [Plasma library](https://github.com/pimoroni/pimoroni-pico/tree/main/micropython/modules/plasma).


## Calibration

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
