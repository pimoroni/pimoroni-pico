# Plasma Stick MicroPython Examples <!-- omit in toc -->

- [About Plasma Stick](#about-plasma-stick)
- [Plasma Library](#plasma-library)
- [Using Breakouts](#using-breakouts)
- [Basic Examples](#basic-examples)
  - [Alternating Blinkies](#alternating-blinkies)
  - [Fire](#fire)
  - [Moon](#moon)
  - [Pulse](#pulse)
  - [Rainbows](#rainbows)
  - [Snow](#snow)
  - [Sparkles](#sparkles)
  - [Thermometer](#thermometer)
  - [Tree](#tree)
- [Advanced Examples](#advanced-examples)
  - [CO2](#co2)
  - [Encoder](#encoder)
  - [Moon (RTC)](#moon-rtc)
  - [PIR](#pir)
  - [Thermometer (BME280)](#thermometer-bme280)
- [Wireless Examples](#wireless-examples)
  - [Cheerlights](#cheerlights)
  - [Weather](#weather)
- [Other Resources](#other-resources)

## About Plasma Stick

Plasma Stick 2040 W is a compact controller for WS2812 strip, powered by Raspberry Pi Pico W and perfect for easy, seasonal lighting. It has built in ✨wireless connectivity✨, courtesy of the onboard Pico W.

You can buy one on its own or in a kit:

- [Plasma Stick 2040 W](https://shop.pimoroni.com/products/plasma-stick-2040-w)

- [Wireless Plasma Kit](https://shop.pimoroni.com/products/wireless-plasma-kit)

Plasma Stick ships with MicroPython firmware pre-loaded, but you can download the most recent version at the link below (you'll want the  `pimoroni-picow` image).

- [MicroPython releases](https://github.com/pimoroni/pimoroni-pico/releases)

## Plasma Library

You can control your WS2812 / NeoPixel™ strip using our handy MicroPython Plasma library.

- [PicoGraphics MicroPython function reference](../../modules/plasma)

Note that the examples in this directory default to a RGB colour order to match the LEDs in the Wireless Plasma Kit. If you're using different LEDs you may need to adjust the colour order in your code - most of the other addressable LEDs we sell are GRB.

## Using Breakouts

Plasma Stick has a Qw/ST (Qwiic/STEMMA QT) connector. Breakouts with Qw/ST connectors, can be plugged straight in with a [JST-SH to JST-SH cable](https://shop.pimoroni.com/products/jst-sh-cable-qwiic-stemma-qt-compatible?variant=31910609813587). You can connect I2C Breakout Garden breakouts without Qw/ST connectors using a [JST-SH to JST-SH cable](https://shop.pimoroni.com/products/jst-sh-cable-qwiic-stemma-qt-compatible?variant=31910609813587) and a [Qw/ST to Breakout Garden adaptor](https://shop.pimoroni.com/products/stemma-qt-qwiic-to-breakout-garden-adapter).

- [List of breakouts currently supported in our C++/MicroPython build](https://github.com/pimoroni/pimoroni-pico#breakouts)

Plasma Stick uses GP4 and GP5 for its I2C interface. You can use the constants in the shared `pimoroni` module to set up the I2C interface:

```python
from pimoroni_i2c import PimoroniI2C
from pimoroni import BREAKOUT_GARDEN_I2C_PINS

i2c = PimoroniI2C(**BREAKOUT_GARDEN_I2C_PINS)
```

Alternatively, you can specify the pin numbers directly:

```python
from pimoroni_i2c import PimoroniI2C

i2c = PimoroniI2C(sda=(4), scl=(5))
```

## Basic Examples

### Alternating Blinkies

[alternating-blinkies.py](alternating-blinkies.py)

A simple example with two alternating colours, great for festive lights!

### Fire

[fire.py](fire.py)

A simple 🔥 fire effect example 🤘 (warning, flashy). 

### Moon

[moon.py](moon.py)

Spooky moon simulator - the LEDs will get brighter as midnight approaches!
Needs to be run from Thonny to get the correct time.

### Pulse

[pulse.py](pulse.py)

Adjust the brightness or saturation of the LEDs using a sine wave.

### Rainbows

[rainbows.py](rainbows.py)

Some good old fashioned rainbows!

### Snow

[snow.py](snow.py)

Snow in a bottle!

### Sparkles

[sparkles.py](sparkles.py)

A festive, customisable sparkly effect.

### Thermometer

[thermometer_pico.py](thermometer_pico.py)

Reads the temperature from the Pico W's internal temperature sensor and changes the LED strip an appropriate colour.

### Tree

[tree.py](tree.py)

A Christmas tree simulator.

## Advanced Examples

These examples require additional hardware.

### CO2

[co2.py](co2.py)

Reads CO2 level from a [SCD41 CO2 breakout](https://shop.pimoroni.com/products/scd41-co2-sensor-breakout) and turns the LED strip an appropriate colour.

### Encoder

[encoder.py](encoder.py)

Adjust the colour of your LEDs easily with an [RGB Encoder breakout](https://shop.pimoroni.com/products/rgb-encoder-breakout?variant=32236590399571).

### Moon (RTC)

[moon_rtc.py](moon_rtc.py)

Spooky moon simulator - the LEDs will get brighter as midnight approaches!
Gets the time from a [RV3028 RTC breakout](https://shop.pimoroni.com/products/rv3028-real-time-clock-rtc-breakout).

### PIR

[pir.py](pir.py)

Connect a PIR motion sensor and trigger some ominous effects. We like [these ones](https://shop.pimoroni.com/products/micro-pir-motion-sensor-2-pcs) - we connected ours to the QwST connector using [this cable](https://shop.pimoroni.com/products/jst-sh-cable-qwiic-stemma-qt-compatible?variant=31910609846355) and some [socket to socket](https://shop.pimoroni.com/products/jumper-jerky-junior?variant=1076482185) jumper jerky.

### Thermometer (BME280)

[thermometer_bme280.py](thermometer_bme280.py)

Reads the temperature from a [BME280 breakout](https://shop.pimoroni.com/products/bme280-breakout) and changes the LED strip an appropriate colour.

## Wireless Examples

The wireless examples need `network_manager.py` and `WIFI_CONFIG.py` from the `common` directory to be saved to your Pico W. Open up `WIFI_CONFIG.py` in Thonny to add your wifi details (and save it when you're done).

- [micropython/examples/common](../../examples/common)

### Cheerlights

[cheerlights.py](cheerlights.py)

Sets your LED strip to the current #cheerlights colour.
Find out more about the Cheerlights API at [https://cheerlights.com/].

### Weather

[weather.py](weather.py)

This Plasma Stick example connects to Open Meteo to access the current weather conditions.
It then does some cool weather appropriate stuff with LEDs.
Find out more about the Open Meteo API at [https://open-meteo.com].

## Other Resources

Here are some Plasma Stick community projects and resources that you might find useful! Note that code at the links below has not been tested by us and we're not able to offer support with it.

- :link: [MQTT Script for Plasma Stick](https://github.com/digitalurban/MQTT-Plasma-Stick-2040W)
- :link: [Pimoroni Wireless Plasma Kit - Server](https://github.com/brunon/Starlight)
