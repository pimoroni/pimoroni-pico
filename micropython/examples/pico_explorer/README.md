# Pico Explorer MicroPython Examples <!-- omit in toc -->

## PicoGraphics

You can draw on Pico Explorer's display using our tiny PicoGraphics display library.

- [PicoGraphics MicroPython function reference](../../modules/picographics)

## Non-Wireless Examples

### Balls Demo

Simple LCD demo showing 100 bouncy balls!

### Button Test

This example shows you a simple, non-interrupt way of reading Pico Explorer's buttons with a loop that checks to see if buttons are pressed.

### Demo

A demo of all Pico Explorer's functions. Connect analog inputs like potentiometers up to ADC0-ADC3 and motors to the motor channels for best results. Connect AUDIO to GP0 with a jumper wire to hear noise.

### Noise

This example shows you how you can use Pico Explorer's onboard buzzer as a speaker to play different notes and string them together into a bleepy tune (you'll need to connect AUDIO to GP0 with a jumper wire to hear noise).

### Rainbow

This example borrows a CircuitPython hsv to rgb function to cycle through some rainbows on Pico Explorer's screen.

### Thermometer

This example takes the temperature from the Pico's onboard temperature sensor, and displays it on Pico Explorer, along with a little pixelly graph.

### Weather Station (BME280)

Plug a BME280 breakout into your Pico Explorer and make a little indoor weather station, with barometer style descriptions.

### Weather Station (BME68x)

Plug a BME680 or BME688 breakout into your Pico Explorer to make a little indoor weather station, with barometer style descriptions.

## Wireless Examples

The wireless examples need `network_manager.py` and `WIFI_CONFIG.py` to be saved to your Pico W. Open up `WIFI_CONFIG.py` in Thonny to add your wifi details (and save it when you're done).

You'll also need to install the `micropython-urllib.urequest` library using Thonny's 'Tools' > 'Manage Packages'.

### Cheerlights

This Pico W example sets your Pico Explorer's screen to the current #cheerlights colour.