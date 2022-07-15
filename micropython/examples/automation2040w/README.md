# Automation 2040 W Micropython Examples <!-- omit in toc -->

- [Function Examples](#function-examples)
  - [Read ADCs](#read-adcs)
  - [Read Inputs](#read-inputs)
  - [Toggle Relays](#toggle-relays)
  - [Toggle Outputs](#toggle-outputs)
  - [Switches and LEDs](#switches-and-leds)
  - [Reset Automation](#reset-automation)
- [Wireless Examples](#wireless-examples)
  - [Web IO Interface](#web-io-interface)


## Function Examples

### Read ADCs
[read_adcs.py](read_adcs.py)

Shows how to read the 3 ADC terminals of Automation 2040 W.


### Read Inputs
[read_inputs.py](read_inputs.py)

Shows how to read the 3 Input terminals of Automation 2040 W.


### Toggle Relays
[toggle_relays.py](toggle_relays.py)

Demonstrates how to toggle the actuation state of each of Automation 2040 W's relays.


### Toggle Outputs
[toggle_outputs.py](toggle_outputs.py)

Demonstrates how to toggle each of Automation 2040 W's output terminals.


### Switches and LEDs
[switches_and_leds.py](switches_and_leds.py)

An example of the user switches and LEDs on Automation 2040 W.


### Reset Automation
[reset_automation.py](reset_automation.py)

A simple program that resets Automation 2040 W, turning off its Relays, Outputs, and LEDs.

## Wireless Examples

The wireless examples need `network_manager.py` and `WIFI_CONFIG.py` from the `common` directory to be saved to your Pico W. Open up `WIFI_CONFIG.py` in Thonny to add your wifi details (and save it when you're done).

### Web IO Interface
[web_io_interface/](web_io_interface/)

Provides a basic web interface for all your Automation 2040W features.

Needs `lib/tinyweb` from `common`!
