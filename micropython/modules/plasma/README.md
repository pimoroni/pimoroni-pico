# Plasma <!-- omit in toc -->

The Plasma library is intended to drive APA102 / DotStar™ or WS2812 / NeoPixel™ LEDs on our [Plasma 2040](https://shop.pimoroni.com/products/plasma-2040) board, though it can also be used with your own custom pins/wiring.

It can also be used to drive WS2812 / NeoPixel™ LEDs from [Plasma Stick](https://shop.pimoroni.com/products/plasma-stick-2040-w). Note that APA102 compatibility, user buttons, RGB LED and current sensing functions are not present on Plasma Stick.

- [Notes On PIO Limitations](#notes-on-pio-limitations)
- [WS2812](#ws2812)
  - [Getting Started](#getting-started)
  - [RGBW and Setting Colour Order](#rgbw-and-setting-colour-order)
  - [Set An LED](#set-an-led)
    - [RGB](#rgb)
    - [HSV](#hsv)
- [APA102](#apa102)
  - [Getting Started](#getting-started-1)
  - [Set An LED](#set-an-led-1)
    - [RGB](#rgb-1)
    - [HSV](#hsv-1)
  - [Set Brightness](#set-brightness)
- [Using the Buttons & RGB LED](#using-the-buttons--rgb-led)
  - [Buttons](#buttons)
  - [RGBLED](#rgbled)
- [Measuring LED Strip Current Draw](#measuring-led-strip-current-draw)
  - [Analog](#analog)

## Notes On PIO Limitations

The WS2812 and APA102 drivers use the PIO hardware on the RP2040. There are only two PIOs with four state machines each, placing a hard limit on how many separate LED strips you can drive.

In most cases you'll use `0` for PIO and `0` for PIO state-machine, but you should change these if you plan on running different strand types together, or if you're using something else that uses PIO.

## WS2812

### Getting Started

Construct a new `WS2812` instance, specifying the number of LEDs, PIO, PIO state-machine and GPIO pin.

For Plasma 2040:

```python
import plasma
from plasma import plasma2040

LEDS = 30
FPS = 60

led_strip = plasma.WS2812(LEDS, 0, 0, plasma2040.DAT)
```

For Plasma Stick:

```python
import plasma
from plasma import plasma_stick

LEDS = 30
FPS = 60

led_strip = plasma.WS2812(LEDS, 0, 0, plasma_stick.DAT)
```

Start the LED strip by calling `start`. This sets up a timer which tells the RP2040 to DMA the pixel data into the PIO (a fast, asyncronous memory->peripheral copy) at the specified framerate.

```python
led_strip.start(FPS)
```

### RGBW and Setting Colour Order

Some WS2812-style LED strips have varying colour orders and support an additional white element. Two keyword arguments are supplied to configure this:

```python
led_strip = plasma.WS2812(LEDS, 0, 0, plasma2040.DAT, rgbw=True, color_order=plasma.COLOR_ORDER_GRB)
```

The available orders are defined as constants in `plasma`:

* `COLOR_ORDER_RGB`
* `COLOR_ORDER_RBG`
* `COLOR_ORDER_GRB`
* `COLOR_ORDER_GBR`
* `COLOR_ORDER_BRG`
* `COLOR_ORDER_BGR`

### Set An LED

You can set the colour of an LED in either the RGB colourspace, or HSV (Hue, Saturation, Value). HSV is useful for creating rainbow patterns.

#### RGB

Set the first LED - `0` - to Purple `255, 0, 255`:

```python
led_strip.set_rgb(0, 255, 0, 255)
```

#### HSV

Set the first LED - `0` - to Red `0.0`:

```python
led_strip.set_hsv(0, 0.0, 1.0, 1.0)
```

## APA102

### Getting Started

Construct a new `APA102` instance, specifying the number of LEDs, PIO, PIO state-machine and GPIO data/clock pins.

```python
import plasma
from plasma import plasma2040

LEDS = 30
FPS = 60

led_strip = plasma.APA102(LEDS, 0, 0, plasma2040.DAT, plasma2040.CLK)
```

Start the LED strip by calling `start`. This sets up a timer which tells the RP2040 to DMA the pixel data into the PIO (a fast, asyncronous memory->peripheral copy) at the specified framerate.

```python
led_strip.start(FPS)
```

### Set An LED

You can set the colour of an LED in either the RGB colourspace, or HSV (Hue, Saturation, Value). HSV is useful for creating rainbow patterns.

#### RGB

Set the first LED - `0` - to Purple `255, 0, 255`:

```python
led_strip.set_rgb(0, 255, 0, 255)
```

#### HSV

Set the first LED - `0` - to Red `0.0`:

```python
led_strip.set_hsv(0, 0.0, 1.0, 1.0)
```

### Set Brightness

APA102 pixels support global brightness, allowing their brightness to be specified independent of their colour. You can set the overall brightness of your strip by calling:

```python
led_strip.set_brightness(15)
```

You can set brightness from `0` to `31`. This directly maps to the 5-bit brightness value sent to the APA102 LEDs.

## Using the Buttons & RGB LED

The `pimoroni` module contains `Button` and `RGBLED` classes to simplify button debounce, auto-repeat and PWM'ing an RGB LED.

```python
Button(button, invert=True, repeat_time=200, hold_time=1000)
```

```python
RGBLED(r, g, b, invert=True)
```

The `plasma` module contains a `plasma2040` sub module with constants for the LED and button pins:

* `plasma2040.LED_R` = 16
* `plasma2040.LED_G` = 17
* `plasma2040.LED_B` = 18
* `plasma2040.BUTTON_A` = 12
* `plasma2040.BUTTON_B` = 13
* `plasma2040.USER_SW` = 23

### Buttons

Import the `Button` class from the `pimoroni` module and the pin constants for the buttons:

```python
from pimoroni import Button
from plasma import plasma2040
```

Set up an instance of `Button` for each button:

```python
button_a = Button(plasma2040.BUTTON_A)
button_b = Button(plasma2040.BUTTON_B)
```

To get the button state, call `.read()`. If the button is held down, then this will return `True` at the interval specified by `repeat_time` until `hold_time` is reached, at which point it will return `True` every `repeat_time / 3` milliseconds. This is useful for rapidly increasing/decreasing values such as hue:

```python
state = button_a.read()
```

### RGBLED

Import the `RGBLED` class from `pimoroni` and the pin constants for the LED:

```python
from pimoroni import RGBLED
from plasma import plasma2040
```

And set up an instance of `RGBLED` for the LED:

```python
led = RGBLED(plasma2040.LED_R, plasma2040.LED_G, plasma2040.LED_B)
```

To set the LED colour, call `.set_rgb(r, g, b)`. Each value should be between 0 and 255:

```python
led.set_rgb(255, 0, 0)  # Full red
led.set_rgb(0, 255, 0)  # Full green
led.set_rgb(0, 0, 255)  # Full blue
```

## Measuring LED Strip Current Draw

Plasma 2040 features low-side current sensing, letting you measure how much current a strip of LEDs is drawing. This could be used just for monitoring, or as a way to reduce the maximum brightness of a strip to keep its current draw within the range of the USB port or power supply being used.

The `pimoroni` module contains an `Analog` class to simplify the reading of this current draw.

```python
Analog(pin, amplifier_gain=1, resistor=0)
```

The `plasma` module contains a `plasma2040` sub module with constants for the current sensing:

* `plasma2040.CURRENT_SENSE` = 29
* `plasma2040.ADC_GAIN` = 50
* `plasma2040.SHUNT_RESISTOR` = 0.015

### Analog

Import the `Analog` class from `pimoroni` and the pin and gain constants for the current sensing:

```python
from pimoroni import Analog
from plasma import plasma2040
```

And set up an instance of `Analog` for the current sensing:

```python
sense = Analog(plasma2040.CURRENT_SENSE, plasma2040.ADC_GAIN, plasma2040.SHUNT_RESISTOR)
```

To read the current draw, call `.read_current()`. The returned value will be in amps (A):

```python
print("Current =", sense.read_current(), "A")
```
