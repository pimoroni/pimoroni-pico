# Pico Unicorn Pack - MicroPython <!-- omit in toc -->

Our Pico Unicorn Pack offers 7x17 bright RGB LEDs driven by Pico's PIO.

We've included helper functions for lighting up the LEDs and interfacing with the buttons. See the [function reference](#function-reference) for details.

- [Example Program](#example-program)
- [Function Reference](#function-reference)
  - [init](#init)
  - [set_pixel](#set_pixel)
  - [set_pixel_value](#set_pixel_value)
  - [is_pressed](#is_pressed)
  - [get_width / get_height](#get_width--get_height)
  - [clear](#clear)

## Example Program

The following example sets up Pico Unicorn, displays a rainbow across the LEDs and turns them all off when a button is pressed.

```python
import math
import time
import picounicorn

picounicorn.init()

# From CPython Lib/colorsys.py
def hsv_to_rgb(h, s, v):
    if s == 0.0:
        return v, v, v
    i = int(h*6.0)
    f = (h*6.0) - i
    p = v*(1.0 - s)
    q = v*(1.0 - s*f)
    t = v*(1.0 - s*(1.0-f))
    i = i%6
    if i == 0:
        return v, t, p
    if i == 1:
        return q, v, p
    if i == 2:
        return p, v, t
    if i == 3:
        return p, q, v
    if i == 4:
        return t, p, v
    if i == 5:
        return v, p, q

w = picounicorn.get_width()
h = picounicorn.get_height()

# Display a rainbow across Pico Unicorn
for x in range(w):
    for y in range(h):
        r, g, b = [int(c * 255) for c in hsv_to_rgb(x / w, y / h, 1.0)]
        picounicorn.set_pixel(x, y, r, g, b)

print("Press Button A")

while not picounicorn.is_pressed(picounicorn.BUTTON_A):  # Wait for Button A to be pressed
    pass

# Clear the display
picounicorn.clear()

print("Button A pressed!")
```

## Function Reference

### init

Sets up Pico Unicorn. `init` must be called before any other functions since it configures the PIO and require GPIO inputs. Just call `init()` like so: 

```python
picounicorn.init()
```

### set_pixel

Sets an RGB LED on Pico Unicorn with an RGB triplet:

```python
picounicorn.set_pixel(x, y, r, g, b)
```

Uses hardware PWM to drive the LED. Values are automatically gamma-corrected to provide smooth brightness transitions and low values may map as "off."

### set_pixel_value

Sets all elements of an RGB LED on Pico Unicorn to a single value:

```python
picounicorn.set_pixel_value(x, y, v)
```

This lights an LED up white at varying intensity and is useful if you want to pretend Pico Unicorn is a monochrome display.

### is_pressed

Reads the GPIO pin connected to one of Pico Unicorn's buttons, returning `True` if it's pressed and `False` if it is released.

```python
picounicorn.is_pressed(button)
```

The button value should be a number denoting a pin, and constants `picounicorn.BUTTON_A`, `picounicorn.BUTTON_B`, `picounicorn.BUTTON_X` and `picounicorn.BUTTON_Y` are supplied to make it easier. e:

```python
is_a_button_pressed = picounicorn.is_pressed(picounicorn.BUTTON_A)
```

### get_width / get_height

You can get the width and height of the Pico Unicorn display:

```python
width = picounicorn.get_width()
height = picounicorn.get_height()
```

This is useful where you're looping through the rows and columns to display a pattern- such as a rainbow using `hsv_to_rgb`.

### clear

Turn off all the LEDs:
```python
picounicorn.clear()
```

