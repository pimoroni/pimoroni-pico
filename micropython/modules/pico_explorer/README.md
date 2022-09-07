# Pico Explorer Base <!-- omit in toc -->

Pico Explorer Base straps a whole host of physical computing goodies to your Pico - a vibrant 1.14" (240x240) IPS LCD screen, four switches, a piezo buzzer/speaker and a DRV8833 motor driver. It also has a bunch of handy accessible general purpose inputs and outputs and a built in breadboard.

[You can buy one here!](https://shop.pimoroni.com/products/pico-explorer-base)

- [Board Functions](#board-functions)
  - [Display](#display)
  - [Buttons](#buttons)
  - [ADC](#adc)
  - [Motors](#motors)
  - [Audio](#audio)
  - [GPIO](#gpio)
  - [Breakout Garden slots / I2C](#breakout-garden-slots--i2c)
- [Pins](#pins)

## Board Functions

### Display

Pico Explorer uses a shared Pico Graphics library to draw graphics and text on its screen. As a result, code written for our other Pico add-ons with a display should be easy to convert to run on Explorer.

- [PicoGraphics MicroPython function reference](https://github.com/pimoroni/pimoroni-pico/tree/main/micropython/modules/picographics)

Please note that the backlight on Pico Explorer is not dimmable (we needed the pins to hook up all the other functions) so `set_backlight` won't do anything on this board.

Here's a simple Hello World example that uses PicoGraphics to set up the display and draw some text on the screen.

```python
from picographics import PicoGraphics, DISPLAY_PICO_EXPLORER

# Set up the display
display = PicoGraphics(display=DISPLAY_PICO_EXPLORER)

# Create a pen colour to draw with
WHITE = display.create_pen(255, 255, 255)

# Choose a font and switch to the white pen
display.set_font("bitmap8")
display.set_pen(WHITE)

# Display some text
display.text("Hello Explorer", 0, 0, scale=3)

# Update the screen
display.update()
```

### Buttons

The four buttons, A, B, X and Y have corresponding constants set to their respective GPIO pins (12, 13, 14 and 15). They can be read with the shared `pimoroni` module which contains a `Button` class that handles button debounce and auto-repeat.

```
Button(button, invert=True, repeat_time=200, hold_time=1000)
```

To set up the buttons, first import the `Button` class from the `pimoroni` module:

```
from pimoroni import Button
```

Then create instances of `Button` using the correct pin numbers:

```
button_a = Button(12)
button_b = Button(13)
button_x = Button(14)
button_y = Button(15)
```

To get the button state, call `.read()`. If the button is held down, then this will return `True` at the interval specified by `repeat_time` until `hold_time` is reached, at which point it will return `True` every `repeat_time / 3` milliseconds. This is useful for rapidly increasing/decreasing values:

```
state = button_a.read()
```

It is also possible to read the raw button state without the repeat feature, if you prefer:

```
state = button_a.raw()
```

### ADC

Pico Explorer's ADC channels are connected to Pico's ADC-capable pins (26, 27 and 28). You can read the voltages from them using the `Analog` class in the shared `pimoroni` module.

First import the `Analog` class from the `pimoroni` module:

```
from pimoroni import Analog
```

Then create instances of `Analog` using the correct pin numbers:

```python
 adc0 = Analog(26)
 adc1 = Analog(27)
 adc2 = Analog(28)
```

And read them like this

```python
reading =  adc0.read_voltage()
```

### Motors

Motors are driven by PWM via an onboard DRV8833.  We'd recommend using our fully featured Motor library to drive them - here's a quick example for how to drive motor 1:

```python
from motor import Motor
import time

m1 = Motor((8, 9))

m1.enable()

# run the motor full speed in one direction for 2 seconds
m1.speed(1.0)
time.sleep(2)

# and in the opposite direction for 2 seconds
m1.speed(-1.0)
time.sleep(2)

m1.disable()
```

You can find much more info about working with motors in the [Motor library documentation](https://github.com/pimoroni/pimoroni-pico/tree/main/micropython/modules/motor).

The red LED next to the motor connectors is part of the motor driver circuit - it will light up if the overvoltage/undervoltage/short circuit auto shutdown functions of the motor driver are triggered. It's not user controllable.

### Audio

To make noise with Explorer, you must select one of the GP0 to GP7 pins to PWM for audio (We're using GP0 in the code below). You'll then need to connect this pin to AUDIO with a jumper wire.

To set up the buzzer, first import the `Buzzer` class from the `pimoroni` module:

```python
from pimoroni import Buzzer
```

Then create a `Buzzer` instance:

```python
buzzer = Buzzer(0)
```

You can then play audio tones like this - frequency should probably be a number between 1 and 5000 if you have human ears. 

```python
buzzer.set_tone(frequency)
```

To make the buzzer be quiet, you can:

```python
buzzer.set_tone(0)
```

### GPIO

The 8 general purpose IO pins on Pico Explorer are connected to GP0 through GP7. You can use `machine` to read inputs in the same way as you would if you were using a Pico on its own.

```python
import machine

GP0 = machine.Pin(0, machine.Pin.IN, machine.Pin.PULL_DOWN)
```

You can also use these pins as outputs, if you wanted to connect up stuff like LEDs:

```python
import machine

GPO = machine.Pin(0, machine.Pin.OUT)
```

Note that if you're connecting external LEDs up to Explorer Base, GP0-7 have built in 100 Ohm resistors, so you don't need to include a resistor in your circuit to protect your LED from drawing too much current.

There's lots more info about how to use `machine` in the [Raspberry Pi documentation](https://www.raspberrypi.org/documentation/rp2040/getting-started/#getting-started-with-micropython).

### Breakout Garden slots / I2C

The slots at the top of the board let you plug (I2C) Breakout Garden breakouts into Pico Explorer.

- [List of Pico-compatible breakouts](https://github.com/pimoroni/pimoroni-pico/blob/main/README.md#breakouts)

Pico Explorer uses GP20 and GP21 for its I2C interface - these pins differ from our default Breakout Garden pins so you will need to specify you're using a Pico Explorer when running breakout examples. You can use the constants in the shared `pimoroni` module to set up the I2C interface:

```python
from pimoroni_i2c import PimoroniI2C
from pimoroni import PICO_EXPLORER_I2C_PINS

i2c = PimoroniI2C(**PICO_EXPLORER_I2C_PINS)
```

Alternatively, you can specify the pin numbers directly:

```python
from pimoroni_i2c import PimoroniI2C

i2c = PimoroniI2C(sda=(20), scl=(21))
```

## Pins

Here's a list of the pins associated with the various features of Pico Explorer. You can also find a list of pins on the underneath of the board!

**GPIO (General Purpose Input Output) pins**

- GP0 = `0`
- GP1 = `1`
- GP2 = `2`
- GP3 = `3`
- GP4 = `4`
- GP5 = `5`
- GP6 = `6`
- GP7 = `7`

**Buttons/Switches**

- Button A = `12`
- Button B = `13`
- Button X = `14`
- Button Y = `15`

**Motors**

- Motor 1 (-) = `8`
- Motor 1 (+) = `9`
- Motor 2 (-) = `10`
- Motor 2 (+) = `11`

**SPI pins (used for the display)**

- SPI MISO - `16`
- LCD CS - `17`
- SPI SCK - `18`
- SPI MOSI - `19`

**I2C pins**

- I2C SDA - `20`
- I2C SCL - `21`
- I2C INT - `22`

**Analog pins**

- ADC0 = `26`
- ADC1 = `27`
- ADC2 = `28`