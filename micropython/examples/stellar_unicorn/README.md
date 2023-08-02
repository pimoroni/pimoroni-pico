# Stellar Unicorn MicroPython Examples <!-- omit in toc -->
 
- [About Stellar Unicorn](#about-stellar-unicorn)
- [Stellar Unicorn and PicoGraphics](#stellar-unicorn-and-picographics)
- [Examples](#examples)
  - [Clock](#clock)
  - [Eighties Super Computer](#eighties-super-computer)
  - [Feature Test](#feature-test)
  - [Feature Test With Audio](#feature-test-with-audio)
  - [Fire Effect](#fire-effect)
  - [Lava Lamp](#lava-lamp)
  - [Light Sensor](#light-sensor)
  - [Nostalgia Prompt](#nostalgia-prompt)
  - [Rainbow](#rainbow)
  - [Scrolling Text](#scrolling-text)
  - [Thermometer](#thermometer)
  - [Today](#today)
- [Wireless Examples](#wireless-examples)
  - [Cheerlights History](#cheerlights-history)
  - [Stellar Paint](#stellar-paint)
  - [Exchange Ticker](#exchange-ticker)
  - [HTTP Text](#http-text)
  - [Weather](#weather)
- [NumPy Examples](#numpy-examples)
- [Other Examples](#other-examples)
  - [CO2](#co2)
  - [Encoder Wheel](#encoder-wheel)
  - [Thermometer (BME280)](#thermometer-bme280)
  - [Thermometer (BME68x)](#thermometer-bme68x)
  - [Launch (Demo Reel)](#launch-demo-reel)

## About Stellar Unicorn

Stellar Unicorn offers 16x16 bright RGB LEDs driven by Pico W's PIO in addition to a 1W amplifier + speaker, a collection of system and user buttons, and two Qw/ST connectors for adding external sensors and devices. Woha!

- :link: [Stellar Unicorn store page](https://shop.pimoroni.com/products/stellar-unicorn)

Stellar Unicorn ships with MicroPython firmware pre-loaded, but you can download the most recent version at the link below (you'll want the  `stellar-unicorn` image).

- [MicroPython releases](https://github.com/pimoroni/pimoroni-pico/releases)
- [Installing MicroPython](../../../setting-up-micropython.md)

## Stellar Unicorn and PicoGraphics

The easiest way to start displaying cool stuff on Stellar Unicorn is using our Stellar Unicorn module (which contains a bunch of helpful functions for interacting with the buttons, adjusting brightness and suchlike) and our PicoGraphics library, which is chock full of useful functions for drawing on the LED matrix.

- [Stellar Unicorn function reference](../../modules/stellar_unicorn/README.md)
- [PicoGraphics function reference](../../modules/picographics/README.md)

## Examples

### Clock

[clock.py](clock.py)

Clock example with (optional) NTP synchronization. You can adjust the brightness with LUX + and -, and resync the time by pressing A.

This example uses a custom tiny bitmap font, find 3x5.bitmapfont in [fonts](../../common/fonts) and copy it to your Pico W.

### Eighties Super Computer

[eighties_super_computer.py](eighties_super_computer.py)

Random LEDs blink on and off mimicking the look of a movie super computer doing its work in the eighties. You can adjust the brightness with LUX + and -.

### Feature Test

[feature_test.py](feature_test.py)

Displays some text, gradients and colours and demonstrates button use. You can adjust the brightness with LUX + and -.

### Feature Test With Audio

[feature_test_with_audio.py](feature_test_with_audio.py)

Displays some text, gradients and colours and demonstrates button use. Also demonstrates some of the audio / synth features.
- Button A plays a synth tune
- Button B plays a solo channel of the synth tune
- Button C plays a sinewave (it's frequency can be adjusted with VOL + and -)
- Button D plays a second sinewave (it's frequency can be adjusted with LUX + and -)
- Sleep button stops the sounds

### Fire Effect

[fire_effect.py](fire_effect.py)

A pretty, procedural fire effect. Switch between landscape fire and vertical fire using the A and B buttons! You can adjust the brightness with LUX + and -.

### Lava Lamp

[lava_lamp.py](lava_lamp.py)

A 70s-tastic, procedural rainbow lava lamp. You can adjust the brightness with LUX + and -.

### Light Sensor

[light_sensor.py](light_sensor.py)

Reads data from the on board light sensor and displays the brightness level of the environment. The display is by default set to auto brightness i.e reacts to the brightness of the environment. 
- Button A turns auto brightness off 
- Button B turns auto brightness on

### Nostalgia Prompt

[nostalgia_prompt.py](nostalgia_prompt.py)

A collection of copies of classic terminal styles including C64, MS-DOS, Spectrum, and more. Images and text are drawn pixel by pixel from a pattern of Os and Xs. You can adjust the brightness with LUX + and -.

### Rainbow

[rainbow.py](rainbow.py)

Some good old fashioned rainbows! You can adjust the cycling speed with A and B, stripe width with C and D, hue with VOL + and -, and the brightness with LUX + and -. The sleep button stops the animation (can be started again with A or B).

### Scrolling Text

[scrolling_text.py](scrolling_text.py)

Display scrolling wisdom, quotes or greetz. You can adjust the brightness with LUX + and -.


### Thermometer
[thermometer_pico.py](thermometer_pico.py)

Shows the temperature (from the Pico W's internal sensor) against an appropriately coloured pulsing blob.

### Today

[today.py](today.py)

Calendar example with (optional) NTP synchronization. You can adjust the brightness with LUX + and -, and resync the date by pressing C.

## Wireless Examples

These examples need `WIFI_CONFIG.py` and `network_manager.py` (from the `common` directory) to be saved to your Pico W. Open up `WIFI_CONFIG.py` in Thonny to add your wifi details (and save it when you're done).

- [micropython/examples/common](../../examples/common)

### Cheerlights History

[cheerlights_history.py](cheerlights_history.py)

Updates one pixel every two minutes to display the most recent #Cheerlights colour. Discover the most popular colours over time, or use it as an avant garde (but colourful) 16 hour clock! Find out more about the Cheerlights API at https://cheerlights.com/

You can adjust the brightness with LUX + and -.

### Stellar Paint

[stellar_paint](stellar_paint)

Draw on your Stellar Unicorn from another device in real time, over wifi!

This example needs the `micropython-phew` and `microdot` libraries (you can install these using Thonny's 'Tools > Manage Packages').

### Exchange Ticker

[exchange_ticker.py](exchange_ticker.py)

This example uses the Coinbase open API to collect the current exchange rates of various cryptocurrencies.

Press A to change to a different base exchange currency.

### HTTP Text

[http_text](http_text)

Display scrolling wisdom, quotes or greetz... from another computer or device!

You can adjust the brightness with LUX + and -.

Requires `logging.mpy` and `tinyweb` from [micropython/examples/common](../../examples/common) - copy these into the `lib` folder on your Pico W. You'll also need `index.html` to be saved alongside `html_text.py`.

### Weather

[weather](weather)

Display current weather data from the [Open-Meteo](https://open-meteo.com/) weather API. Make sure to copy across the `icons` folder to your Unicorn.

Buttons:
A - show / hide temperature
B - swap between Celsius and Fahrenheit
C - randomly select a weather icon
D - add rainbows
LUX + and - adjust brightness

## NumPy Examples

[numpy](numpy)

The examples in the folder use `numpy`-like array functions contained in the `ulab` library for super fast graphical effects.

## Other Examples

These examples use additional hardware.

### CO2

[co2.py](co2.py)

Add a [SCD41 sensor breakout](https://shop.pimoroni.com/products/scd41-co2-sensor-breakout) to make an carbon dioxide detector. Press A, B and C to switch between modes.

This example uses a custom tiny bitmap font, find 3x5.bitmapfont in [fonts](../../common/fonts) and copy it to your Pico W.

### Encoder Wheel
[encoder_wheel.py](encoder_wheel.py)

This example uses [RGB Encoder Wheel breakout](https://shop.pimoroni.com/products/rgb-encoder-wheel-breakout) to make an RGB colour picker. Use the encoder wheel to pick a hue and view the RGB breakdown of that colour on the Unicorn display (you can adjust saturation and brightness using the buttons on the breakout too).

### Thermometer (BME280)
[thermometer_bme280.py](thermometer_bme280.py)

Shows temperature, humidity and pressure (from a [BME280 sensor breakout](https://shop.pimoroni.com/products/bme280-breakout)) against an appropriately coloured pulsing blob.

### Thermometer (BME68x)
[thermometer_bme68x.py](thermometer_bme68x.py)

Shows temperature, humidity and pressure (from a [BME680](https://shop.pimoroni.com/products/bme680-breakout) or [BME688](https://shop.pimoroni.com/products/bme688-breakout) sensor breakout) against an appropriately coloured pulsing blob.

### Launch (Demo Reel)

[launch](launch)

If you want to get the demo reel that Stellar Unicorn ships with back, copy the contents of this `launch` folder to your Pico W.
