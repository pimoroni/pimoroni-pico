# Badger 2040 <!-- omit in toc -->

Badger 2040 is an RP2040 powered E Ink badge.

:warning: This code has been deprecated in favour of a dedicated Badger 2040 project: https://github.com/pimoroni/badger2040

- [Summary](#summary)
  - [Getting Started](#getting-started)
  - [Update Speed](#update-speed)
  - [Buttons](#buttons)
  - [System speed](#system-speed)
  - [Other Functions](#other-functions)
  - [Other Constants](#other-constants)
    - [Screen Size](#screen-size)
    - [E Ink Pins](#e-ink-pins)
    - [Power Pins](#power-pins)
    - [Activity LED Pin](#activity-led-pin)
- [Function Reference](#function-reference)
  - [Basic Drawing Settings](#basic-drawing-settings)
    - [Pen Colour](#pen-colour)
    - [Pen Thickness](#pen-thickness)
  - [Text](#text)
    - [Draw Text](#draw-text)
    - [Measure Text](#measure-text)
    - [Change Font](#change-font)
  - [Lines, Pixels \& Rectangles](#lines-pixels--rectangles)
    - [Pixel](#pixel)
    - [Line](#line)
    - [Rectangle](#rectangle)
  - [Images](#images)
    - [Converting Images](#converting-images)
    - [Image](#image)
    - [Icon](#icon)
  - [Updating The Display](#updating-the-display)
    - [Update](#update)
    - [Clear](#clear)
    - [Partial Update](#partial-update)
    - [Invert (aka Dark Mode)](#invert-aka-dark-mode)
    - [Update Speed](#update-speed-1)
  - [LED](#led)

# Summary

## Getting Started

To start coding your Badger 2040, you will need to add the following lines of code to the start of your code file.
```python
import badger2040
badger = badger2040.Badger2040()
```
This will create a `Badger2040` class called `badger` that will be used in the rest of the examples going forward.

## Update Speed

The E Ink display on Badger 2040 supports several update speeds. These can be set using `update_speed(speed)` where `speed` is a value from `0` to `3`. For convenience these speeds have been given the following constants:

* `UPDATE_NORMAL` = `0`
* `UPDATE_MEDIUM` = `1`
* `UPDATE_FAST` = `2`
* `UPDATE_TURBO` = `3`

## Buttons

Badger 2040 features five buttons on its front, labelled A, B, C, ↑ (up), ↓  (down), and 1 button on the rear labelled BOOT/USR. These can be read using the `pressed(button)` function, which accepts the button's pin number. For convenience, each button can be referred to using these constants:

* `BUTTON_A` = `12`
* `BUTTON_B` = `13`
* `BUTTON_C` = `14`
* `BUTTON_UP` = `15`
* `BUTTON_DOWN` = `11`
* `BUTTON_USER` = `23`

## System speed

The system clock speed of the RP2040 can be controlled, allowing power to be saved if on battery, or faster computations to be performed.  Use `badger2040.system_speed(speed)` where `speed` is one of the following constants:

* `SYSTEM_VERY_SLOW` = `0`  _4 MHz if on battery, 48 MHz if connected to USB_
* `SYSTEM_SLOW` = `1`  _12 MHz if on battery, 48 MHz if connected to USB_
* `SYSTEM_NORMAL` = `2`  _48 MHz_
* `SYSTEM_FAST` = `3`  _133 MHz_
* `SYSTEM_TURBO` = `4`  _250 MHz_

On USB, the system will not run slower than 48MHz, as that is the minimum clock speed required to keep the USB connection stable.

It is best to set the clock speed as the first thing in your program, and you must not change it after initializing any drivers for any I2C hardware connected to the qwiic port.  To allow you to set the speed at the top of your program, this method is on the `badger2040` module, rather than the `badger` instance, although we have made sure that it is safe to call it after creating a `badger` instance.

Note that `SYSTEM_TURBO` overclocks the RP2040 to 250MHz, and applies a small over voltage to ensure this is stable.  We've found that every RP2040 we've tested is happy to run at this speed without any issues.

## Other Functions

Below is a list of other functions that have been made available, to help with the creation of more advanced programs.

```python
pen(color)
thickness(thickness)

pixel(x, y)
line(x1, y1, x2, y2)
rectangle(x, y, w, h)

text(message, x, y, scale=1.0, rotation=0.0)
glyph(char, x, y, scale=1.0, rotation=0.0)
measure_text(message, scale=1.0)
measure_glyph(char, scale=1.0)
font(font)

led(brightness)

image(data, w=296, h=128, x=0, y=0)
icon(data, icon_index, sheet_size, icon_size)

clear()
update()
partial_update(x, y, w, h)
invert(inverted)
```

## Other Constants

Below is a list of other constants that have been made available, to help with the creation of more advanced programs.

### Screen Size
* `WIDTH` = `296`
* `HEIGHT` = `128`

### E Ink Pins
* `PIN_CS` = `17`
* `PIN_CLK` = `18`
* `PIN_MOSI` = `19`
* `PIN_DC` = `20`
* `PIN_RESET` = `21`
* `PIN_BUSY` = `26`

### Power Pins
* `PIN_VBUS_DETECT` = `24`
* `PIN_VREF_POWER` = `27`
* `PIN_1V2_REF` = `28`
* `PIN_BATTERY` = `29`
* `PIN_ENABLE_3V3` = `10`

### Activity LED Pin
* `PIN_LED` = `25`

# Function Reference

## Basic Drawing Settings

### Pen Colour

There are 16 pen colours - or "shades of grey" - to choose, from 0 (black) to 15 (white).

Since Badger2040 cannot display colours other than black and white, any value from 1 to 14 will apply dithering when drawn, to simulate a shade of grey.

```python
pen(
    colour  # int: colour from 0 to 15
)
```

### Pen Thickness

Thickness governs how thick a line should be and affects lines and text making them thicker or bolder respectively:

```python
thickness(
    value  # int: thickness in pixels
)
```

## Text

### Draw Text

To draw text in your selected font:

```python
text(
    text,         # string: the text to draw
    x,            # int: x coordinate for the left middle of the text
    y,            # int: y coordinate for the left middle of the text
    scale=1.0,    # float: size of the text
    rotation=0.0  # float: rotation of the text in degrees
)
```

Text uses the "thickness" value, too, and a larger thickness value will give you bold text.

### Measure Text

Sometimes it's useful to know how big a particular bit of text will be on the screen. You can measure it like so:

```python
measure_text(
    text,  # string: the text to measure
    scale  # float: size of the text
)
```

### Change Font

There are five vector fonts to pick from:

* "sans"
* "gothic"
* "cursive"
* "serif"
* "serif_italic"

There are also three bitmap fonts, useful for smaller text and retro goodness. They should handle characters like £, ° and accented characters more gracefully.

* "bitmap6"
* "bitmap8"
* "bitmap14_outline"

When working with bitmap fonts, note that `scale` must be an integer >= 1, and text is orientated from its top left corner.

```python
font(
    font  # string: one of the fonts listed above
)
```

## Lines, Pixels & Rectangles

The basic building blocks of any Badger2040 interface are lines and rectangles.

### Pixel

Single pixels are always drawn in your pen colour, and with the thickness set by `thickness`.

Be wary that colours other than 0 and 15 can result in your pixel being dithered, and invisible! Badger2040 cannot draw just *one* grey pixel, sorry.

```python
pixel(
    x,  # int: x coordinate of pixel to draw
    y   # int: y coordinate of pixel to draw
)
```

### Line

Lines are always drawn in your pen colour, and with the line thickness set by `thickness`.

```python
line(
    x1, # int: x coordinate of starting point
    y1, # int: y coordinate of starting point
    x2, # int: x coordinate of ending point
    y2, # int: y coordinate of ending point
)
```

### Rectangle

Rectangles are always drawn in your pen colour.

They are the best way to see the dithering effects of different pens since anything from 1 to 14 (ie: not full black or white) is dithered.

```python
rectangle(
    x, # int: x coordinate of the rectangle's top left corner
    y, # int: y coordinate of the rectangle's top left corner
    w, # int: width of rectangle
    h  # int: height of rectangle
)
```

## Images

Must be a multiple of 8 pixels wide (because reasons).

You will normally be using a `bytearray` as your source of data.

To load an image you must first allocate a `bytearray` big enough to store it. The formula is `WIDTH * HEIGHT / 8` since there are eight image pixels in every byte (one bit per pixels indicating either 1 black or 0 white):

```python
my_image = bytearray(int(296 * 128 / 8))
```

You can then open your file and read it into your `bytearray`:

```python
open("my_image.bin", "r").readinto(my_image)
```

And finally display it:

```python
screen = badger2040.Badger2040()
screen.image(my_image)
screen.update()
```

### Converting Images

We've supplied a script - `convert.py` - which will help you get your images converted.

Ideally you should pick something already 296x128 pixels or smaller, and in monochrome, but it will dither and convert images for you.

Find it in [/examples/badger2040/image_converter](/examples/badger2040/image_converter).

To convert an oversized image use:

```bash
python3 convert.py --resize --binary my_image.png
```

This will output `my_image.bin`, which you can save to your Badger 2040 via Thonny and display with the code above.

For smaller images such as icons you can omit the `--resize` flag:

```bash
python3 convert.py --binary my_icon.png
```

In all cases your images should be a multiple of 8 pixels wide.

### Image

```python
image(
    data,   # bytearray: raw image data 1bpp
    w=296,  # int: width in pixels
    h=128,  # int: height in pixels
    x=0,    # int: destination x
    y=0,    # int: destination y
)
```

When displaying a full-sized (296x128 pixel) image you can supply only the raw data, eg:

```python
image(data)
```

### Icon

Copies a portion from an icon sheet onto the screen at x/y.

Icons must be square- equal width/height- and sized in multiples of 8 pixels.

IE: 8x8, 16x16, 24x24, 32x32, 64x64

```python
icon(
    data,        # bytearray: raw image data 1bpp
    icon_index,  # int: location of the icon in the sheet, left-most is 0, etc
    sheet_size,  # int: width of the icon sheet in pixels
    icon_size,   # int: icon width/height in pixels (multiple of 8)
    dx,          # int: destination x
    dy,          # int: destination y
)
```

## Updating The Display

### Update

Starts a full update of the screen. Will block until the update has finished.

Update takes no parameters, but the update time will vary depending on which `update_speed` you've selected.

```python
update()
```

### Clear

Before drawing again it can be useful to `clear` your display.

`clear` fills the drawing buffer with the pen colour, giving you a clean slate:

```python
clear()
```

### Partial Update

Starts a partial update of the screen. Will block until the update has finished.

A partial update allows you to update a portion of the screen rather than the whole thing.

That portion *must* be a multiple of 8 pixels tall, but can be any number of pixels wide.

```python
partial_update(
    x,  # int: x coordinate of the update region
    y,  # int: y coordinate of the update region (must be a multiple of 8)
    w,  # int: width of the update region
    h   # int: height of the update region (must be a multiple of 8)
)
```

### Invert (aka Dark Mode)

Badger 2040 can invert all your display data for a quick and easy dark mode:

```python
invert(
    inverted  # bool: True to invert, False to... not invert!
)
```

### Update Speed

Badger 2040 is capable of updating the display at multiple different speeds.

These offer a tradeoff between the quality of the final image and the speed of the update.

There are currently four constants naming the different update speeds from 0 to 3:

* `UPDATE_NORMAL` - a normal update, great for display the first screen of your application and ensuring good contrast and no ghosting
* `UPDATE_MEDIUM` - a good balance of speed and clarity, you probably want this most of the time
* `UPDATE_FAST` - a fast update, good for stepping through screens such as the pages of a book or the launcher
* `UPDATE_TURBO` - a super fast update, prone to ghosting, great for making minor changes such as moving a cursor through a menu

```python
update_speed(
    speed  # int: one of the update constants
)
```

## LED

The white indicator LED can be controlled, with brightness ranging from 0 (off) to 255:

```python
led(
    brightness  # int: 0 (off) to 255 (full)
)
```
