# GFX Pack MicroPython <!-- omit in toc -->

This library offers convenient functions for interacting with [Pico GFX Pack](https://shop.pimoroni.com/products/gfxpack) - The Pico GFX Pack adds a 128x64 LCD Matrix display to your headered Raspberry Pi Pico or PicoW, with RGBW backlight and 5 input buttons for all your display anc control needs.

## Table of Content
- [Table of Content](#table-of-content)
- [GFX Pack Classes](#GFX-Pack-class)
  - [Switches](#switches)
  - [RGB Backlight](#rgb-backlight)


## GFX Pack Class

The `GfxPack` class deals with RGB backlight and buttons on the GFX Pack. To create one, import the `gfx_pack` module, then define a new `board` variable:

```python
import gfx_pack

board = gfx_pack.GfxPack()
```


### Switches

The GFX Pack has 5 user switchs located just under the display labeled A to E. The names of these switches in the class are:

`.switch_a`

`.switch_b`

`.switch_c`

`.switch_d`

`.switch_e`

These can be read with the `.is_pressed` method.

```python

if (board.switch_a.is_pressed):
    print('You pressed Switch A')

if (board.switch_b.is_pressed):
    print('You pressed Switch B')
```



### RGB Backlight

The GFX has an RGB backlight as well as the regular Matrix display backlight to change the colour of the backlight. This is accessed via the following method.


`.rgb.set_rgb(r, g, b)`

Where r, g, b are values between 0 and 255


example:


```python

board.rgb.set_rgb(255, 0, 0)  # Makes the Backlight Red

board.rgb.set_rgb(0, 255, 0)  # Makes the Backlight Blue

board.rgb.set_rgb(0, 0, 255)  # Makes the Backlight Green

```