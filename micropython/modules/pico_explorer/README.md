# Pico Explorer Base

Pico Explorer Base straps a whole host of physical computing goodies to your Pico - a vibrant 1.14" (240x240) IPS LCD screen, four switches, a piezo buzzer/speaker and a DRV8833 motor driver. It also has a bunch of handy accessible general purpose inputs and outputs and a built in breadboard.

[You can buy one here!](https://shop.pimoroni.com/products/pico-explorer-base)

The `picoexplorer` module contains constants you can use with our shared libraries to make it easy to draw to the screen and interface with the buttons, piezo buzzer and motor driver. 

toc goes here

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



--



### Motors

Motors are driven by PWM via an onboard DRV8833.  We'd recommend using our Motor library to driver them - here's a quick example

The red LED next to the motor connectors is part of the motor driver circuit - it will light up if the overvoltage/undervoltage/short circuit auto shutdown functions of the motor driver are triggered. It's not user controllable.

#### set_motor

```python
picoexplorer.set_motor(channel, action, speed)
```

Channel should be one of `0` (motor 1) or `1` (motor 2).

Action should be `0` (forwards) or `1` (backwards).

Speed should be given as a number between `0.0` and `1.0`, eg:

```python
picoexplorer.set_motor(0, 0, 0.5)    # motor 1 forwards
picoexplorer.set_motor(1, 1, 0.5)    # motor 2 backwards
```

And to stop the motor:

```python
picoexplorer.set_motor(0, 0, 0)      # motor 1 stop
picoexplorer.set_motor(1, 0, 0)      # motor 2 stop
```

### Audio

To make noise with Explorer, you must first select one of the GP0 to GP7  pins to PWM for audio. You'll then need to connect this pin to AUDIO with a jumper wire.

#### set_audio_pin

```python
picoexplorer.set_audio_pin(channel)
```

`set_audio_pin` configures the PIN that Pico Explorer uses for audio output. It should be one of `GP0` through `GP7`, eg:

```python
picoexplorer.set_audio_pin(0)
```

This pin must be bridged to the `AUDIO` pin on the Pico Explorer header in order to drive the onboard Piezo.

#### set_tone

```python
picoexplorer.set_tone(frequency)
```

`set_tone` will play an audio tone out of your chosen audio pin.

```python
frequency = 440
picoexplorer.set_tone(frequency)
```

To make the buzzer be quiet, you can:

``` python
picoexplorer.set_tone(-1)
```

### GPIO

The 8 general purpose IO pins on the lower Pico Explorer are GP0 through GP7. You can use `machine` to read a pin in the same way as you would if you were using a Pico on its own.

```python
import machine

GP0 = machine.Pin(0, machine.Pin.IN, machine.Pin.PULL_DOWN)
```

There's lots more info about how to use `machine` in the [Raspberry Pi documentation](https://www.raspberrypi.org/documentation/rp2040/getting-started/#getting-started-with-micropython).

## Pin Constants

Here's a list of constants that are available in `picoexplorer`, and the pin numbers that they correspond to on the Pico.

**Buttons**

- `BUTTON_A` = `12`
- `BUTTON_B` = `13`
- `BUTTON_X` = `14`
- `BUTTON_Y` = `15`

**ADC**

- `ADC0`
- `ADC1`
- `ADC2`

**Motors**

- `MOTOR1`
- `MOTOR2`

**GPIO**

- `GP0`
- `GP1`
- `GP2`
- `GP3`
- `GP4`
- `GP5`
- `GP6`
- `GP7`

