# Galactic Unicorn MicroPython Examples <!-- omit in toc -->
 
- [About Galactic Unicorn](#about-galactic-unicorn)
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
  - [Cheerlights History](#cheerlights-history)
  - [Galactic Paint](#galactic-paint)
- [Other Examples](#other-examples)
  - [Launch (Demo Reel)](#launch-demo-reel)

## About Galactic Unicorn

Galactic Unicorn offers 53x11 bright RGB LEDs driven by Pico W's PIO in addition to a 1W amplifier + speaker, a collection of system and user buttons, and two Qw/ST connectors for adding external sensors and devices. Woha!

- :link: [Galactic Unicorn store page](https://shop.pimoroni.com/products/galactic-unicorn)

Galactic Unicorn ships with MicroPython firmware pre-loaded, but you can download the most recent version at the link below (you'll want the  `galactic-unicorn` image).

- [MicroPython releases](https://github.com/pimoroni/pimoroni-pico/releases)
- [Installing MicroPython](../../../setting-up-micropython.md)

## Galactic Unicorn and PicoGraphics

The easiest way to start displaying cool stuff on Galactic Unicorn is using our Galactic Unicorn module (which contains a bunch of helpful functions for interacting with the buttons, adjusting brightness and suchlike) and our PicoGraphics library, which is chock full of useful functions for drawing on the LED matrix.

- [Galactic Unicorn function reference](../../modules/galactic_unicorn/README.md)
- [PicoGraphics function reference](../../modules/picographics/README.md)

## Examples

### Clock

[clock.py](clock.py)

Clock example with (optional) NTP synchronization. You can adjust the brightness with LUX + and -, and resync the time by pressing A.

### Eighties Super Computer

[eighties_super_computer.py](eighties_super_computer.py)

Random LEDs blink on and off mimicing the look of a movie super computer doing its work in the eighties. You can adjust the brightness with LUX + and -.

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

### Nostalgia Prompt

[nostalgia_prompt.py](nostalgia_prompt.py)

A collection of copies of classic terminal styles including C64, MS-DOS, Spectrum, and more. Images and text are drawn pixel by pixel from a pattern of Os and Xs. You can adjust the brightness with LUX + and -.

### Rainbow

[rainbow.py](rainbow.py)

Some good old fashioned rainbows! You can adjust the cycling speed with A and B, stripe width with C and D, hue with VOL + and -, and the brightness with LUX + and -. The sleep button stops the animation (can be started again with A or B).

### Scrolling Text

[scrolling_text.py](scrolling_text.py)

Display scrolling wisdom, quotes or greetz. You can adjust the brightness with LUX + and -.

## Wireless Examples

These examples need `WIFI_CONFIG.py` (from the `common` directory) to be saved to your Pico W. Open up `WIFI_CONFIG.py` in Thonny to add your wifi details (and save it when you're done).

- [micropython/examples/common](../../examples/common)

### Cheerlights History

[cheerlights_history.py](cheerlights_history.py)

Updates one pixel every five minutes to display the most recent #Cheerlights colour. Discover the most popular colours over time, or use it as an avant garde (but colourful) 53 hour clock! Find out more about the Cheerlights API at https://cheerlights.com/

Requires `WIFI_CONFIG.py` and `network_manager.py` from the `common` directory.

You can adjust the brightness with LUX + and -.

### Galactic Paint

[galactic_paint](galactic_paint)

Draw on your Galactic Unicorn from another device in real time, over wifi!

Requires `WIFI_CONFIG.py` from the `common` directory. It also needs the `micropython-phew` and `microdot` libraries (you can install these using Thonny's 'Tools > Manage Packages').

## Other Examples

### Launch (Demo Reel)

[launch](launch)

If you want to get the demo reel that Galactic Unicorn ships with back, copy the contents of this `launch` folder to your Pico W.

