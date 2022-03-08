# Servo <!-- omit in toc -->

## At a glance
### Servo
```python
Servo(pin, type=ANGULAR, freq=50)
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

### ServoCluster
```python
ServoCluster(pio, sm, pins, type=ANGULAR, freq=50, auto_phase=True)
count()
pin(servo)
enable(servo, load=True)
disable(servo, load=True)
is_enabled(servo)
pulse(servo)
pulse(servo, pulse, load=True)
value(servo)
value(servo, value, load=True)
phase(servo)
phase(servo, phase, load=True)
frequency()
frequency(freq)
min_value(servo)
mid_value(servo)
max_value(servo)
to_min(servo, load=True)
to_mid(servo, load=True)
to_max(servo, load=True)
to_percent(servo, in, load=True)
to_percent(servo, in, in_min, in_max, load=True)
to_percent(servo, in, in_min, in_max, value_min, value_max, load=True)
calibration(servo)
calibration(servo, calibration)
load()
```

### Calibration
```python
Calibration(type=ANGULAR)

create_blank_calibration(size)
create_two_point_calibration(min_pulse, max_pulse, min_value, max_value)
create_three_point_calibration(min_pulse, mid_pulse, max_pulse, min_value, mid_value, max_value)
create_uniform_calibration(size, min_pulse, max_pulse, min_value, max_value)
create_default_calibration(type)
size()
point_at(index)
point_at(index, point)
first_point()
first_point(point)
last_point()
last_point(point)
has_lower_limit()
has_upper_limit()
limit_to_calibration(lower, upper)
value_to_pulse(value)
pulse_to_value(pulse)
```

## Breakdown

### Servo Class

#### Constructing a Servo

```python
Servo(
  pin,            # int: the hardware pin to use for the servo
  type=ANGULAR,   # int: the type the servo will start as (options are ANGULAR, LINEAR, CONTINUOUS, and EMPTY)
  freq=50         # float: the frequency the servo will receive pulses at
)
```

#### Pin

To get the pin number the servo is assigned to:
```python
pin() # returns int: the hardware pin of the servo
```

#### Enabling and Disabling

To enable the servo:
```python
enable()
```
If the servo has not previously been enabled, it will default to the middle of its range.

To disable the servo:
```python
disable()
```

To check the enabled state of the servo:
```python
is_enabled() # returns bool: True if enabled, False if disabled
```

#### Control by Pulse Width

Servos operate by receiving a digital signal with specific pulse widths. Typically values are between 500 microseconds and 2500 microseconds.

To read the current pulse the servo is receiving:
```python
pulse() # returns float: the duration of the pulse in microseconds
```
If the servo is disabled, this will be the last pulse that was provided when enabled.

To set a new pulse for the servo to receive:
```python
pulse(
  pulse   # float: the pulse duration in microseconds
)
```
If the servo is disabled, this will also enable it. It will also recalculate the related value.


#### Control by Value

Value is a way to control servos using numbers that have a real-world meaning, rather than with pulse widths. For instance, -90 to +90 degrees for an angular servo, or -1 to +1 for a continous rotation servo. See [Calibration](#calibration) for more details.

To read the current value the servo is at:
```python
value() # returns float: the value
```
If the servo is disabled, this will be the last value that was provided when enabled.

To set the servo to a new value:
```python
value(
  value   # float: the value
)
```
If the servo is disabled, this will also enable it. The resulting pulse width will also be stored.


#### Frequency Control

The vast majority of Servos expect to receive pulses with a frequency of 50Hz, so this library uses that as its default. However, there may be cases where this value needs to be changed, such as when using servos that operate up to frequencies of 333Hz.

To read the current servo pulse frequency:
```python
frequency() # returns float: the frequency in Hz
```

To set a new servo pulse frequency:
```python
frequency(
  freq   # float: the frequency between 10 and 350Hz
)
```
If the servo is disabled, this will enable it with the last pulse it was provided with.
TODO change this behaviour!

#### Useful Values

When performing motion patterns on a servo, it can sometimes be useful to know what a servo's minimum, middle, and maximum values are.

To get the minimum value the servo can reach:
```python
min_value()   # returns float: the minimum value
```

To get the middle value of the servo:
```python
mid_value()   # returns float: the middle value
```

To get the maximum value the servo can reach:
```python
mid_value()   # returns float: the maximum value
```

Similarly, it can be useful to command a servo to one of these values without needing to know the actual number.

To move the servo to its minimum value.
```python
to_min()
```

To move the servo to its middle value.
```python
to_mid()
```

To move the servo to its maximum value.
```python
to_max()
```

#### Control by Percent

Sometimes there are projects where you want a servo to move based on the reading from a sensor or another device, but the numbers given out are not easy to convert to values the servo accepts. To overcome this the library lets you move the servo to a percent between its minimum and maximum values, or two values you provided, based on that input.

With an input between `-1.0` and `1.0`, move the servo to a percent between its minimum and maximum values:
```python
to_percent(
  in    # float: the input, from -1.0 to +1.0
)
```

With an input between a provided min and max, move the servo to a percent between its minimum and maximum values:
```python
to_percent(
  in,     # float: the input, from in_min to in_max
  in_min, # float: the minimum expected input
  in_max  # float: the maximum expected input
)
```

With an input between a provided min and max, move the servo to a percent between two provided values:
```python
to_percent(
  in,         # float: the input, from in_min to in_max
  in_min,     # float: the minimum expected input
  in_max      # float: the maximum expected input
  value_min,  # float: the value the servo will go to when receiving a minimum input
  value_max   # float: the value the servo will go to when receiving a maximum input
)
```

#### Calibration

There are different types of servos, with angular, linear, and continuous being common. To support these different types, each Servo class contains a calibration object that stores the specific value to pulse mapping needed for its type. This object can be accessed for each servo as well as updated on the fly.

To get the calibration of the servo:
```python
calibration()   # returns Calibration: a copy of the servo's calibration
```

To update the calibration of the servo.
```python
calibration(
  calibration   # Calibration: the object to update the servo's calibration with
)
```


### ServoCluster Class

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

#### Pin

To get the number of servos assigned to this cluster:
```python
count() # returns int: the number of servos
```

#### Pin

To get the pin number a servo on the cluster is assigned to:
```python
pin(
  servo   # int: the servo to get the pin of
) # returns int: the hardware pin of the servo
```

#### Enabling and Disabling

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

#### Control by Pulse Width

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


#### Control by Value

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


#### Phase Control

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

#### Frequency Control

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
If the servo is disabled, this will enable it with the last pulse it was provided with.
TODO change this behaviour!

Note, currently the frequency changes immediately, even if part-way through a pulse. It is recommended to disable all servos first before changing the frequency.

#### Useful Values

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

#### Control by Percent

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

#### Calibration

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

#### Delayed Loading

To match behaviour with the regular Servo class, the ServoCluster automatically applies each change to a servo's state immediately. However, sometimes this may not be wanted, and instead you want all servos to receive updated pulses at the same time, regardless of how long the code ran that calculated the update.

For this purpose, all the functions that modify a servo state include an optional parameter `load`, which by default is `True`. To avoid this "loading" include `load=False` in the relevant function calls. Then either the last call can include `load=True`, or the following function can be called:
```python
load()
```