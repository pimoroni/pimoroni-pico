# Pico Display Pack - MicroPython <!-- omit in toc -->

Pico Display Pack is a vibrant 1.14", 240 x 135 pixel IPS LCD screen for your Raspberry Pi Pico, with four useful buttons and a RGB LED. [Click here](https://shop.pimoroni.com/products/pico-display-pack) to find out more!

We've included helper functions to handle every aspect of drawing to the screen and interfacing with the buttons and LED. See the [function reference](#function-reference) for details.

- [Example Program](#example-program)
- [Function Reference](#function-reference)
  - [init](#init)
  - [set_backlight](#set_backlight)
  - [set_led](#set_led)
  - [is_pressed](#is_pressed)
  - [update](#update)
  - [set_pen](#set_pen)
  - [create_pen](#create_pen)
  - [clear](#clear)
  - [pixel](#pixel)
  - [pixel_span](#pixel_span)
  - [rectangle](#rectangle)
  - [circle](#circle)
  - [character](#character)
  - [text](#text)
  - [set_clip](#set_clip)
  - [remove_clip](#remove_clip)

## Example Program

The following example sets up Pico Display, displays some basic demo text and illuminates the RGB LED green when the A button is pressed.

```python
import utime
import picodisplay

# Initialise Picodisplay with a bytearray display buffer
buf = bytearray(picodisplay.get_width() * picodisplay.get_height() * 2)
picodisplay.init(buf)
picodisplay.set_backlight(1.0)

picodisplay.set_pen(255, 0, 0)                    # Set a red pen
picodisplay.clear()                               # Clear the display buffer
picodisplay.set_pen(255, 255, 255)                # Set a white pen
picodisplay.text("pico display", 10, 10, 240, 6)  # Add some text
picodisplay.update()                              # Update the display with our changes

picodisplay.set_led(255, 0, 0)   # Set the RGB LED to red
utime.sleep(1)                   # Wait for a second
picodisplay.set_led(0, 255, 0)   # Set the RGB LED to green
utime.sleep(1)                   # Wait for a second
picodisplay.set_led(0, 0, 255)   # Set the RGB LED to blue

while picodisplay.is_pressed(picodisplay.BUTTON_A) == False:
    pass

picodisplay.set_led(0, 255, 0)  # Set the RGB LED to green
```

## Function Reference

### init

Sets up Pico Display. `init` must be called before any other functions since it configures the required PWM and GPIO. `init()` needs a bytearray type display buffer that MicroPython's garbage collection isn't going to eat, make sure you create one and pass it in like so:

```python
buf = bytearray(picodisplay.get_width() * picodisplay.get_height() * 2)
picodisplay.init(buf)
```

### set_backlight

Sets the display backlight from 0.0 to 1.0.

```python
picodisplay.set_backlight(brightness)
```

Uses hardware PWM to dim the display backlight, dimming values are gamma-corrected to provide smooth brightness transitions across the full range of intensity. This may result in some low values mapping as "off."

### set_led

Sets the RGB LED on Pico Display with an RGB triplet.

```python
picodisplay.set_led(r, g, b)
```

Uses hardware PWM to drive the LED. Values are automatically gamma-corrected to provide smooth brightness transitions and low values may map as "off."

### is_pressed

Reads the GPIO pin connected to one of Pico Display's buttons, returning `True` if it's pressed and `False` if it is released.

```python
picodisplay.is_pressed(button)
```

The button value should be a number denoting a pin, and constants `picodisplay.BUTTON_A`, `picodisplay.BUTTON_B`, `picodisplay.BUTTON_X` and `picodisplay.BUTTON_Y` are supplied to make it easier. e:

```python
is_a_button_pressed = picodisplay.is_pressed(picodisplay.BUTTON_A)
```

### update

To display your changes on Pico Display's screen you need to call `update`.

```python
picodisplay.update()
```

### set_pen

Sets the colour to be used by subsequent calls to drawing functions.  The values for `r`, `g` and `b` should be from 0-255 inclusive.

```python
picodisplay.set_pen(r, g, b)
```

### create_pen

Creates a pen which can be stored as a variable for faster re-use of the same colour through calls to `set_pen`.  The values for `r`, `g` and `b` should be from 0-255 inclusive.

```python
pen_colour = picodisplay.create_pen(r, g, b)
picodisplay.set_pen(penColour)
```

### clear

Fills the display buffer with the currently set pen colour.

```python
picodisplay.clear()
```

### pixel

Sets a single pixel in the display buffer to the current pen colour.  The `x` and `y` parameters determine the X and Y coordinates of the drawn pixel in the buffer.

```python
picodisplay.pixel(x, y)
```

### pixel_span

Draws a horizontal line of pixels to the buffer.  The `x` and `y` parameters specify the coordinates of the first pixel of the line.  The `l` parameter describes the length of the line in pixels.  This function will only extend the line towards the end of the screen, i.e. the `x` coordinate should specify the left hand extreme of the line.

```python
picodisplay.pixel_span(x, y, l)
```

### rectangle

Draws a rectangle filled with the current pen colour to the buffer.  The `x` and `y` parameters specify the upper left corner of the rectangle, `w` specifies the width in pixels, and `h` the height.

```python
picodisplay.rectangle(x, y, w, h)
```

![Rectangle function explanation image](/micropython/modules/pico_display/images/rectangle.png)

### circle

Draws a circle filled with the current pen colour to the buffer.  The `x` and `y` parameters specify the centre of the circle, `r` specifies the radius in pixels.

```python
picodisplay.rectangle(x, y, w, h)
```

![Circle function explanation image](/micropython/modules/pico_display/images/circle.png)

### character

Draws a single character to the display buffer in the current pen colour.  The `c` parameter should be the ASCII numerical representation of the character to be printed, `x` and `y` describe the top-left corner of the character's drawing field.  The `character` function can also be given an optional 4th parameter, `scale`, describing the scale of the character to be drawn.  Default value is 2.

```python
char_a = ord('a')
picodisplay.character(char_a, x, y)
picodisplay.character(char_a, x, y, scale)
```

### text

Draws a string of text to the display buffer in the current pen colour.  The `string` parameter is the string of text to be drawn, and `x` and `y` specify the upper left corner of the drawing field.  The `wrap` parameter describes the width, in pixels, after which the next word in the string will be drawn on a new line underneath the current text.  This will wrap the string over multiple lines if required.  This function also has an optional parameter, `scale`, which describes the size of the characters to be drawn.  The default `scale` is 2.

```python
picodisplay.text(string, x, y, wrap)
picodisplay.text(string, x, y, wrap, scale)
```

![Text scale explanation image](/micropython/modules/pico_display/images/text_scale.png)

### set_clip

This function defines a rectangular area outside which no drawing actions will take effect.  If a drawing action crosses the boundary of the clip then only the pixels inside the clip will be drawn.  Note that `clip` does not remove pixels which have already been drawn, it only prevents new pixels being drawn outside the described area.  A more visual description of the function of clips can be found below.  Only one clip can be active at a time, and defining a new clip replaces any previous clips.  The `x` and `y` parameters describe the upper-left corner of the clip area, `w` and `h` describe the width and height in pixels.

```python
picodisplay.set_clip(x, y, w, h)
```

![Clip function explanation image](/micropython/modules/pico_display/images/clip.png)

### remove_clip

This function removes any currently implemented clip.
