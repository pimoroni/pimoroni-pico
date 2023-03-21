# Badger 2040 W MicroPython Examples <!-- omit in toc -->

:warning: This code has been deprecated in favour of a dedicated Badger 2040 project: https://github.com/pimoroni/badger2040
 
- [About Badger 2040 W](#about-badger-2040-w)
- [Badger 2040 W and PicoGraphics](#badger-2040-w-and-picographics)
- [Examples](#examples)
  - [Badge](#badge)
  - [Clock](#clock)
  - [Ebook](#ebook)
  - [Fonts](#fonts)
  - [Help](#help)
  - [Image](#image)
  - [Info](#info)
  - [List](#list)
  - [Net Info](#net-info)
  - [News](#news)
  - [Qrgen](#qrgen)
  - [Weather](#weather)
- [Other Resources](#other-resources)


## About Badger 2040 W

Badger 2040 W is a programmable E Paper/eInk/EPD badge with 2.4GHz wireless connectivity, powered by Raspberry Pi Pico W. It can go into a deep sleep mode between updates to preserve battery.

- :link: [Badger 2040 W store page](https://shop.pimoroni.com/products/badger-2040-w)

Badger 2040 W ships with MicroPython firmware pre-loaded, but you can download the most recent version at the link below (you'll want the  `pimoroni-badger2040w` .uf2). If you download the `-with-examples` file, it will come with examples built in.

- [MicroPython releases](https://github.com/pimoroni/pimoroni-pico/releases)
- [Installing MicroPython](../../../setting-up-micropython.md)

## Badger 2040 W and PicoGraphics

The easiest way to start displaying cool stuff on Badger is by using our `badger2040w` module (which contains helpful functions for interacting with the board hardware) and our PicoGraphics library (which contains a bunch of functions for drawing on the E Ink display).

- [Badger 2040 W function reference](../../modules/badger2040w/README.md)
- [PicoGraphics function reference](../../modules/picographics/README.md)

## Examples

Find out more about how to use these examples in our Learn guide:

- [Getting Started with Badger 2040 W](https://learn.pimoroni.com/article/getting-started-with-badger-2040-w)

### Badge
[badge.py](examples/badge.py)

Customisable name badge example.

### Clock
[clock.py](examples/clock.py)

Clock example with (optional) NTP synchronization and partial screen updates. 

### Ebook
[ebook.py](examples/ebook.py)

View text files on Badger.

### Fonts
[fonts.py](examples/fonts.py)

View all the built in fonts.

### Help
[help.py](examples/help.py)

How to navigate the launcher.

### Image
[image.py](examples/image.py)

Display .jpegs on Badger.

### Info
[info.py](examples/info.py)

Info about Badger 2040 W.

### List
[list.py](examples/list.py)

A checklist to keep track of to-dos or shopping.

### Net Info
[net_info.py](examples/net_info.py)

Show IP address and other wireless connection details.

### News
[news.py](examples/news.py)

View BBC news headlines.

### Qrgen
[qrgen.py](examples/qrgen.py)

Display QR codes and associated text.

### Weather
[weather.py](examples/weather.py)

Display current weather data from the [Open-Meteo weather API](https://open-meteo.com/)

## Other Resources

Here are some cool Badger 2040 W community projects and resources that you might find useful / inspirational! Note that code at the links below has not been tested by us and we're not able to offer support with it.

- :link: [Send messages to Badger via webform](https://github.com/techcree/Badger2040W/tree/main/webform)
- :link: [3D printed Badger 2040 W enclosure](https://kaenner.de/badger2040w)
- :link: [Badger Pixel Client for a Raspberry Pi Pixel Server](https://github.com/penguintutor/badger-pixel-client)