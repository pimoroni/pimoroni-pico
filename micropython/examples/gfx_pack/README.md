# Pico GFX Pack MicroPython Examples <!-- omit in toc -->
 
- [About Pico GFX Pack](#about-pico-gfx-pack)
- [Pico GFX Pack and PicoGraphics](#pico-gfx-pack-and-picographics)
- [Basic Examples](#basic-examples)
  - [Balls Demo](#balls-demo)
  - [Button Test](#button-test)
  - [Calc](#calc)
  - [Rainbow](#rainbow)
  - [Snake](#snake)
- [Advanced Examples](#advanced-examples)
  - [CO2](#co2)
  - [Thermometer](#thermometer)
- [Wireless Examples](#wireless-examples)
  - [Carbon Intensity Graph](#carbon-intensity-graph)
  - [Sunrise / Sunset](#sunrise--sunset)
  - [Zoo Facts](#zoo-facts)
- [Other Resources](#other-resources)

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

### Calc

[calc.py](calc.py)

This example draws a nice sine wave on the display, reminiscent of the graphical calculators from the 90s.

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

- :link: [Qw/ST cable store page](https://shop.pimoroni.com/products/jst-sh-cable-qwiic-stemma-qt-compatible?variant=31910609813587)

### CO2

[co2.py](co2.py)

Add a SCD41 sensor breakout to your GFX Pack to make a handy CO2 detector!
Press A to reset the high/low values.

- :link: [SCD41 breakout store page](https://shop.pimoroni.com/products/scd41-co2-sensor-breakout)

### Thermometer

[thermometer.py](thermometer.py)

This demo uses a BME680 or BME688 breakout to measure temperature, pressure, and humidity and display it on the GFX display. 

To use the Pico's internal temperature sensor in place of the BME68x breakout, just change `use_bme68x_breakout` to `False`.

- :link: [BME680 breakout store page](https://shop.pimoroni.com/products/bme680-breakout)
- :link: [BME688 breakout store page](https://shop.pimoroni.com/products/bme688-breakout)

## Wireless Examples

These wireless examples need `network_manager.py` and `WIFI_CONFIG.py` from the `common` directory to be saved to your Pico W. Open up `WIFI_CONFIG.py` in Thonny to add your wifi details (and save it when you're done).

- [micropython/examples/common](../../examples/common)

### Carbon Intensity Graph

[carbon_intensity.py](carbon_intensity.py)

Displays near realtime information about the different types of electricity generation for a given UK postcode area.  Uses the backlight and a bar graph to show the current mix of generation sources, and updates itself periodically or when button E is pressed.

Find out more about the Carbon Intensity API here:  https://carbon-intensity.github.io/api-definitions/#carbon-intensity-api-v2-0-0

### Sunrise / Sunset

[sunrise.py](sunrise.py)

This sunrise / sunset simulator displays information from the Sunrise Sunset API, and also shows how to use a 16x16 animated sprite.
Find out more about Sunrise Sunset API here:  https://sunrise-sunset.org/api

### Zoo Facts

[zoo_facts.py](zoo_facts.py)

Downloads a list of five zoo animals and displays their vital statistics.
Find out more about Zoo Animal API here: https://zoo-animal-api.herokuapp.com/

- A = Next animal
- B = Last animal
- D = Show stats
- E = Fetch a different 5 animals

## Other Resources

Here are some GFX Pack community projects and resources that you might find useful! Note that code at the links below has not been tested by us and we're not able to offer support with it.

- :link: [Octopus Energy Monitor](https://github.com/MaverickUK/OctopusEnergyMonitorPicoW)
- :link: [Redis MicroPython Workshop](https://github.com/simonprickett/micropython-workshop)



