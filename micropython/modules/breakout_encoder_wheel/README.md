# RGB Encoder Wheel Breakout (Micropython) <!-- omit in toc -->

This is the Micropython library reference for the [Pimoroni RGB Encoder Wheel Breakout](https://shop.pimoroni.com/products/rgb-encoder-wheel-breakout).


## Table of Content <!-- omit in toc -->
- [Getting Started](#getting-started)
- [Reading the Buttons](#reading-the-buttons)
- [Reading the Encoder](#reading-the-encoder)
  - [Count and Angle](#count-and-angle)
  - [Count Delta](#count-delta)
  - [Step and Turn](#step-and-turn)
  - [Changing the Direction](#changing-the-direction)
  - [Resetting to Zero](#resetting-to-zero)
- [LEDs](#leds)
  - [Setting an LED](#setting-an-led)
    - [RGB](#rgb)
    - [HSV](#hsv)
  - [Clear all LEDs](#clear-all-leds)
  - [Showing](#showing)
- [GPIOs](#gpios)
  - [Setup](#setup)
  - [Mode](#mode)
  - [As Input or ADC](#as-input-or-adc)
  - [As Output](#as-output)
  - [As PWM](#as-pwm)
    - [Delayed Loading](#delayed-loading)
    - [Limitations](#limitations)
- [Function Reference](#function-reference)
- [Constants Reference](#constants-reference)
  - [Address Constants](#address-constants)
  - [Button Constants](#button-constants)
  - [GPIO Constants](#gpio-constants)
  - [Count Constants](#count-constants)


## Getting Started

To start coding for your Encoder Wheel breakout, you will first need to create an object for accessing the I2C bus that the breakout is connected to. The easiest way to do this is via the `PimoroniI2C` class, with one of the handy pin constants from `pimoroni`, like so:

```python
from pimoroni_i2c import PimoroniI2C
from pimoroni import BREAKOUT_GARDEN_I2C_PINS  # or PICO_EXPLORER_I2C_PINS or HEADER_I2C_PINS
i2c = PimoroniI2C(**BREAKOUT_GARDEN_I2C_PINS)
```

This creates a `i2c` variable that can be passed into the Encoder Wheel's class as part of its creation:
```python
from breakout_encoder_wheel import BreakoutEncoderWheel
wheel = BreakoutEncoderWheel(i2c)
```

The above lines of code import the `BreakoutEncoderWheel` class and create an instance of it, called `wheel`. This will be used in the rest of the examples going forward.


## Reading the Buttons

EncoderWheel has five buttons, covering up, down, left, right, and centre. These can be read using the `.pressed(button)` function, which accepts a button number between `0` and `4`. For convenience, each button can be referred to using these constants:

* `UP` = `0`
* `DOWN` = `1`
* `LEFT` = `2`
* `RIGHT` = `3`
* `CENTRE` = `4`

For example, to read the centre button you would write:

```python
centre_state = wheel.pressed(CENTRE)
```

You can also get the number of buttons using the `NUM_BUTTONS` constant.


## Reading the Encoder

Within the directional buttons of the Encoder Wheel breakout is a rotary encoder with 24 counts per revolution.

### Count and Angle

The current count can be read by calling `.count()`. It can also be read back as either the number of `.revolutions()` of the encoder, or the angle in `.degrees()` or `.radians()`.

Be aware that the count is stored as an integer, if it is continually increased or decreased it will eventually wrap at `+32767` and `-32768`. This will cause a jump in the returned by `.revolutions()`, `degrees()` and `.radians()`, that will need handling by your code.


### Count Delta

Often you are not interested in the exact count that the encoder is at, but rather if the count has changed since the last time you checked. This change can be read by calling `.delta()` at regular intervals. The returned value can then be used with a check in code, for the value being non-zero.


### Step and Turn

Sometimes it can be useful to know the encoder's position in the form of which step it is at and how many turns have occurred. The current step can be read by calling `.step()`, which returns a value from `0` to `23`, and the number of turns can be read by calling `.turn()`.

These functions differ from reading the `.count()` or `.revolutions()` by using separate counters, and so avoid the wrapping issue that these functions experience.


### Changing the Direction

The counting direction of an encoder can be changed by calling `.direction(REVERSED_DIR)` at any time in code. The `REVERSED_DIR` constant comes from the `pimoroni` module. There is also a `NORMAL_DIR` constant, though this is the default.


### Resetting to Zero

There are times where an encoder's count (and related values) need to be reset back to zero. This can be done by calling `.zero()`.


## LEDs

The Encoder Wheel breakout features 24 RGB LEDs arranged in a ring around the wheel. This is the same number as there are steps on the wheel, letting you use the LEDs to show the current step of the wheel.


### Setting an LED

You can set the colour of a LED on the ring in either the RGB colourspace, or HSV (Hue, Saturation, Value). HSV is useful for creating rainbow patterns.

#### RGB

Set the first LED - `0` - to Purple `255, 0, 255`:

```python
wheel.set_rgb(0, 255, 0, 255)
```

#### HSV

Set the first LED - `0` - to Red `0.0`:

```python
wheel.set_hsv(0, 0.0, 1.0, 1.0)
```


### Clear all LEDs

To turn off all the LEDs, the function `.clear()` can be called. This simply goes through each LED and sets its RGB colour to black, making them emit no light.

This function is useful to have at the end of your code to turn the lights off, otherwise they will continue to show the last colours they were given.


### Showing

Changes to the LEDs do not get applied immediately, due to the amount of I2C communication involved. As such, to have the LEDs show what they have been set to after calling the `.set_rgb()`, `.set_hsv()`, and `.clear()` functions, a specific call to `.show()` needs to be made.


## GPIOs

There are three spare GPIO pins on the edge of Encoder Wheel. These can be used as digital outputs, pwm outputs, digital inputs, and analog inputs.


### Setup

To start using a GPIO pin, first import one of the handy constants used to reference them (see [GPIO Constants](#gpio-constants)). For example, to use the first GPIO pin:

```python
from breakout_encoder_wheel import GP7
```

Then you need to import the constants for the pin mode to use. These are on the `breakout_ioexpander` module that Encoder Wheel is based on.

```python
# For input
from breakout_ioexpander import IN  # or IN_PU of a pull-up is wanted

# For output
from breakout_ioexpander import OUT

# For PWM
from breakout_ioexpander import PWM

# For ADC
from breakout_ioexpander import ADC
```


### Mode

With the intended constants imported, the mode of a GPIO pin can be set by calling `.gpio_pin_mode(gpio, mode)`:

```python
wheel.gpio_pin_mode(GP7, <IN or IN_PU or OUT or PWM or ADC>)
```

It is also possible to read the current mode of a GPIO pin by calling `.gpio_pin_mode(gpio)`:

```python
mode = wheel.gpio_pin_mode(GP7)
```


### As Input or ADC

The current value of an GPIO pin in input or ADC mode can be read by calling `.gpio_pin_value(gpio)`:

```python
value = wheel.gpio_pin_value(GP7)
```

If the mode is digital, the value will either be `0` or `1`.
If the mode is analog, the value will be a voltage from `0.0` to `3.3`.


### As Output

The current value of a GPIO pin in output mode can be set by calling `.gpio_pin_value(gpio, value)`:

```python
wheel.gpio_pin_value(GP7, value)
```

The expected value is either `0` or `1`, or `True` or `False`.


### As PWM

The GPIO pins can also be set as PWM outputs. The `PWM` constant can be imported from the `breakout_ioexpander` module, and passed into the `.gpio_pin_mode()` function.

The frequency of the PWM signal can then be configured by calling `.gpio_pwm_frequency()`, which accepts a frequency (in Hz). It returns the cycle period, which should be used to set duty cycles.

Finally, the duty cycle of the PWM signal can be set by calling `.gpio_pin_value()` and providing it with a value between `0` and the cycle period.

Below is an example of setting a gpio pin to output a 25KHz signal with a 50% duty cycle:

```python
from pimoroni_i2c import PimoroniI2C
from pimoroni import BREAKOUT_GARDEN_I2C_PINS  # or PICO_EXPLORER_I2C_PINS or HEADER_I2C_PINS
from breakout_ioexpander import PWM
from breakout_encoder_wheel import BreakoutEncoderWheel, GP7

# Initialise EncoderWheel
i2c = PimoroniI2C(**BREAKOUT_GARDEN_I2C_PINS)
wheel = BreakoutEncoderWheel(i2c)

# Setup the gpio pin as a PWM output
wheel.gpio_pin_mode(GP7, PWM)

# Set the gpio pin's frequency to 25KHz, and record the cycle period
period = wheel.gpio_pwm_frequency(25000)

# Output a 50% duty cycle square wave
wheel.gpio_pin_value(GP7, int(period * 0.5))
```


#### Delayed Loading

By default, changes to a gpio pin's frequency or value are applied immediately. However, sometimes this may not be wanted, and instead you want all pins to receive updated parameters at the same time, regardless of how long the code ran that calculated the update.

For this purpose, `.gpio_pwm_frequency()` and `.gpio_pin_value()` include an optional parameter `load`, which by default is `True`. To avoid this "loading" include `load=False` in the relevant function calls. Then either the last call can include `load=True`, or a specific call to `.gpio_pwm_load()` can be made.

In addition, any function that performs a load, including the `.gpio_pwm_load()` function, can be made to wait until the new PWM value has been sent out of the pins. By default this is disabled, but can be enabled by including `wait_for_load=True` in the relevant function calls.


#### Limitations

All of Encoder Wheel's PWM outputs share the same timing parameters. This means that GP7, GP8, and GP9 share the same frequency. Keep this in mind if changing the frequency of one, as the others will not automatically know about the change, resulting in unexpected duty cycle outputs.


## Function Reference

Here is the complete list of functions available on the `BreakoutEncoderWheel` class:
```python
BreakoutEncoderWheel(ioe_address=0x13, led_address=0x77, interrupt=PIN_UNUSED)
set_ioe_address(address)
get_interrupt_flag()
clear_interrupt_flag()
pressed(button)
count()
delta()
step()
turn()
zero()
revolutions()
degrees()
radians()
direction()
direction(direction)
set_rgb(index, r, g, b)
set_hsv(index, h, s=1.0, v=1.0)
clear()
show()
gpio_pin_mode(gpio)
gpio_pin_mode(gpio, mode)
gpio_pin_value(gpio)
gpio_pin_value(gpio, value, load=True, wait_for_load=False)
gpio_pwm_load(wait_for_load=True)
gpio_pwm_frequency(frequency, load=True, wait_for_load=True)
```

## Constants Reference

Here is the complete list of constants on the `breakoutencoderwheel` module:

### Address Constants

* `DEFAULT_IOE_I2C_ADDR` = `0x13`
* `DEFAULT_LED_I2C_ADDR` = `0x77`
* `ALTERNATE_LED_I2C_ADDR` = `0x74`


### Button Constants

* `UP` = `0`
* `DOWN` = `1`
* `LEFT` = `2`
* `RIGHT` = `3`
* `CENTRE` = `4`


### GPIO Constants

* `GP7` = `7`
* `GP8` = `8`
* `GP9` = `9`
* `GPIOS` = (`7`, `8`, `9`)


### Count Constants

* `NUM_LEDS` = `24`
* `NUM_BUTTONS` = `5`
* `NUM_GPIOS` = `5`
