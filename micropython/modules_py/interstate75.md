# Interstate75 (MicroPython) <!-- omit in toc -->

This library offers convenient functions for interacting with [Interstate75](https://shop.pimoroni.com/products/interstate-75) and [Interstate75W](https://shop.pimoroni.com/products/interstate-75-w) - Interstate75 and Interstate75W offer a convenient way and 2 input buttons for all your display and control needs.

## Table of Content
- [Table of Content](#table-of-content)
- [Interstate75 Module](#interstate75-class)
  - [Switches](#switches)
  - [RGB LED](#rgb-led)



## Interstate75 Class

The `Interstate75` class deals with RGB LED and buttons on the Interstate75 and 75W. To create one, import the `interstate75` module, then define a new `board` variable.

This is where you define the HUB75 matrix display size that you wish to use by defining `display=`

```python
DISPLAY_INTERSTATE75_32X32
DISPLAY_INTERSTATE75_64X32
DISPLAY_INTERSTATE75_96X32
DISPLAY_INTERSTATE75_128X32
DISPLAY_INTERSTATE75_64X64
DISPLAY_INTERSTATE75_128X64
DISPLAY_INTERSTATE75_192X64
DISPLAY_INTERSTATE75_256X64
```


```python
import interstate75

display = interstate75.DISPLAY_INTERSTATE75_32X32

board = interstate75.Interstate75(display=display)
```

From here, all features can be accessed by calling functions on `board`. In addition, when using Qwiic / Stemma QT devices, the I2C channel to use can be accessed with `board.i2c`.

### Switches

Interstate75 and 75W have two switches in the front of the board. To read one of the switches, call `.switch_pressed(switch)`, where `switch` is a value from `0` to `.NUM_SWITCHES - 1`. This returns `True` when the specified switch is pressed, and `False` otherwise.

To read a specific input, the `interstate75` module contains these handy constants:

* `SWITCH_A` = `0`
* `SWITCH_B` = `1`

```python
if board.switch_pressed(SWITCH_A):
  # Do something interesting here!

if board.switch_pressed(SWITCH_B):
  # Do something else even more interesting here!
```


### RGB LED

The Interstate has an RGB LED. This is accessed via the following method.


`.set_led(r, g, b)`

Where r, g, b are values between 0 and 255


example:

```python
board.set_led(255, 0, 0)  # Makes the LED Red
board.set_led(0, 255, 0)  # Makes the LED Blue
board.set_led(0, 0, 255)  # Makes the LED Green
```

## Display

The display is all handled by our custom picographics drivers they can be accessed via `.display`.

example:

```python
display = board.display
display.text("Hello World!", 0, 0)
display.line(0, 0, 128, 64) 
board.update()  # Update display with the above items
```
All the picographics functions can be found [Here](../modules/picographics/README.md)