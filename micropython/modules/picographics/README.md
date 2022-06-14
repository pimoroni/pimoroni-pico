# Pico Graphics <!-- omit in toc -->

Pico Graphics is our unified graphics and display library for driving displays from your Pico in MicroPython.

Pico Graphics replaces the individual drivers for displays- if you're been using breakout_colorlcd, ST7789 then you'll need to update your code!

- [Setting up Pico Graphics](#setting-up-pico-graphics)
  - [Supported Displays](#supported-displays)
  - [Supported Graphics Modes (Pen Type)](#supported-graphics-modes-pen-type)
  - [Supported Rotations](#supported-rotations)
  - [Custom Pins](#custom-pins)
- [Function Reference](#function-reference)
  - [Text](#text)
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

### Supported Graphics Modes (Pen Type)

* 4-bit - `PEN_P4` - 16-colour palette of your choice
* 8-bit - `PEN_P8` - 256-colour palette of your choice
* 8-bit RGB332 - `PEN_RGB332` - 256 fixed colours (3 bits red, 3 bits green, 2 bits blue)
* 16-bit RGB565 - `PEN_RGB565` - 64K colours at the cost of RAM. (5 bits red, 6 bits green, 5 bits blue)

These offer a tradeoff between RAM usage and available colours. In most cases you would probably use `RGB332` since it offers the easiest tradeoff. It's also the default.

Eg:

```python
display = PicoGraphics(display=PICO_DISPLAY, pen_type=PEN_RGB332)
```

### Supported Rotations

All SPI LCDs support 0, 90, 180 and 270 degree rotations.

Eg:

```python
display = PicoGraphics(display=PICO_DISPLAY, roation=90)
```

### Custom Pins

The `pimoroni_bus` library includes `SPIBus` for SPI LCDs and `ParallelBus` for Parallel LCDs (like Tufty 2040).

In most cases you'll never need to use these, but they come in useful if you're wiring breakouts to your Pico or using multiple LCDs.

A custom SPI bus:

```python
from pimoroni_bus import SPIBus
from picographics import PicoGraphics, DISPLAY_PICO_EXPLORER, PEN_RGB332

spibus = SPIBus(cs=17, dc=16, sck=18, mosi=19)

display = PicoGraphics(display=DISPLAY_PICO_EXPLORER, bus=spibus, pen_type=PEN_RGB332)
```

## Function Reference

### Text

### Sprites

Pico Display has very limited support for sprites in RGB332 mode.

Sprites must be 8x8 pixels arranged in a 128x128 pixel spritesheet. 1-bit transparency is handled by electing a single colour to skip over.

We've prepared some RGB332-compatible sprite assets for you, but you can use `spritesheet-to-rgb332.py <filename>` to convert your own.

#### Loading Sprites

Use Thonny to upload your `spritesheet.rgb332` file onto your Pico. Then load it into Pico Graphics:

```python
display.load_spritesheet("s4m_ur4i-dingbads.rgb332")
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
j.decode(0, 0, jpegdec.JPEG_SCALE_FULL)

# Display the result
display.update()
```

JPEG files must be small enough to load into RAM for decoding, and must *not* be progressive.

JPEG files will be automatically dithered in RGB332 mode.

In P4 and P8 modes JPEGs are dithered to your custom colour paleete. Their appearance of an image will vary based on the colours you choose.

The arguments for `decode` are as follows:

1. Decode X - where to place the decoded JPEG on screen
2. Decode Y
3. Flags - one of `JPEG_SCALE_FULL`, `JPEG_SCALE_HALF`, `JPEG_SCALE_QUARTER` or `JPEG_SCALE_EIGHTH`