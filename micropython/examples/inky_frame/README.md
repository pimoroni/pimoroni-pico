# Pico Inky Frame MicroPython Examples <!-- omit in toc -->

- [PicoGraphics](#picographics)
- [Examples](#examples)
  - [Button Test](#button-test)
  - [Daily Activity](#daily-activity)
  - [Image Gallery](#image-gallery)
  - [LED PWM](#led-pwm)
  - [News](#news)
  - [PlaceKitten](#placekitten)
  - [Quote of the Day](#quote-of-the-day)
  - [Random Joke](#random-joke)
  - [SD Card Test](#sd-card-test)
  - [XKCD Daily](#xkcd-daily)

## PicoGraphics

You can draw on Inky Frame using our tiny PicoGraphics display library.
- [PicoGraphics MicroPython function reference](../../modules/picographics)

## Examples

The wireless examples need `network_manager.py` and `WIFI_CONFIG.py` from the `common` directory to be saved to your Pico W. Open up `WIFI_CONFIG.py` in Thonny to add your wifi details (and save it when you're done).

You'll also need to install the `micropython-urllib.urequest` library using Thonny's 'Tools' > 'Manage Packages' or `common/lib/urllib` which contains a compiled `.mpy` version that uses less RAM. You should place this directory in `lib` on your Pico W.

Finally for examples loading images, you'll need `sdcard.mpy` from `common/lib`. You should place this file in `lib` on your Pico W.

- [/micropython/examples/common](../common)

### Button Test
[button_test.py](button_test.py)

Inky Frame's buttons (and the RTC alarm, busy signal from the screen and external trigger from the hack header) are connected to a shift register to help conserve pins, and to allow these inputs to wake the board up from sleep.

This example demonstrates a simple way of reading when a button has been pushed by reading the shift register and checking if the bit in a specific position is 0 or 1.

### Daily Activity
[inky_frame_daily_activity.py](inky_frame_daily_activity.py)

Generate a random activity from Bored API.

### Image Gallery
[/image_gallery](../inky_frame/image_gallery)

This photo frame example displays local images on Inky Frame and lets you switch between them with the buttons. Use `image_gallery.py` if your images are stored on your Pico, or `image_gallery_sd.py` if the images are on your SD card.

### LED PWM
[led_pwm.py](led_pwm.py)

A basic example showing how you can control the brightness of Inky Frame's LEDs using PWM.

### News
[inky_frame_news.py](inky_frame_news.py)

Display headlines from BBC News.

### PlaceKitten
[inky_frame_placekitten.py](inky_frame_placekitten.py)

Download a random (from a small subset) image from PlaceKitten.

### Quote of the Day
[inky_frame_quote_of_the_day.py](inky_frame_quote_of_the_day.py)

Load the WikiQuotes Quote of the Day and display it.

### Random Joke
[inky_frame_random_joke.py](inky_frame_random_joke.py)

Load a random joke from JokeAPI.dev and display it.

Jokes are rendered into images "offline" by our feed2image service for two reasons:

1. Saves the Pico W having to process them
2. JokeAPI.dev needs TLS1.3 which Pico W does not support!

For bugs/contributions or to complain about a joke, see: https://github.com/pimoroni/feed2image

### SD Card Test
[sd_test.py](sd_test.py)

This simple example shows how to read and write from the SD card on Inky Frame.

### XKCD Daily
[inky_frame_xkcd_daily.py](inky_frame_xkcd_daily.py)

Download and display the daily webcomic from https://xkcd.com/

The webcomic is rendered "offline" by our feed2image service since xkcd.com requires TLS1.3!

For bugs/contributions see: https://github.com/pimoroni/feed2image

