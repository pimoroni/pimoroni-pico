# Pico Explorer MicroPython Examples <!-- omit in toc -->

- [PicoGraphics](#picographics)
- [Non-Wireless Examples](#non-wireless-examples)
  - [Balls Demo](#balls-demo)
  - [Button Test](#button-test)
  - [Demo](#demo)
  - [Noise](#noise)
  - [Rainbow](#rainbow)
  - [Thermometer](#thermometer)
  - [Weather Station (BME280)](#weather-station-bme280)
  - [Weather Station (BME68X)](#weather-station-bme68x)
- [Wireless Examples](#wireless-examples)
  - [Cheerlights](#cheerlights)
- [Other Resources](#other-resources)

## PicoGraphics

You can draw on Pico Explorer's display using our tiny PicoGraphics display library.
- [PicoGraphics MicroPython function reference](../../modules/picographics)

## Non-Wireless Examples

### Balls Demo
[balls_demo.py](balls_demo.py)

LCD demo showing 100 bouncy balls!

### Button Test
[button_test.py](button_test.py)

This example shows you a simple, non-interrupt way of reading Pico Explorer's buttons with a loop that checks to see if buttons are pressed.

### Demo
[demo.py](demo.py)

A demo of all Pico Explorer's functions. Connect analog inputs like potentiometers up to ADC0-ADC3 and motors to the motor channels for best results. Connect AUDIO to GP0 with a jumper wire to hear noise.

### Noise
[noise.py](noise.py)

This example shows you how you can use Pico Explorer's onboard buzzer as a speaker to play different notes and string them together into a bleepy tune (you'll need to connect AUDIO to GP0 with a jumper wire to hear noise).

### Rainbow
[rainbow.py](rainbow.py)

This example borrows a CircuitPython hsv to rgb function to cycle through some rainbows on Pico Explorer's screen.

### Thermometer
[thermometer.py](thermometer.py)

This example takes the temperature from the Pico's onboard temperature sensor, and displays it on Pico Explorer, along with a little pixelly graph.

### Weather Station (BME280)
[weatherstation_BME280.py](weatherstation_BME280.py)

Plug a [BME280 breakout](https://shop.pimoroni.com/products/bme280-breakout) into your Pico Explorer and make a little indoor weather station, with barometer style descriptions.

### Weather Station (BME68X)
[weatherstation_BME68X.py](weatherstation_BME68X.py)

Plug a [BME680](https://shop.pimoroni.com/products/bme680-breakout) or [BME688](https://shop.pimoroni.com/products/bme688-breakout) breakout into your Pico Explorer to make a little indoor weather station, with barometer style descriptions.

## Wireless Examples

The wireless examples need `network_manager.py` and `WIFI_CONFIG.py` from the `common` directory  to be saved to your Pico W. Open up `WIFI_CONFIG.py` in Thonny to add your wifi details (and save it when you're done).

You may also need to install the `micropython-urllib.urequest` library using Thonny's 'Tools' > 'Manage Packages'.

### Cheerlights
[cheerlights.py](cheerlights.py)

This Pico W example sets your Pico Explorer's screen (and optionally, a [RGB LED](https://shop.pimoroni.com/products/led-rgb-clear-common-cathode)) to the current #cheerlights colour.

## Other Resources

Here are some Pico Explorer community projects and resources that you might find useful! Note that code at the links below has not been tested by us and we're not able to offer support with it.

- :link: [PicoGraphics Zoomable Mandelbrot](https://github.com/ExperiMentor/PicoGraphics_Zoomable_Mandelbrot)


