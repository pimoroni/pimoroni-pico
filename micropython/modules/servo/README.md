# Servos and Servo 2040 <!-- omit in toc -->

The Servo library lets you drive 3-pin hobby servo motors from a Raspberry Pi Pico or any other RP2040-based board, such as the [Pimoroni Servo 2040](https://pimoroni.com/servo2040).

This library offers two servo implementations:
* a `Servo` class that uses hardware PWM to drive a single servo, with support for up to 16 servos.
* a `ServoCluster` class that uses Programmable IO (PIO) hardware to drive up to 30 servos.

There is also a `Calibration` class for performing advanced tweaking of each servo's movement behaviour.


## Table of Content

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
  - [Getting Started](#getting-started)
  - [Control by Value](#control-by-value)
    - [Common Values](#common-values)
  - [Control by Percent](#control-by-percent)
  - [Control by Pulse Width](#control-by-pulse-width)
  - [Frequency Control](#frequency-control)
  - [Phase Control](#phase-control)
  - [Calibration](#calibration)
  - [Delayed Loading](#delayed-loading)
  - [Function Reference](#function-reference)
  - [PIO Limitations](#pio-limitations)
- [Calibration](#calibration)
  - [Getting Started](#getting-started)
  - [Function Reference](#function-reference)


## Servo

### Getting Started

To start using servos with your Servo 2040, you will need to first import the `Servo` class.
```python
from servo import Servo, servo2040
```
If you are using another RP2040 based board, then `servo2040` can be omitted from the above line

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

The value of a servo can be set calling `.value(value)`, which takes a float as its input. If the servo is disabled, this will enable it. The resulting pulse width will also be stored.

To read back the current value of the servo, call `.value()` without any input. If the servo is disabled, this will be the last value that was provided when enabled.


#### Common Values

To simplify certain motion patterns, a servo can be commanded to three common values. These are, their minimum, middle, and maximum. These are performed by calling `.to_min()`, `.to_mid()`, and `.to_max()`, respectively. If the servo is disabled, these will enable it.

It is also possible to read back the values each of those three commands is using internally, using `.min_value()`, `.mid_value()`, and `.max_value()`. These can be useful as inputs to equations that provide numbers directly to `.value(value)`, for example.


### Control by Percent

Sometimes there are projects where a servo needs to move based on the reading from a sensor or another device, but the numbers given out are not easy to convert to values the servo accepts. To overcome this the library lets you move the servo to a percent between its minimum and maximum values, or two values provided, based on that input.

With an input between `-1.0` and `1.0`, a servo can be moved to a percent between its minimum and maximum values using `s.to_percent(in)`.

With an input between a provided min and max, a servo can be moved to a percent between its minimum and maximum values using `s.to_percent(in, in_min, in_max)`.

With an input between a provided min and max, a servo can be moved to a percent between two provided values using `s.to_percent(in, in_min, value_min, value_max)`.

If the servo is disabled, these will enable it.


### Control by Pulse Width

At a hardware level servos operate by receiving a digital signal with specific pulse widths. Typically pulses are between 500 microseconds and 2500 microseconds in length, and are usually repeated every 20 milliseconds (50Hz). These functions let you interact with pulse widths directly.

The pulse width (in microseconds) of a servo can be set by calling `.pulse(pulse)`, which takes a float as its input. If the servo is disabled this will enable it, except for the case of `0` where instead the servo will be disabled. This function will also recalculate the related value.

To read back the current pulse width (in microseconds) of the servo, call `.pulse()` without any input. If the servo is disabled, this will be the last pulse that was provided when enabled.


### Frequency Control

The vast majority of Servos expect to receive pulses with a frequency of 50Hz, so this library uses that as its default. However, there may be cases where this value needs to be changed, such as when using servos that operate up to frequencies of 333Hz.

The frequency (in Hz) of a servo can be set by calling `.frequency(freq)`, which takes a float as its input. The supported range of this input is `10` to `350` Hz.

To read back the current frequency (in Hz) of the servo, call `.frequency()` without any input.

Note that changing the frequency does not change the pulse widths sent to the servos, only how frequently they are sent. This is why `350` is the upper limit, as any higher and the maximum pulse would be longer than the time period. If you encounter any servos where this behaviour is not what they expect, please raise it as aa Github issue.


### Calibration

There are different types of servos, with `ANGULAR`, `LINEAR`, and `CONTINUOUS` being common. To support these different types, each Servo class contains a calibration object that stores the specific value to pulse mapping needed for its type. A copy of this can be accessed using `.calibration()`. It is also possible to provide a servo with a new calibration using `.calibration(calibration)`.


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

The next step is to choose which GPIO pins the cluster will be connected to. The easiest way to do this for a contiguous block is to use a `range` to create each pin number, and convert that to a `list`. For example, the below line creates the list `[0, 1, 2, 3]`
```python
pins = list(range(servo2040.SERVO_1, servo2040.SERVO_4 + 1))
```

To create your servo cluster, specify the PIO, PIO state-machine and GPIO pins you chose a moment ago, and pass those into `ServoCluster`. For this example we will use the handy constants of the `servo2040`.

```python
cluster = ServoCluster(0, 0, pins)
```

You now have a `ServoCluster` class called `cluster` that will control the physical servos connected to `SERVO_1`, `SERVO_2`, `SERVO_3`, and `SERVO_4`.

From here there are various ways to control these servos. These are covered in more detail in the following sections.



#### Constructing a ServoCluster

```python
ServoCluster(
  pio,              # int: the pio device to use for the cluster (0 or 1)
  sm,               # int: the state machine on that pio to use for the cluster (0 to 3)
  type=ANGULAR,     # int: the type the servos will start as (options are ANGULAR, LINEAR, CONTINUOUS, and EMPTY)
  freq=50,          # float: the frequency the servos will receive pulses at
  auto_phase=True   # bool: whether to automatically set the servo phases to reduce peak current draw
)
```

### Function Reference

Here is the complete list of functions available on the `ServoCluster` class:

```python
ServoCluster(pio, sm, pins, calibration=ANGULAR, freq=50, auto_phase=True)
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

### Count

To get the number of servos assigned to this cluster:
```python
count() # returns int: the number of servos
```

### Pin

To get the pin number a servo on the cluster is assigned to:
```python
pin(
  servo   # int: the servo to get the pin of
) # returns int: the hardware pin of the servo
```

### Enabling and Disabling

To enable a servo on the cluster:
```python
enable(
  servo,      # int: the servo to enable
  load=True   # bool: whether to load the change immediately
)
```
If the servo has not previously been enabled, it will default to the middle of its range.

To disable a servo on the cluster:
```python
disable(
  servo,      # int: the servo to disable
  load=True   # bool: whether to load the change immediately
)
```

To check the enabled state of a servo on the cluster:
```python
is_enabled(
  servo,      # int: the servo to check the enabled state of
) # returns bool: True if enabled, False if disabled
```

### Control by Pulse Width

Servos operate by receiving a digital signal with specific pulse widths. Typically values are between 500 microseconds and 2500 microseconds.

To read the current pulse the servo is receiving:
```python
pulse(
  servo       # int: the servo to read the pulse of
) # returns float: the duration of the pulse in microseconds
```
If the servo is disabled, this will be the last pulse that was provided when enabled.

To set a new pulse for the servo to receive:
```python
pulse(
  servo,      # int: the servo to set the pulse of
  pulse,      # float: the pulse duration in microseconds
  load=True   # bool: whether to load the change immediately
)
```
If the servo is disabled, this will also enable it. It will also recalculate the related value.

### Control by Value

Value is a way to control servos using numbers that have a real-world meaning, rather than with pulse widths. For instance, -90 to +90 degrees for an angular servo, or -1 to +1 for a continous rotation servo. See [Calibration](#calibration) for more details.

To read the current value the servo is at:
```python
value(
  servo       # int: the servo to read the value of
) # returns float: the value
```
If the servo is disabled, this will be the last value that was provided when enabled.

To set the servo to a new value:
```python
value(
  servo,      # int: the servo to set the value of
  value,      # float: the value
  load=True   # bool: whether to load the change immediately
)
```
If the servo is disabled, this will also enable it. The resulting pulse width will also be stored.

### Phase Control

When dealing with many servos, there can often be large current draw spikes caused by them all responding to pulses at the same time. To minimise this, a servo cluster allows for the start time of a servo's pulses to be delayed by a percentage of the available time period. This is called their phase.

To read the current phase of a servo on the cluster:
```python
phase(
  servo       # int: the servo to read the phase of
) # returns float: the phase, between 0.0 and 1.0
```

To set the phase of a servo on the cluster:
```python
phase(
  servo,      # int: the servo to set the phase of
  phase,      # float: the phase, between 0.0 and 1.0
  load=True   # bool: whether to load the change immediately
)
```

### Frequency Control

The vast majority of Servos expect to receive pulses with a frequency of 50Hz, so this library uses that as its default. However, there may be cases where this value needs to be changed, such as when using servos that operate up to frequencies of 333Hz.
All servos on a cluster share the same frequency.

To read the current servo cluster pulse frequency:
```python
frequency() # returns float: the frequency in Hz
```

To set a new servo cluster pulse frequency:
```python
frequency(
  freq   # float: the frequency between 10 and 350Hz
)
```
Note, currently the frequency changes immediately, even if part-way through a pulse. It is recommended to disable all servos first before changing the frequency.


### Useful Values

When performing motion patterns on a servo, it can sometimes be useful to know what a servo's minimum, middle, and maximum values are.

To get the minimum value a servo on the cluster can reach:
```python
min_value(
  servo,      # int: the servo to get the min value of
)   # returns float: the minimum value
```

To get the middle value of a servo on the cluster:
```python
mid_value(
  servo,      # int: the servo to get the mid value of
)   # returns float: the middle value
```

To get the maximum value a servo on the cluster:
```python
mid_value(
  servo,      # int: the servo to get the max value of
)   # returns float: the maximum value
```

Similarly, it can be useful to command a servo to one of these values without needing to know the actual number.

To move a servo on the cluster to its minimum value.
```python
to_min(
  servo,      # int: the servo to move to its min value
  load=True   # bool: whether to load the change immediately
)
```

To move a servo on the cluster to its middle value.
```python
to_mid(
  servo,      # int: the servo to move to its mid value
  load=True   # bool: whether to load the change immediately
)
```

To move a servo on the cluster to its maximum value.
```python
to_max(
  servo,      # int: the servo to move to its max value
  load=True   # bool: whether to load the change immediately
)
```

### Control by Percent

Sometimes there are projects where you want a servo to move based on the reading from a sensor or another device, but the numbers given out are not easy to convert to values the servo accepts. To overcome this the library lets you move the servo to a percent between its minimum and maximum values, or two values you provided, based on that input.

With an input between `-1.0` and `1.0`, move a servo on the cluster to a percent between its minimum and maximum values:
```python
to_percent(
  servo,      # int: the servo to move to the percent
  in          # float: the input, from -1.0 to +1.0
  load=True   # bool: whether to load the change immediately
)
```

With an input between a provided min and max, move a servo on the cluster to a percent between its minimum and maximum values:
```python
to_percent(
  servo,      # int: the servo to move to the percent
  in,         # float: the input, from in_min to in_max
  in_min,     # float: the minimum expected input
  in_max      # float: the maximum expected input
  load=True   # bool: whether to load the change immediately
)
```

With an input between a provided min and max, move a servo on the cluster to a percent between two provided values:
```python
to_percent(
  servo,      # int: the servo to move to the percent
  in,         # float: the input, from in_min to in_max
  in_min,     # float: the minimum expected input
  in_max      # float: the maximum expected input
  value_min,  # float: the value the servo will go to when receiving a minimum input
  value_max   # float: the value the servo will go to when receiving a maximum input
  load=True   # bool: whether to load the change immediately
)
```

### Calibrating

There are different types of servos, with angular, linear, and continuous being common. To support these different types, each Servo class contains a calibration object that stores the specific value to pulse mapping needed for its type. This object can be accessed for each servo as well as updated on the fly.

To get the calibration of a servo on the cluster:
```python
calibration(
  servo,      # int: the servo to get the calibration of
)   # returns Calibration: a copy of the servo's calibration
```

To update the calibration of a servo on the cluster.
```python
calibration(
  servo,        # int: the servo to set the calibration of
  calibration   # Calibration: the object to update the servo's calibration with
)
```

### Delayed Loading

To match behaviour with the regular Servo class, the ServoCluster automatically applies each change to a servo's state immediately. However, sometimes this may not be wanted, and instead you want all servos to receive updated pulses at the same time, regardless of how long the code ran that calculated the update.

For this purpose, all the functions that modify a servo state include an optional parameter `load`, which by default is `True`. To avoid this "loading" include `load=False` in the relevant function calls. Then either the last call can include `load=True`, or the following function can be called:
```python
load()
```


### PIO Limitations

The RP2040 features two PIOs with four state machines each. This places a hard limit on how many ServoClusters can be created. As this class is capable of driving all 30 GPIO pins, the only time this limit will be of concern is when servos with different frequencies are wanted, as all the outputs a ServoCluster controls share the same frequency. Relating this to the hardware PWM, think of it as a single PWM slice with up to 30 sub channels, A, B, C, D etc.

When creating a ServoCluster, in most cases you'll use `0` for PIO and `0` for PIO state-machine. You should change these though if you plan on running multiple clusters, or using a cluster alongside something else that uses PIO, such as our [Plasma library](https://github.com/pimoroni/pimoroni-pico/tree/main/micropython/modules/plasma).


## Calibration

### Getting Started

### Function Reference

Here is the complete list of functions available on the `Calibration` class:

```python
Calibration(type=ANGULAR)

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

