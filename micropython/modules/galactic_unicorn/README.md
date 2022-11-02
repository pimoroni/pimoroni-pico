# Galactic Unicorn (MicroPython) <!-- omit in toc -->

Galactic Unicorn offers 53x11 bright RGB LEDs driven by Pico W's PIO in addition to a 1W amplifier + speaker, a collection of system and user buttons, and two Qw/ST connectors for adding external sensors and devices. Woha!

## These are not your everyday RGB LEDs!

Internally Galactic Unicorn applies gamma correction to the supplied image data and updates the display with 14-bit precision resulting in extremely linear visual output - including at the low end.

The display is refreshed around 300 times per second (300fps!) allowing for rock solid stability even when being filmed, no smearing or flickering even when in motion.

No strobing or brightness stepping here folks - it's the perfect backdrop for your tricked out streaming setup!

## Getting started

The Galactic Unicorn library provides a collection of methods that allow you to easily access all of the features on the board.

Drawing is primarily handled via our [PicoGraphics](https://github.com/pimoroni/pimoroni-pico/tree/main/micropython/modules/picographics) library which provides a comprehensive selection of drawing methods - once your drawing work is complete you pass the PicoGraphics object to Galactic Unicorn to have it displayed on the screen.

- [Example Program](#example-program)
- [Interleaved framebuffer](#interleaved-framebuffer)
- [Function Reference](#function-reference)
  - [Imports and objects](#imports-and-objects)
  - [System state](#system-state)
    - [`set_brightness(value)`](#set_brightnessvalue)
    - [`get_brightness()`](#get_brightness)
    - [`adjust_brightness(delta)`](#adjust_brightnessdelta)
    - [`set_volume(value)`](#set_volumevalue)
    - [`get_volume()`](#get_volume)
    - [`adjust_volume(delta)`](#adjust_volumedelta)
    - [`light()`](#light)
    - [`is_pressed(button)`](#is_pressedbutton)
  - [Drawing](#drawing)
    - [`update(PicoGraphics)`](#updatepicographics)
  - [Audio](#audio)
    - [`play_sample(data, length)`](#play_sampledata-length)
    - [`synth_channel(channel)`](#synth_channelchannel)
    - [`play_synth()`](#play_synth)
    - [`stop_playing()`](#stop_playing)
  - [Constants](#constants)
    - [`WIDTH` & `HEIGHT`](#width--height)

# Example Program

The following example shows how to scroll a simple message across the display.

```python
from galactic import GalacticUnicorn
from picographics import PicoGraphics, DISPLAY_GALACTIC_UNICORN
import time

# create a PicoGraphics framebuffer to draw into
graphics = PicoGraphics(display=DISPLAY_GALACTIC_UNICORN)

# create our GalacticUnicorn object
gu = GalacticUnicorn()

# start position for scrolling (off the side of the display)
scroll = -53.0

# message to scroll
MESSAGE = "Pirate. Monkey. Robot. Ninja."

# pen colours to draw with
BLACK = graphics.create_pen(0, 0, 0)
YELLOW = graphics.create_pen(255, 255, 0)

while True:
    # determine the scroll position of the text
    width = graphics.measure_text(MESSAGE, 1)
    scroll += 0.25
    if scroll > width:
      scroll = -53.0

    # clear the graphics object
    graphics.set_pen(BLACK)
    graphics.clear()

    # draw the text
    graphics.set_pen(YELLOW)
    graphics.text(MESSAGE, round(0 - scroll), 2, -1, 0.55);    

    # update the display
    gu.update(graphics)

    time.sleep(0.02)
```

# Interleaved framebuffer

Galactic Unicorn takes advantage of the RP2040's PIOs to drive screen updates - this is what gives it the performance it needs to render with 14-bit precision at over 300 frames per second.

The PIO is a powerful, but limited, tool. It has no way to access memory at random and minimal support for decision making and branching. All it can really do is process a stream of data/instructions in order. 

This means that we need to be clever about the way we pass data into the PIO program, the information needs to be delivered in the exact order that the PIO will need to process it. To achieve this we "interleave" our framebuffer - each frame of BCM data is passed one after another with values for the current row, pixel count, and timing inserted as needed:

    row 0 data:
      for each bcd frame:
        bit    : data
              0: 00110110                           // row pixel count (minus one)
        1  - 53: xxxxxbgr, xxxxxbgr, xxxxxbgr, ...  // pixel data
        54 - 55: xxxxxxxx, xxxxxxxx                 // dummy bytes to dword align
             56: xxxxrrrr                           // row select bits
        57 - 59: tttttttt, tttttttt, tttttttt       // bcd tick count (0-65536)
    
    row 1 data:
      ...

If you're working with our library then you don't need to worry about any of these details, they are handled for you.

# Function Reference

## Imports and objects

To access these functions, you'll need to first `import` the relevant libraries and then set up a Galactic Unicorn object:

```python
from galactic import GalacticUnicorn

gu = GalacticUnicorn()
```

or (with PicoGraphics):

```python
from galactic import GalacticUnicorn
from picographics import PicoGraphics, DISPLAY_GALACTIC_UNICORN

gu = GalacticUnicorn()
graphics = PicoGraphics(display=DISPLAY_GALACTIC_UNICORN)
```

## System state

### `set_brightness(value)`

Set the brightness - `value` is supplied as a floating point value between `0..1`.

### `get_brightness()`

Returns the current brightness as a value between `0..1`.

### `adjust_brightness(delta)`

Adjust the brightness of the display - `delta` is supplied as a floating point value and will be added to the current brightness (and then clamped to the range `0..1`).

For example:

```python
gu.set_brightness(0.5)
gu.adjust_brightness(0.1)  # brightness is now 0.6
gu.adjust_brightness(0.7)  # brightness is now 1.0
gu.adjust_brightness(-0.2)  # brightness is now 0.8
```

### `set_volume(value)`

Set the volume - `value` is supplied as a floating point value between `0..1`.

### `get_volume()`

Returns the current volume as a value between `0..1`.

### `adjust_volume(delta)`

Adjust the volume - `delta` is supplied as a floating point value and will be added to the current volume (and then clamped to the range `0..1`).

For example:

```python
gu.set_volume(0.5)
gu.set_volume(0.1)  # volume is now 0.6
gu.adjust_volume(0.7)  # volume is now 1.0
gu.adjust_volume(-0.2)  # volume is now 0.8
```

### `light()`

Get the current value seen by the onboard light sensor as a value between `0...4096`.

### `is_pressed(button)`

Returns true if the requested `button` is currently pressed.

There are a set of constants in the GalacticUnicorn class that represent each of the buttons. The brightness, sleep, and volume buttons are not tied to hardware functions (they are implemented entirely in software) so can also be used for user functions if preferred. Here's a list of the constants and their associated pin numbers:

```python
SWITCH_A               =  0
SWITCH_B               =  1
SWITCH_C               =  3
SWITCH_D               =  6
SWITCH_SLEEP           = 27
SWITCH_VOLUME_UP       =  7
SWITCH_VOLUME_DOWN     =  8
SWITCH_BRIGHTNESS_UP   = 21
SWITCH_BRIGHTNESS_DOWN = 26
```

For example:

```python
while gu.is_pressed(gu.SWITCH_A):
    # wait for switch A to be pressed
    print("We did it! We pressed switch A! Heck yeah!")
```

## Drawing

### `update(PicoGraphics)`

**This is our recommended way to update the image on Galactic Unicorn.** The PicoGraphics library provides a collection of powerful drawing methods to make things simple.

The image on the PicoGraphics object provided is copied to the interleaved framebuffer with gamma correction applied.

For example (assuming you've set up your Galactic Unicorn and PicoGraphics objects up as [we did above](#imports-and-objects)):

```python
  gu.update(graphics)
```

⚠️ If you've used PicoGraphics on our other boards note that this `update` function works a little differently  (here it's a Galactic Unicorn function to which you need to pass a PicoGraphics object).

## Audio

Audio functionality is supported by our [PicoSynth library](https://github.com/pimoroni/pimoroni-pico/tree/main/libraries/pico_synth) which allows you to create multiple voice channels with ADSR envelopes. It provides a similar set of functionality to the classic SID chip in the Commodore 64.

### `play_sample(data, length)`

Play the provided 16-bit audio sample. `data` must point to a buffer that contains 16-bit PCM data and `length` must be the number of samples.

### `synth_channel(channel)`

Gets an `AudioChannel` object which can then be configured with voice, ADSR envelope, etc.

### `play_synth()`

Start the synth playing.

### `stop_playing()`

Stops any currently playing audio.

## Constants

### `WIDTH` & `HEIGHT`

The width and height of Galactic Unicorn are available in constants `WIDTH` and `HEIGHT`.

For example:

```python
num_pixels = gu.WIDTH * gu.HEIGHT
print(num_pixels)
```