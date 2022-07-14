# Pico Inky Pack MicroPython Examples <!-- omit in toc -->

- [PicoGraphics](#picographics)
- [Non-Wireless Examples](#non-wireless-examples)
  - [Button Test](#button-test)
  - [Clock](#clock)
- [Wireless Examples](#wireless-examples)
  - [Bored API](#bored-api)
  - [Place Kitten](#place-kitten)
  - [Quote of the Day](#quote-of-the-day)

## PicoGraphics

You can draw on Inky Pack's display using our tiny PicoGraphics display library.

- [PicoGraphics MicroPython function reference](../../modules/picographics)

## Non-Wireless Examples

### Button Test
[button_test.py](button_test.py)

Shows a simple way of reading the buttons and drawing on the screen.

### Clock
[clock.py](clock.py)

Demonstrates a fast update speed.

## Wireless Examples

The wireless examples need `network_manager.py` and `WIFI_CONFIG.py` from the `common` directory to be saved to your Pico W. Open up `WIFI_CONFIG.py` in Thonny to add your wifi details (and save it when you're done).

You'll also need to install the `micropython-urllib.urequest` library using Thonny's 'Tools' > 'Manage Packages'.

### Bored API
[bored_api.py](bored_api.py)

Simple demo to get a random activity from BoredAPI.com

### Place Kitten
[placekitten.py](placekitten.py)

Grab a random image from PlaceKitten.com and display it on Inky Pack.

### Quote of the Day
[quote_of_the_day.py](quote_of_the_day.py)

Grab the quote of the day from Wikipedia.
