# Galactic Unicorn MicroPython Examples <!-- omit in toc -->

Galactic Unicorn offers 53x11 bright RGB LEDs driven by Pico W's PIO in addition to a 1W amplifier + speaker, a collection of system and user buttons, and two Qw/ST connectors for adding external sensors and devices. Woha!

- link:[Galactic Unicorn store page](https://shop.pimoroni.com/products/galactic-unicorn)

Galactic Unicorn ships with MicroPython firmware pre-loaded, but you can download the most recent version at the link below (you'll want the  `galactic-unicorn` image).

- [MicroPython releases](https://github.com/pimoroni/pimoroni-pico/releases)
- [Installing MicroPython](../../../setting-up-micropython.md)
  
- [Galactic Unicorn and PicoGraphics](#galactic-unicorn-and-picographics)
- [Examples](#examples)
  - [Clock](#clock)
  - [Eighties Super Computer](#eighties-super-computer)
  - [Feature Test](#feature-test)
  - [Feature Test With Audio](#feature-test-with-audio)
  - [Fire Effect](#fire-effect)
  - [Lava Lamp](#lava-lamp)
  - [Nostalgia Prompt](#nostalgia-prompt)
  - [Rainbow](#rainbow)
  - [Scrolling Text](#scrolling-text)
- [Wireless Examples](#wireless-examples)
  - [Cheerlights Over Time](#cheerlights-over-time)
  - [Galactic Paint](#galactic-paint)

## Galactic Unicorn and PicoGraphics

The easiest way to start displaying stuff on Galactic Unicorn is using our Galactic Unicorn module (which contains a bunch of helpful functions for interacting with the buttons, adjusting brightness and suchlike) and our PicoGraphics library, which is chock full of useful functions for drawing on the LED matrix.

- [Galactic Unicorn function reference](../../modules/picographics/README.md)
- [PicoGraphics function reference](../../modules/galactic_unicorn/README.md)

## Examples

### Clock

[clock.py](clock.py)

Clock example with (optional) NTP synchronization.

### Eighties Super Computer

[eighties_super_computer.py](eighties_super_computer.py)

todo

### Feature Test

[feature_test.py](feature_test.py)

todo

### Feature Test With Audio

[feature_test_with_audio.py](feature_test_with_audio.py)

todo

### Fire Effect

[fire_effect.py](fire_effect.py)

todo

### Lava Lamp

[lava_lamp.py](lava_lamp.py)

todo

### Nostalgia Prompt

[nostalgia_prompt.py](nostalgia_prompt.py)

todo

### Rainbow

[rainbow.py](rainbow.py)

Some good old fashioned rainbows!

### Scrolling Text

[scrolling_text.py](scrolling_text.py)

todo

## Wireless Examples

### Cheerlights Over Time

[cheerlights_over_time.py](cheerlights_over_time.py)

todo

This wireless example needs `network_manager.py` and `WIFI_CONFIG.py` from the `common` directory to be saved to your Pico W. Open up `WIFI_CONFIG.py` in Thonny to add your wifi details (and save it when you're done).

- [micropython/examples/common](../../examples/common)

### Galactic Paint

[galactic_paint](galactic_paint)

todo


