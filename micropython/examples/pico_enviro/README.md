# Pico Enviro+ MicroPython Examples <!-- omit in toc -->

- [PicoGraphics](#picographics)
- [BME688](#bme688)
- [PMS5003 Particulate Sensor](#pms5003-particulate-sensor)
- [Non-Wireless Examples](#non-wireless-examples)
  - [BME688 sensor](#bme688-sensor)
  - [Button Test](#button-test)
  - [Enviro All](#enviro-all)
  - [Enviro All (Basic)](#enviro-all-basic)
  - [LCD](#lcd)
  - [Light](#light)
  - [Mic](#mic)
  - [Particle](#particle)
- [Wireless Examples](#wireless-examples)
  - [Enviro All (MQTT)](#enviro-all-mqtt)

## PicoGraphics

You can draw on Pico Enviro+'s display using our tiny PicoGraphics display library.
- [PicoGraphics MicroPython function reference](../../modules/picographics)

## BME688

The 4-in-1 BME688 environmental sensor on Pico Enviro+ measures temperature, humidity, pressure and gas.
- [BME68X MicroPython function reference](../breakout_bme68x)

## PMS5003 Particulate Sensor

The examples that use the [optional particulate sensor](https://shop.pimoroni.com/products/pms5003-particulate-matter-sensor-with-cable) require separate drivers - you can install them from PyPi by searching for 'pms5003-micropython' in Thonny's 'Tools > Manage Packages'

- [PMS5003 MicroPython library](https://github.com/pimoroni/pms5003-micropython)

## Non-Wireless Examples

### BME688 sensor
[bme688_sensor.py](bme688_sensor.py)

Reads from the BME688 environmental sensor and shows temperature, humidity and pressure on the display.

### Button Test
[button_test.py](button_test.py)

This example shows you a simple, non-interrupt way of reading Pico Enviro+'s buttons with a loop that checks to see if buttons are pressed.

### Enviro All
[enviro_all.py](enviro_all.py)

An advanced example that reads from all the sensors on Pico Enviro+ (including the particulate sensor) and displays the results on screen. Press A and B to turn the backlight on and off, and press X and Y to switch between sensor mode and graphic equaliser mode!

### Enviro All (Basic)
[enviro_all_basic.py](enviro_all_basic.py)

This basic example shows how to read from all the sensors on Pico Enviro+ (plus the optional particulate sensor). Prints results to the REPL only.

### LCD
[lcd.py](lcd.py)

LCD demo with bouncy balls.

### Light
[light.py](light.py)

Reads the light/proximity sensors and displays the results on screen.

### Mic
[mic.py](mic.py)

Reads the microphone and displays a waveform on screen.

### Particle
[particle.py](particle.py)

Reads from the (optional) particulate sensor and displays the results on screen.

## Wireless Examples

The wireless examples need `network_manager.py` and `WIFI_CONFIG.py` from the [micropython/examples/common](/micropython/examples/common) directory to be saved to your Pico W. Open up `WIFI_CONFIG.py` in Thonny to add your wifi details (and save it when you're done).

For MQTT, you'll need to install `micropython-mqtt.simple` using Thonny's 'Tools' > 'Manage Packages'.

### Enviro All (MQTT)
[enviro_all_mqtt.py](enviro_all_mqtt.py)

This example reads from all the sensors on Pico Enviro+ (plus the optional particulate sensor) and posts the results into an MQTT broker.
