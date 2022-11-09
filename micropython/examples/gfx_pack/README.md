# Pico GFX Pack MicroPython Examples <!-- omit in toc -->
 
- [About Pico GFX Pack](#about-pico-gfx-pack)
- [Pico GFX Pack and PicoGraphics](#pico-gfx-pack-and-picographics)
- [Basic Examples](#basic-examples)
  - [Balls Demo](#balls-demo)
  - [Button Test](#button-test)
  - [Rainbow](#rainbow)
  - [Snake](#snake)
- [Advanced Examples](#advanced-examples)
  - [CO2](#co2)
  - [Thermometer](#thermometer)

## About Pico GFX Pack

GFX Pack adds a 128x64 LCD Matrix display to your headered Raspberry Pi Pico or PicoW, with RGBW backlight and 5 input buttons for all your display and control needs.

- :link: [Pico GFX Pack store page](https://shop.pimoroni.com/products/pico-gfx-pack)

You'll need to download the most recent version of our MicroPython firmware for your board from the link below.

- [MicroPython releases](https://github.com/pimoroni/pimoroni-pico/releases)
- [Installing MicroPython](../../../setting-up-micropython.md)

## Pico GFX Pack and PicoGraphics

The easiest way to start displaying cool stuff on GFX Pack is using our GFX Pack helper module (which contains a bunch of useful shortcuts for interacting with the board) and our PicoGraphics display library which handles drawing things on the screen.

- [GFX Pack function reference](../../modules_py/gfx_pack.md)
- [PicoGraphics function reference](../../modules/picographics/README.md)

## Basic Examples

### Balls Demo

[balls_demo.py](balls_demo.py)

LCD demo with a bunch of bouncy balls!

### Button Test

[button_test.py](button_test.py)

Shows how to read the buttons, display text and change the colour of the RGBW backlight.

### Rainbow

[rainbow.py](rainbow.py)

This example borrows a CircuitPython hsv to rgb function to cycle through some rainbows on GFX Pack's screen.

### Snake

[snake.py](snake.py)

Basic Snake demo for GFX Pack.

- A = up
- B = down
- C = reset
- D = left
- E = right

## Advanced Examples

These examples require additional hardware. We connected our breakouts using a JST-SH to JST-SH cable:

- :link: [Qw/ST cable store page](https://shop.pimoroni.com/products/jst-sh-cable-qwiic-stemma-qt-compatible?variant=31910609846355)

### CO2

[co2](co2.py)

Add a SCD41 sensor breakout to your GFX Pack to make a handy CO2 detector!
Press A to reset the high/low values.

- :link: [SCD41 breakout store page](https://shop.pimoroni.com/products/scd41-co2-sensor-breakout)

### Thermometer

[thermometer.py](thermometer.py)

This demo uses a BME680 or BME688 breakout to measure temperature, pressure, and humidity and display it on the GFX display. 

To use the Pico's internal temperature sensor in place of the BME68x breakout, just change `use_bme68x_breakout` to `False`.

- :link: [BME680 breakout store page](https://shop.pimoroni.com/products/bme680-breakout)
- :link: [BME688 breakout store page](https://shop.pimoroni.com/products/bme688-breakout)
