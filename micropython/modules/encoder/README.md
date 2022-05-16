# Encoders <!-- omit in toc -->

The Encoder library lets you read incremental rotary encoders from a Raspberry Pi Pico or any other RP2040-based board, such as the [Pimoroni Motor 2040](https://pimoroni.com/motor2040).

This library offers an `Encoder` class that uses Programmable IO (PIO) hardware to read a single encoder, with support for up to 8 encoders.


## Table of Content
- [Encoder](#encoder)
  - [Getting Started](#getting-started)
  - [Count and Angle](#count-and-angle)
  - [Count Delta](#count-delta)
  - [Step and Turn](#step-and-turn)
  - [Changing the Direction](#changing-the-direction)
  - [Resetting to Zero](#resetting-to-zero)
  - [Capture](#capture)
  - [State](#state)
  - [Function Reference](#function-reference)
  - [Constants Reference](#constants-reference)
  - [PIO Limitations](#pio-limitations)


## Encoder

### Getting Started

To start using encoders, you will need to first import the `Encoder` class.
```python
from encoder import Encoder
```

To create your encoder, specify the PIO, PIO state-machine and GPIO pins it will be connected to, and pass them into `Encoder`. The example below shows the typical setup for a rotary encoder that has A, B and C pins.
```python
PIN_A = 26    # The A channel pin
PIN_B = 28    # The B channel pin
PIN_C = 27    # The common pin
enc = Encoder(0, 0, (PIN_A, PIN_B), PIN_C)
```

If using a board such as Motor 2040 with its motor encoder support, then the encoder's GPIO pins can be assigned using one of the handy constants of `motor2040`.
```python
from encoder import MMME_CPR
from motor import motor2040
enc = Encoder(0, 0, motor2040.ENCODER_A, counts_per_rev=MMME_CPR, count_microsteps=True)
```
As the above example shows, motor encoders often have a different number of counts per revolution than a traditional rotary encoder. For our MMME shims the `MMME_CPR` constant is provided. Also, as motor encoders lack the tactile steps of rotary encoders, their counting resolution can be increased by a factor of four by including each microstep.

Following either of the above setups gives you an `Encoder` class called `enc` that will monitor the state of the physical encoder.

From this point on the encoder can be read in several ways, depending on the intended application. These are covered in more detail in the following sections.


### Count and Angle

The Encoder class keeps track of an encoder's current count, which gets increased or reduced as the physical encoder is turned. This count is read by calling `.count()`.

The count can also be read back as either the number of `.revolutions()` of the encoder, or the angle in `.degrees()` or `.radians()`. These use the encoder's counts per revolution, which can either be specified when the object is created, or updated by calling `.counts_per_rev(counts_per_rev)`.

Be aware that the count is stored as an integer, if it is continually increased or decreased it will eventually wrap at `+2147483647` and `-2147483648`. This will cause a jump in the returned by `.revolutions()`, `degrees()` and `.radians()`, that will need handling by your code.


### Count Delta

Often you are not interested in the exact count that the encoder is at, but rather if the count has changed since the last time you checked. This change can be read by calling `.delta()` at regular intervals. The returned value can then be used with a check in code, for the value being non-zero.


### Step and Turn

If using a rotary encoder with a physical direction marker, it can be useful to know its position in the form of which step it is at and how many turns have occurred. The current step can be read by calling `.step()`, which returns a value from zero up to the encoder's `counts_per_rev - 1`. The number of turns can be read by calling `.turn()`.

These functions differ from reading the `.count()` or `.revolutions()` by using seperate counters, and so avoid the wrapping issue that these functions experience.


### Changing the Direction

The counting direction of an encoder can be changed either by providing `direction=REVERSED_DIR` when creating the `Encoder` object, or by calling `.direction(REVERSED_DIR)` at any time in code. The `REVERSED_DIR` constant comes from the `pimoroni` module. There is also a `NORMAL_DIR` constant, though this is the default.


### Resetting to Zero

There are times where an encoder's count (and related values) need to be reset back to zero. This can be done by calling `.zero()`.


### Capture

There are times with encoders when you want to query multiple pieces of information from a single moment in time. Typically this would be done by reading the values up-front for use later, rather than reading them at the point they are needed in your code. Unfortunately even this offers no guarantee that the values were from a single moment, particularly when dealing with high speed encoders.

To support this usage the encoder class has a `.capture()` function, which returns a named tuple containing the encoder's:
```
count
delta
frequency
revolutions
degrees
radians
revolutions_delta
degrees_delta
radians_delta
revolutions_per_second
revolutions_per_minute
degrees_per_second
radians_per_second
```

Internally `.capture()` does the same up-front reading of values but does so more optimally within the underlying C++ driver. As an added bonus, it calculates encoder speeds too, by using the captured `delta` along with timing information returned by the PIO, more accurately than estimating a speed from the `delta` alone.


### State

The raw state of an encoder's pins can be read using `.state()`, which returns pair of booleans for the A and B pins. This can be useful for visualising the signals of an encoder alongside its other values.


### Function Reference

Here is the complete list of functions available on the `Encoder` class:
```python
Encoder(pio, sm, pins, common_pin=PIN_UNUSED, direction=NORMAL_DIR, counts_per_rev=ROTARY_CPR, count_microsteps=False, freq_divider=1)
pins()
common_pin()
state()
count()
delta()
zero()
step()
turn()
revolutions()
degrees()
radians()
direction()
direction(direction)
counts_per_rev()
counts_per_rev(counts_per_rev)
capture()
```

### Constants Reference

Here is the complete list of constants on the `encoder` module:

* `ROTARY_CPR` = `24`
* `MMME_CPR` = `12`

Here are useful constants from the `pimoroni` module:

* `NORMAL_DIR` = `0`
* `REVERSED_DIR` = `1`


### PIO Limitations

The RP2040 features two PIOs with four state machines each. This places a hard limit on how many Encoders can be created of 8.

When creating an Encoder, in most cases you'll use `0` for PIO and `0` for PIO state-machine. You should change these though if you plan on running multiple encoders, or using encoders alongside something else that uses PIO, such as our [Plasma library](https://github.com/pimoroni/pimoroni-pico/tree/main/micropython/modules/plasma).
