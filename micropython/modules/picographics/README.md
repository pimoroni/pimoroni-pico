# Pico Graphics <!-- omit in toc -->

Pico Graphics is our unified graphics and display library for driving displays from your Pico in MicroPython.

Pico Graphics replaces the individual drivers for displays- if you're been using breakout_colorlcd, ST7789 then you'll need to update your code!

- [Setting up Pico Graphics](#setting-up-pico-graphics)
  - [Supported Displays](#supported-displays)
    - [Interstate75 and Interstate75W Display modes](#interstate75-and-interstate75w-display-modes)
  - [Supported Graphics Modes (Pen Type)](#supported-graphics-modes-pen-type)
  - [Supported Rotations](#supported-rotations)
  - [Custom Pins](#custom-pins)
    - [SPI / Parallel](#spi--parallel)
    - [I2C](#i2c)
- [Function Reference](#function-reference)
  - [General](#general)
    - [Creating and Setting Pens](#creating-and-setting-pens)
      - [RGB888, RGB565, RGB332, P8 and P4 modes](#rgb888-rgb565-rgb332-p8-and-p4-modes)
      - [Monochrome Modes](#monochrome-modes)
      - [Inky Frame](#inky-frame)
    - [Controlling the Backlight](#controlling-the-backlight)
    - [Clipping](#clipping)
    - [Clear](#clear)
    - [Update](#update)
    - [Get Bounds](#get-bounds)
  - [Text](#text)
    - [Changing The Font](#changing-the-font)
    - [Changing The Thickness](#changing-the-thickness)
    - [Drawing Text](#drawing-text)
  - [Basic Shapes](#basic-shapes)
    - [Line](#line)
    - [Circle](#circle)
    - [Rectangle](#rectangle)
    - [Triangle](#triangle)
    - [Polygon](#polygon)
  - [Pixels](#pixels)
  - [Palette Management](#palette-management)
    - [Utility Functions](#utility-functions)
  - [Sprites](#sprites)
    - [Loading Sprites](#loading-sprites)
    - [Drawing Sprites](#drawing-sprites)
  - [JPEG Files](#jpeg-files)

## Setting up Pico Graphics

You must construct an instance of PicoGraphics with your desired display:

```python
from picographics import PicoGraphics, DISPLAY_LCD_160X80

display = PicoGraphics(display=DISPLAY_LCD_160X80)
```

Bear in mind that MicroPython has only 192K of RAM available- a 320x240 pixel display in RGB565 mode uses 150K!

### Supported Displays

* Pico Display - 240x135 SPI LCD - `DISPLAY_PICO_DISPLAY`
* Pico Display 2 - 320x240 SPI LCD - `DISPLAY_PICO_DISPLAY_2`
* Tufty 2040 - 320x240 Parallel LCD - `DISPLAY_TUFTY_2040`
* Pico Explorer - 240x240 SPI LCD - `DISPLAY_PICO_EXPLORER`
* Enviro Plus - 240x240 SPI LCD - `DISPLAY_ENVIRO_PLUS`
* 240x240 Round SPI LCD Breakout - `DISPLAY_ROUND_LCD_240X240`
* 240x240 Square SPI LCD Breakout - `DISPLAY_LCD_240X240`
* 160x80 SPI LCD Breakout - `DISPLAY_LCD_160X80`
* 128x128 I2C OLED - `DISPLAY_I2C_OLED_128X128`
* Pico Inky Pack / Badger 2040 / Badger 2040 W - 296x128 mono E ink - `DISPLAY_INKY_PACK`
* Inky Frame 5.7" - 600x448 7-colour E ink - `DISPLAY_INKY_FRAME`
* Inky Frame 4.0" - 640x400 7-colour E ink - `DISPLAY_INKY_FRAME_4`
* Inky Frame 7.3" - 800x480 7-colour E ink - `DISPLAY_INKY_FRAME_7`
* Pico GFX Pack - 128x64 mono LCD Matrix - `DISPLAY_GFX_PACK`
* Galactic Unicorn - 53x11 LED Matrix - `DISPLAY_GALACTIC_UNICORN`
* Interstate75 and 75W - HUB75 Matrix driver - `DISPLAY_INTERSTATE75_SIZEOFMATRIX` please read below!
* Cosmic Unicorn - 32x32 LED Matrix - `DISPLAY_COSMIC_UNICORN`

#### Interstate75 and Interstate75W Display modes

Both the Interstate75 and Interstate75W support lots of different sizes of HUB75 matrix displays.

The available display settings are listed here:

* 32 x 32 Matrix - `DISPLAY_INTERSTATE75_32X32`
* 64 x 32 Matrix - `DISPLAY_INTERSTATE75_64X32`
* 96 x 32 Matrix - `DISPLAY_INTERSTATE75_96X32`
* 128 x 32 Matrix - `DISPLAY_INTERSTATE75_128X32`
* 64 x 64 Matrix - `DISPLAY_INTERSTATE75_64X64`
* 128 x 64 Matrix - `DISPLAY_INTERSTATE75_128X64`
* 192 x 64 Matrix - `DISPLAY_INTERSTATE75_192X64`
* 256 x 64 Matrix - `DISPLAY_INTERSTATE75_256X64`

### Supported Graphics Modes (Pen Type)

* 1-bit - `PEN_1BIT` - mono, used for Pico Inky Pack and i2c OLED
* 3-bit - `PEN_3BIT` - 8-colour, used for Inky Frame
* 4-bit - `PEN_P4` - 16-colour palette of your choice
* 8-bit - `PEN_P8` - 256-colour palette of your choice
* 8-bit RGB332 - `PEN_RGB332` - 256 fixed colours (3 bits red, 3 bits green, 2 bits blue)
* 16-bit RGB565 - `PEN_RGB565` - 64K colours at the cost of RAM. (5 bits red, 6 bits green, 5 bits blue)
* 24-bit RGB888 - `PEN_RGB888` - 16M colours at the cost of lots of RAM. (8 bits red, 8 bits green, 8 bits blue)

These offer a tradeoff between RAM usage and available colours. In most cases you would probably use `RGB332` since it offers the easiest tradeoff. It's also the default for colour LCDs.

Eg:

```python
display = PicoGraphics(display=PICO_DISPLAY)
```

Is equivalent to:

```python
display = PicoGraphics(display=PICO_DISPLAY, pen_type=PEN_RGB332)
```

### Supported Rotations

All SPI LCDs support 0, 90, 180 and 270 degree rotations.

Eg:

```python
display = PicoGraphics(display=PICO_DISPLAY, rotate=90)
```

### Custom Pins

#### SPI / Parallel

The `pimoroni_bus` library includes `SPIBus` for SPI LCDs and `ParallelBus` for Parallel LCDs (like Tufty 2040).

In most cases you'll never need to use these, but they come in useful if you're wiring breakouts to your Pico or using multiple LCDs.

A custom SPI bus:

```python
from pimoroni_bus import SPIBus
from picographics import PicoGraphics, DISPLAY_PICO_EXPLORER, PEN_RGB332

spibus = SPIBus(cs=17, dc=16, sck=18, mosi=19)

display = PicoGraphics(display=DISPLAY_PICO_EXPLORER, bus=spibus, pen_type=PEN_RGB332)
```

#### I2C

The `pimoroni_i2c` library includes `PimoroniI2C` which can be used to change the pins used by the mono OLED:

```python
from pimoroni_i2c import PimoroniI2C
from picographics import PicoGraphics, DISPLAY_I2C_OLED_128X128

i2cbus = PimoroniI2C(4, 5)

display = PicoGraphics(display=DISPLAY_I2C_OLED_128X128, bus=i2cbus)
```

## Function Reference

### General

#### Creating and Setting Pens

##### RGB888, RGB565, RGB332, P8 and P4 modes

Create a pen colour for drawing into a screen:

```python
my_pen = display.create_pen(r, g, b)
```

In RGB565 and RGB332 modes this packs the given RGB into an integer representing a colour in these formats and returns the result.

In P4 and P8 modes this will consume one palette entry, or return an error if your palette is full. Palette colours are stored as RGB and converted when they are displayed on screen.

You can also now specify an HSV pen, which allows a pen to be created from HSV (Hue, Saturation, Value) values between 0.0 and 1.0, avoiding the need to calculate the RGB result in Python.

```python
display.create_pen_hsv(h, s, v)
```

To tell PicoGraphics which pen to use:

```python
display.set_pen(my_pen)
```

This will be either an RGB332, RGB565 or RGB888 colour, or a palette index.

##### Monochrome Modes

For 1BIT mode - such as for Inky Pack and the Mono OLED - pens are handled a little differently.

There's no need to create one, since mapping an RGB colour to black/white is meaningless.

Instead you can pick from 16 shades of grey which are automatically dithered into the PicoGraphics buffer, where:

* `0` is Black,
* `1 - 14` are shades of grey,
* `15` is white.

And just call `set_pen` with your desired shade:

```python
display.set_pen(0)   # Black
display.set_pen(15)  # White
```

Because shades 1 through 14 are created with ordered dither you should avoid using them for text, small details or lines.

Dithering works by mixing black and white pixels in various patterns and quantities to fake grey shades.

If you were to try and draw a single "grey" pixel it will end up either black or white depending on where it's drawn and which shade of grey you pick.

##### Inky Frame

Inky Frame is a special case- the display itself supports only 7 (8 if you include its cleaning "clear" colour, which we call Taupe) colours.

These are:

* `BLACK` = 0
* `WHITE` = 1
* `GREEN` = 2
* `BLUE` = 3
* `RED` = 4
* `YELLOW` = 5
* `ORANGE` = 6
* `TAUPE` = 7

#### Controlling the Backlight

You can set the display backlight brightness between `0.0` and `1.0`:

```python
display.set_backlight(0.5)
```

#### Clipping

Set the clipping bounds for drawing:

```python
display.set_clip(x, y, w, h)
```

Remove the clipping bounds:

```python
display.remove_clip()
```

#### Clear

Clear the display to the current pen colour:

```python
display.clear()
```

This is equivalent to:

```python
w, h = display.get_bounds()
display.rectangle(0, 0, w, h)
```

You can clear portions of the screen with rectangles to save time redrawing things like JPEGs or complex graphics.

#### Update

Send the contents of your Pico Graphics buffer to your screen:

```python
display.update()
```

If you are using a Galactic Unicorn, then the process for updating the display is different. Instead of the above, do:

```python
galactic_unicorn.update(display)
```

#### Get Bounds

You can use `get_bounds()` to get the width and height of the display - useful for writing code that's portable across different displays.

```python
WIDTH, HEIGHT = display.get_bounds()
```

### Text

#### Changing The Font

Change the font:

```python
display.set_font(font)
```

Bitmap fonts.
These are aligned from their top-left corner.

* `bitmap6`
* `bitmap8`
* `bitmap14_outline`

Vector (Hershey) fonts. 
These are aligned horizontally (x) to their left edge, but vertically (y) to their midline excluding descenders [i.e., aligned at top edge of lower case letter m]. At `scale=1`, the top edge of upper case letters is 10 pixels above the specified `y`, text baseline is 10 pixels below the specified `y`, and descenders go down to 20 pixels below the specified `y`.

* `sans`
* `gothic`
* `cursive`
* `serif_italic`
* `serif`

#### Changing The Thickness

Vector (Hershey) fonts are drawn with individual lines. By default these are 1px thick, making for very thin and typically illegible text.

To change the thickness of lines used for Vector fonts, use the `set_thickness` method:

```python
display.set_thickness(n)
```

Drawing thick text involves setting a lot more pixels and may slow your drawing down considerably. Be careful how and where you use this.

#### Drawing Text

Write some text:

```python
display.text(text, x, y, wordwrap, scale, angle, spacing)
```

* `text` - the text string to draw
* `x` - the destination X coordinate
* `y` - the destination Y coordinate
* `wordwrap` - number of pixels width before trying to break text into multiple lines
* `scale` - size
* `angle` - rotation angle (Vector only!)
* `spacing` - letter spacing

Text scale can be a whole number (integer) for Bitmap fonts, or a decimal (float) for Vector (Hershey) fonts.

For example:

```python
display.set_font("bitmap8")
display.text("Hello World", 0, 0, scale=2)
```
Draws "Hello World" in a 16px tall, 2x scaled version of the `bitmap8` font.

Sometimes you might want to measure a text string for centering or alignment on screen, you can do this with:

```python
width = display.measure_text(text, scale, spacing)
```

The height of each Bitmap font is explicit in its name.

Write a single character:

```python
display.character(char, x, y, scale)
```

Specify `char` using a [decimal ASCII code](https://www.ascii-code.com/). Note not all characters are supported.

For example:
```python
display.set_font("bitmap8")
display.character(38, 0, 0, scale=2)
```
Draws an ampersand in a 16px tall, 2x scaled version of the 'bitmap8' font.

### Basic Shapes

#### Line

To draw a straight line at any angle between two specified points:

```python
display.line(x1, y1, x2, y2)
```

The X1/Y1 and X2/Y2 coordinates describe the start and end of the line respectively.

If you need a thicker line, for an outline or UI elements you can supply a fifth parameter - thickness - like so:

```python
display.line(x1, y1, x2, y2, thickness)
```

#### Circle

To draw a circle:

```python
display.circle(x, y, r)
```

* `x` - the destination X coordinate
* `y` - the destination Y coordinate
* `r` - the radius

The X/Y coordinates describe the center of your circle.

#### Rectangle

```python
display.rectangle(x, y, w, h)
```

* `x` - the destination X coordinate
* `y` - the destination Y coordinate
* `w` - the width
* `h` - the height

#### Triangle

```python
display.triangle(x1, y1, x2, y2, x3, y3)
```

The three pairs of X/Y coordinates describe each point of the triangle.

#### Polygon

To draw other shapes, you can provide a list of points to `polygon`:

```python
display.polygon([
  (0, 10),
  (20, 10),
  (20, 0),
  (30, 20),
  (20, 30),
  (20, 20),
  (0, 20),
])
```

### Pixels

Setting individual pixels is slow, but you can do it with:

```python
display.pixel(x, y)
```

You can draw a horizontal span of pixels a little faster with:

```python
display.pixel_span(x, y, length)
```

(use `display.line()` instead if you want to draw a straight line at any angle)

### Palette Management

Intended for P4 and P8 modes.

You have a 16-color and 256-color palette respectively.

Set n elements in the palette from a list of RGB tuples:

```python
display.set_palette([
  (r, g, b),
  (r, g, b),
  (r, g, b)
])
```

Update an entry in the P4 or P8 palette with the given colour.

```python
display.update_pen(index, r, g, b)
```

This is stored internally as RGB and converted to whatever format your screen requires when displayed.

Reset a pen back to its default value (black, marked unused):

```python
display.reset_pen(index)
```

#### Utility Functions

Sometimes it can be useful to convert between colour formats:

* `RGB332_to_RGB`
* `RGB_to_RGB332`
* `RGB565_to_RGB`
* `RGB_to_RGB565`


### Sprites

Pico Display has very limited support for sprites in RGB332 mode.

Sprites must be 8x8 pixels arranged in a 128x128 pixel spritesheet. 1-bit transparency is handled by electing a single colour to skip over.

We've prepared some RGB332-compatible sprite assets for you, but you can use `spritesheet-to-rgb332.py <filename>` to convert your own.

#### Loading Sprites

You'll need to include the [pen_type](#supported-graphics-modes-pen-type) in the import statement, and define the pen_type before using loading the spritesheet:

``` python
from picographics import PicoGraphics, PEN_RGB565, PEN_RGB332

display = PicoGraphics(display=PICO_DISPLAY, pen_type=PEN_RGB332)
```
Use Thonny to upload your `spritesheet.rgb332` file onto your Pico. Then load it into Pico Graphics:

```python
display.load_spritesheet("s4m_ur4i-dingbads.rgb332")
```

and then update the display, to show the sprite:

```python
display.update()
```

#### Drawing Sprites

And finally display a sprite:

```python
display.sprite(0, 0, 0, 0)
```

These arguments for `sprite` are as follows:

1. Sprite X position (from 0-15) - this selects the horizontal location of an 8x8 sprite from your 128x128 pixel spritesheet.
2. Sprite Y position (from 0-15)
3. Destination X - where to draw on your screen horizontally
4. Destination Y = where to draw on your screen vertically
5. Scale (optional) - an integer scale value, 1 = 8x8, 2 = 16x16 etc.
6. Transparent (optional) - specify a colour to treat as transparent

### JPEG Files

We've included BitBank's JPEGDEC - https://github.com/bitbank2/JPEGDEC - so you can display JPEG files on your LCDs.

Eg:

```python
import picographics
import jpegdec

display = picographics.PicoGraphics(display=picographics.DISPLAY_PICO_EXPLORER)

# Create a new JPEG decoder for our PicoGraphics
j = jpegdec.JPEG(display)

# Open the JPEG file
j.open_file("filename.jpeg")

# Decode the JPEG
j.decode(0, 0, jpegdec.JPEG_SCALE_FULL, dither=True)

# Display the result
display.update()
```

JPEG files must be small enough to load into RAM for decoding, and must *not* be progressive.

JPEG files will be automatically dithered in RGB332 mode.

In P4 and P8 modes JPEGs are dithered to your custom colour palette. Their appearance of an image will vary based on the colours you choose.

The arguments for `decode` are as follows:

1. Decode X - where to place the decoded JPEG on screen
2. Decode Y
3. Flags - one of `JPEG_SCALE_FULL`, `JPEG_SCALE_HALF`, `JPEG_SCALE_QUARTER` or `JPEG_SCALE_EIGHTH`
4. If you want to turn off dither altogether, try `dither=False`. This is useful if you want to [pre-dither your images](https://ditherit.com/) or for artsy posterization effects.
