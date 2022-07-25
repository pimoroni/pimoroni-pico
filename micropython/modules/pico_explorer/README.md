# Pico Explorer Base <!-- omit in toc -->

Pico Explorer Base straps a whole host of physical computing goodies to your Pico - a vibrant 1.14" (240x240) IPS LCD screen, four switches, a piezo buzzer/speaker and a DRV8833 motor driver. It also has a bunch of handy accessible general purpose inputs and outputs and a built in breadboard.

[You can buy one here!](https://shop.pimoroni.com/products/pico-explorer-base)

The `picoexplorer` module contains constants you can use with our shared libraries to make it easy to draw to the screen and interface with the buttons, piezo buzzer and motor driver. You don't need to use the constants of course, you can skip the `import pico_explorer` and just enter the pin number/s if you prefer. There's a handy reference of what pins are used for which functions on the bottom of the board.

- [Board Functions](#board-functions)
  - [Display](#display)
  - [Buttons](#buttons)
  - [ADC](#adc)
  - [Motors](#motors)
  - [Audio](#audio)
  - [GPIO](#gpio)
  - [Breakout Garden slots / I2C](#breakout-garden-slots--i2c)
- [Pin Constants](#pin-constants)

## Board Functions

### Display

Pico Explorer uses a shared Pico Graphics library to draw graphics and text on its screen. As a result, code written for our other Pico add-ons with a display should be easy to convert to run on Explorer.

- [PicoGraphics MicroPython function reference](https://github.com/pimoroni/pimoroni-pico/tree/main/micropython/modules/picographics)

Please note that the backlight on Pico Explorer is not dimmable (we needed the pins to hook up other functions) so `set_backlight` won't do anything on this board.

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

To set up the buttons, first import the `Button` class from the `pimoroni` module and the pin constants from `picoexplorer`:

```
from pimoroni import Button
import picoexplorer
```

Then create instances of `Button` using our constants:

```
button_a = Button(picoexplorer.BUTTON_A)
button_b = Button(picoexplorer.BUTTON_B)
button_x = Button(picoexplorer.BUTTON_X)
button_y = Button(picoexplorer.BUTTON_Y)
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

First import the `Analog` class from the `pimoroni` module and the pin constants from `picoexplorer`:

```
from pimoroni import Analog
import pico explorer
```

Then create instances of `Analog` using our constants:

```python
 adc0 = Analog(picoexplorer.ADC0)
 adc1 = Analog(picoexplorer.ADC1)
 adc2 = Analog(picoexplorer.ADC2)
```

And read them like this

```python
reading =  adc0.read_voltage()
```

### Motors

Motors are driven by PWM via an onboard DRV8833.  We'd recommend using our fully featured Motor library to drive them - here's a quick example:

``` python
import picoexplorer
from motor import Motor
import time

m = Motor(picoexplorer.MOTOR_1)

m.enable()

# run the motor full speed in one direction for 2 seconds
m.speed(1.0)
time.sleep(2)

# and in the opposite direction for 2 seconds
m.speed(-1.0)
time.sleep(2)

m.disable()
```

You can find much more info about working with motors in the [Motor library documentation](https://github.com/pimoroni/pimoroni-pico/tree/main/micropython/modules/motor).

The red LED next to the motor connectors is part of the motor driver circuit - it will light up if the overvoltage/undervoltage/short circuit auto shutdown functions of the motor driver are triggered. It's not user controllable.

### Audio

To make noise with Explorer, you must select one of the GP0 to GP7 pins to PWM for audio. You'll then need to connect this pin to AUDIO with a jumper wire.

To set up the buzzer, first import the `Buzzer` class from the `pimoroni` module and the pin constants from `picoexplorer`:

``` python
from pimoroni import Buzzer
import picoexplorer
```

Then create a `Buzzer` instance:

``` python
BUZZER = Buzzer(picoexplorer.GP0)
```

You can then play audio tones like this - frequency should probably be a number between 1 and 5000 if you have human ears. 

``` python
buzzer.set_tone(frequency)
```

To make the buzzer be quiet, you can:

``` python
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

Pico Explorer uses GP20 and GP21 for its I2C interface - these pins differ from our default Breakout Garden pins so you will specify you're using a Pico Explorer when running breakout examples. You can use the constants in the shared `pimoroni` module to set up the I2C interface:

``` python
from pimoroni_i2c import PimoroniI2C
from pimoroni import PICO_EXPLORER_I2C_PINS

i2c = PimoroniI2C(**PICO_EXPLORER_I2C_PINS)
```

Alternatively, you can specify the pin numbers directly:

``` python
from pimoroni_i2c import PimoroniI2C

i2c = PimoroniI2C(sda=(20), scl=(21))
```
## Pin Constants

Here's a list of constants that are available in `picoexplorer`, and the pin numbers that they correspond to on the Pico.

**Buttons**

- `BUTTON_A` = `12`
- `BUTTON_B` = `13`
- `BUTTON_X` = `14`
- `BUTTON_Y` = `15`

**ADC**

- `ADC0` = `26`
- `ADC1` = `27`
- `ADC2` = `28`

**Motors**

- `MOTOR_1` = `8, 9`
- `MOTOR_2` = `10, 11`
