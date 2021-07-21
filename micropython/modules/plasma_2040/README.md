# Plasma <!-- omit in toc -->

The Plasma library is intended to drive APA102 / DotStar™ or WS2812 / NeoPixel™ LEDs on the Plasma 2040 board, though it can be used with your own custom pins/wiring.

- [Notes On PIO Limitations](#notes-on-pio-limitations)
- [APA102](#apa102)
  - [Getting Started](#getting-started)
  - [Set An LED](#set-an-led)
    - [RGB](#rgb)
    - [HSV](#hsv)
  - [Set Brightness](#set-brightness)
- [WS2812](#ws2812)
  - [Getting Started](#getting-started-1)
  - [Set An LED](#set-an-led-1)
    - [RGB](#rgb-1)
    - [HSV](#hsv-1)

## Notes On PIO Limitations

The WS2812 and APA102 drivers use the PIO hardware on the RP2040. There are only two PIOs with four state machines each, placing a hard limit on how many separate LED strips you can drive.

In most cases you'll use `0` for PIO and `0` for PIO state-machine, but you should change these if you plan on running different strand types together, or if you're using something else that uses PIO.

## APA102

### Getting Started

Construct a new `WS2812` instance, specifying the number of LEDs, PIO, PIO state-machine and GPIO pin.

```python
import plasma

LEDS = 30
FPS = 60

led_strip = plasma.WS2812(LEDS, 0, 0, 15)
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
led_strip.set_led(0, 255, 0, 255)
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

## WS2812

### Getting Started

Construct a new `APA102` instance, specifying the number of LEDs, PIO, PIO state-machine and GPIO data/clock pins.

```python
import plasma

LEDS = 30
FPS = 60

led_strip = plasma.APA102(LEDS, 0, 0, 15, 14)
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
led_strip.set_led(0, 255, 0, 255)
```

#### HSV

Set the first LED - `0` - to Red `0.0`:

```python
led_strip.set_hsv(0, 0.0, 1.0, 1.0)
```