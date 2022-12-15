# Interstate75 (MicroPython) <!-- omit in toc -->

This library offers convenient functions for interacting with [Interstate75](https://shop.pimoroni.com/products/interstate75) and [Interstate75W](https://shop.pimoroni.com/products/interstate75w)- Interstate75 and Interstate75W offer an convenient way  and 2 input buttons for all your display and control needs.

## Table of Content
- [Table of Content](#table-of-content)
- [Interstate75 Module](#gfx-pack-class)
  - [Switches](#switches)
  - [RGBW Backlight](#rgbw-backlight)
- [Display](#display)
  - [Backlight](#backlight)


## Interstate75 Class

The `GfxPack` class deals with RGBW backlight and buttons on the GFX Pack. To create one, import the `gfx_pack` module, then define a new `board` variable:

```python
import interstate75

board = interstate75.Interstate75(display=DISPLAY_INTERSTATE75_32X32)
```

From here, all features can be accessed by calling functions on `board`. In addition, when using Qwiic / Stemma QT devices, the I2C channel to use can be accessed with `board.i2c`.

### Switches

Interstate75 and 75W have two switches in the font of the board. To read one of the switches, call `.switch_pressed(switch)`, where `switch` is a value from `0` to `.NUM_SWITCHES - 1`. This returns `True` when the specified switch is pressed, and `False` otherwise.

To read a specific input, the `interstate75` module contains these handy constants:

* `SWITCH_A` = `0`
* `SWITCH_B` = `1`

```python
if board.switch_pressed(SWITCH_A):
  # Do something interesting here!

if board.switch_pressed(SWITCH_B):
  # Do something else even more interesting here!
```


### RGBW Backlight

The GFX has an RGB backlight as well as the regular Matrix display backlight to change the colour of the backlight. This is accessed via the following method.


`.set_backlight(r, g, b)`

Where r, g, b and w are values between 0 and 255


example:

```python
board.set_backlight(255, 0, 0)  # Makes the Backlight Red
board.set_backlight(0, 255, 0)  # Makes the Backlight Blue
board.set_backlight(0, 0, 255)  # Makes the Backlight Green
board.set_backlight(0, 0, 0, 255)  # Makes the Backlight White
```

## Display

The display is all handled by our custom picographics drivers the can be accessed via `.display`.

example:

```python
display = board.display
display.text("Hello World!", 0, 0)
display.line(0, 0, 128, 64) 
display.update()  # Update display with the above items
```
All the picographics functions can be found [Here](../modules/picographics/README.md)

### Backlight

Included in the picographics display drivers is a function for controling the displays white backlight only which is accessed via `.set_backlight()`.

This function takes a floating point value between `0.0` and `1.0`

```python
display = board.display
display.set_backlight(0.0)  # Backlight is off
display.set_backlight(0.5)  # Backlight is 50%
display.set_backlight(1.0)  # Backlight is 100%
```