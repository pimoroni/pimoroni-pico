# Pico Scroll Pack - MicroPython <!-- omit in toc -->

Our Pico Scroll Pack offers a 17x7 white LED matrix for your Raspberry Pi Pico.  It also includes four buttons!

We've included helper functions to handle every aspect of drawing to the matrix and interfacing with the buttons. See the [function reference](#function-reference) for details.

- [Example Program](#example-program)
- [Function Reference](#function-reference)
  - [init](#init)
  - [get_width](#get_width)
  - [get_height](#get_height)
  - [set_pixel](#set_pixel)
  - [update](#update)
  - [clear](#clear)
  - [is_pressed](#is_pressed)

## Example Program

The following example sets up the matrix, sets each pixel to an increasing brightnesses level, and then clears the matrix only after button A is pressed.

```python
import picoscroll

# Initialise the board
picoscroll.init()

brightness = 0

# For each pixel in the matrix
for y in range (0, picoscroll.get_height()):
  for x in range (0, picoscroll.get_width()):
    # Set that pixel to an increasing level of brightness
    picoscroll.set_pixel(x, y, brightness)
    brightness += 2

# Push the data to the matrix
picoscroll.update()

# Wait until the A button is pressed
while picoscroll.is_pressed(picoscroll.BUTTON_A) == False:
    pass

# Set the brightness of all pixels to 0
picoscroll.clear()
picoscroll.update()
```

## Function reference

### init

Sets up the Pico Scroll Pack. The `init` function must be called before any other functions as it configures the required pins on the Pico board.

```python
picoscroll.init()
```

### get_width
### get_height

These return integers describing the height and width of the Scroll Pack in pixels.

```python
width_in_pixels = picoscroll.get_width()
height_in_pixels = picoscroll.get_height()
```

### set_pixel

This function sets a pixel at the `x` and `y` coordinates to a brightness level specified by the `l` parameter.  The value of `l` must be 0-255.  Changes will not be visible until `update()` is called.

```python
picoscroll.set_pixel(x, y, l)
```

### update

Pushes pixel data from the Pico to the Scroll Pack.  Until this function is called any `set_pixel` or `clear` calls won't have any visible effect.

```python
picoscroll.update()
```

### clear

Sets the brightness of all pixels to `0`.  Will not visibly take effect until `update` is called.

```python
picoscroll.clear()
```

### is_pressed

Checks if a specified button is currently being pressed.  Valid values of `b` are `picoscroll.BUTTON_A`, `picoscroll.BUTTON_B`, `picoscroll.BUTTON_X`, or `picoscroll.BUTTON_Y`, which match the silkscreen labels beside the buttons on the board.

```python
picoscroll.is_pressed(b)
```
