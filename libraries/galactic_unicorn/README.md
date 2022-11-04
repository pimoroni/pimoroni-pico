# Galactic Unicorn (C/C++)<!-- omit in toc -->

Galactic Unicorn offers 53x11 bright RGB LEDs driven by Pico W's PIO in addition to a 1W amplifier + speaker, a collection of system and user buttons, and two Qw/ST connectors for adding external sensors and devices. Woha!

You can buy one here: https://shop.pimoroni.com/products/galactic-unicorn

## These are not your everyday RGB LEDs!

Internally Galactic Unicorn applies gamma correction to the supplied image data and updates the display with 14-bit precision resulting in extremely linear visual output - including at the low end.

The display is refreshed around 300 times per second (300fps!) allowing for rock solid stability even when being filmed, no smearing or flickering even when in motion.

No strobing or brightness stepping here folks - it's the perfect backdrop for your tricked out streaming setup!

## Getting started

The Galactic Unicorn library provides a collection of methods that allow you to easily access all of the features on the board.

Drawing is primarily handled via our [PicoGraphics](https://github.com/pimoroni/pimoroni-pico/tree/main/libraries/pico_graphics) library which provides a comprehensive selection of drawing methods - once your drawing work is complete you pass the PicoGraphics object to Galactic Unicorn to have it displayed on the screen.

- [Example Program](#example-program)
- [Interleaved Framebuffer](#interleaved-framebuffer)
- [Function Reference](#function-reference)
  - [System State](#system-state)
    - [`void init()`](#void-init)
    - [`void set_brightness(float value)`](#void-set_brightnessfloat-value)
    - [`float get_brightness()`](#float-get_brightness)
    - [`void adjust_brightness(float delta)`](#void-adjust_brightnessfloat-delta)
    - [`void set_volume(float value)`](#void-set_volumefloat-value)
    - [`float get_volume()`](#float-get_volume)
    - [`void adjust_volume(float delta)`](#void-adjust_volumefloat-delta)
    - [`uint16_t light()`](#uint16_t-light)
    - [`bool is_pressed(uint8_t button)`](#bool-is_presseduint8_t-button)
  - [Drawing](#drawing)
    - [`void update(PicoGraphics *graphics)`](#void-updatepicographics-graphics)
    - [`void clear()`](#void-clear)
    - [`void set_pixel(int x, int y, uint8_t r, uint8_t g, uint8_t b)`](#void-set_pixelint-x-int-y-uint8_t-r-uint8_t-g-uint8_t-b)
  - [Audio](#audio)
    - [`void play_sample(uint8_t *data, uint32_t length)`](#void-play_sampleuint8_t-data-uint32_t-length)
    - [`AudioChannel& synth_channel(uint channel)`](#audiochannel-synth_channeluint-channel)
    - [`void play_synth()`](#void-play_synth)
    - [`void stop_playing()`](#void-stop_playing)
  - [Constants](#constants)
    - [`WIDTH` & `HEIGHT`](#width--height)

# Example Program

The following example shows how to scroll a simple message across the display.

```c++
#include <stdio.h>
#include <stdlib.h>

#include "libraries/pico_graphics/pico_graphics.hpp"
#include "galactic_unicorn.hpp"

using namespace pimoroni;

// create a PicoGraphics framebuffer to draw into
PicoGraphics_PenRGB888 graphics(GalacticUnicorn::WIDTH, GalacticUnicorn::HEIGHT, nullptr);

// create our GalacticUnicorn object
GalacticUnicorn galactic_unicorn;

// message to scroll
std::string message = "Pirate. Monkey. Robot. Ninja.";

int main() {

  stdio_init_all();

  // initialise the GalacticUnicorn object
  galactic_unicorn.init();

  // start position for scrolling (off the side of the display)
  float scroll = -(float)GalacticUnicorn::WIDTH;

  while(true) {
    // determine the scroll position of the text
    int width = graphics.measure_text(message, 1);
    scroll += 0.25f;
    if(scroll > width) {
      scroll = -(float)GalacticUnicorn::WIDTH;
    }

    // clear the graphics object
    graphics.set_pen(0, 0, 0);
    graphics.clear();

    // draw the text
    graphics.set_pen(255, 255, 0); // a nice yellow
    graphics.text(message, Point(0 - scroll, 5), -1, 0.55);    

    // update the display
    galactic_unicorn.update(&graphics);

    sleep_ms(10);
  }

  return 0;
}
```

# Interleaved Framebuffer

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

## System State

### `void init()`

Initialise the Galactic Unicorn hardware, interleaved framebuffer, and PIO programs. This function must be called before attempting to do anything else with Galactic Unicorn.

### `void set_brightness(float value)`

Set the brightness - `value` is supplied as a floating point value between `0.0` and `1.0`.

### `float get_brightness()`

Returns the current brightness as a value between `0.0` to `1.0`.

### `void adjust_brightness(float delta)`

Adjust the brightness of the display - `delta` is supplied as a floating point value and will be added to the current brightness (and then clamped to the range `0.0` to `1.0`).

For example:

```c++
galactic.set_brightness(0.5f);
galactic.adjust_brightness(0.1f); // brightness is now 0.6
galactic.adjust_brightness(0.7f); // brightness is now 1.0
galactic.adjust_brightness(-0.2f); // brightness is now 0.8
```

### `void set_volume(float value)`

Set the volume - `value` is supplied as a floating point value between `0.0` and `1.0`.

### `float get_volume()`

Returns the current volume as a value between `0.0` and `1.0`.

### `void adjust_volume(float delta)`

Adjust the volume - `delta` is supplied as a floating point value and will be added to the current volume (and then clamped to the range `0.0` to `1.0`).

For example:

```c++
galactic.set_volume(0.5f);
galactic.adjust_volume(0.1f); // volume is now 0.6
galactic.adjust_volume(0.7f); // volume is now 1.0
galactic.adjust_volume(-0.2f); // volume is now 0.8
```

### `uint16_t light()`

Get the current value seen by the onboard light sensor as a value between `0` and `4095`.

### `bool is_pressed(uint8_t button)`

Returns true if the requested `button` is currently pressed.

There are a set of constants on the GalacticUnicorn class that represent each of the buttons. The brightness, sleep, and volume buttons are not tied to hardware functions (they are implemented entirely in software) so can also be used for user functions if preferred.

```c++
static const uint8_t SWITCH_A               =  0;
static const uint8_t SWITCH_B               =  1;
static const uint8_t SWITCH_C               =  3;
static const uint8_t SWITCH_D               =  6;
static const uint8_t SWITCH_SLEEP           = 27;
static const uint8_t SWITCH_VOLUME_UP       =  7;
static const uint8_t SWITCH_VOLUME_DOWN     =  8;
static const uint8_t SWITCH_BRIGHTNESS_UP   = 21;
static const uint8_t SWITCH_BRIGHTNESS_DOWN = 26;
```

For example:

```c++
while(!galactic.is_pressed(GalacticUnicorn::SWITCH_A)) {
  // wait for switch A to be pressed
}
printf("We did it! We pressed switch A! Heck yeah!");
```

## Drawing

### `void update(PicoGraphics *graphics)`

**This is our recommended way to update the image on Galactic Unicorn.** The PicoGraphics library provides a collection of powerful drawing methods to make things simple.

The image on the PicoGraphics object provided is copied to the interleaved framebuffer with gamma correction applied. This lets you have multiple PicoGraphics objects on the go at once and switch between them by changing which gets passed into this function.

If however you'd rather twiddle individual pixels (for example you're producing some sort of algorithmic output) then you can simply use the `clear()` and `set_pixel()` methods mentioned below.

### `void clear()`

Clear the contents of the interleaved framebuffer. This will make your Galactic Unicorn display turn off when the next frame is displayed.

If you're using PicoGraphics to build your image (recommended!) then you won't need to call this method as you'll overwrite the entire display when you call `update()` anyway.

### `void set_pixel(int x, int y, uint8_t r, uint8_t g, uint8_t b)`

Set a single pixel to the specified colour. The newly set colour will be shown at the next frame. Pixel coordinates go from `0` to `52` along the `x` axis and from `0` to `10` on the `y` axis. Colour values are specified as a `0` to `255` RGB triplet - the supplied colour will be gamma corrected automatically.

 When drawing a full image it's recommended that you keep the time between each `set_pixel` call short to ensure your image gets displayed on the next frame. Otherwise you can get scanning-like visual artefacts (unless that is your intention of course!)

## Audio

Audio functionality is supported by our [PicoSynth library](https://github.com/pimoroni/pimoroni-pico/tree/main/libraries/pico_synth) which allows you to create multiple voice channels with ADSR (attack decay sustain release) envelopes. It provides a similar set of functionality to the classic SID chip in the Commodore 64.

### `void play_sample(uint8_t *data, uint32_t length)`

Play the provided 16-bit audio sample. `data` must point to a buffer that contains 16-bit PCM data and `length` must be the number of samples.

### `AudioChannel& synth_channel(uint channel)`

Gets an `AudioChannel` object which can then be configured with voice, ADSR envelope, etc.

### `void play_synth()`

Start the synth playing.

### `void stop_playing()`

Stops any currently playing audio.

## Constants

### `WIDTH` & `HEIGHT`

The width and height of Galactic Unicorn are available in constants `WIDTH` and `HEIGHT`.

For example:

```c++
int num_pixels = GalacticUnicorn::WIDTH * GalacticUnicorn::HEIGHT;
```