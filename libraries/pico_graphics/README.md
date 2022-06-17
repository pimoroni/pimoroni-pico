# Pico Graphics <!-- omit in toc -->

Pico Graphics is a tiny graphics library supporting a number of underlying buffer formats including 8-bit paletted (256 colour), 8-bit RGB332 (256 colour), 16-bit RGB565 (65K colour) and 4-bit packed (8 colour).

It supports drawing text, primitive and individual pixels and includes basic types such as `rect` and `point` brimming with methods to help you develop games and applications.

- [Overview](#overview)
  - [Pen Types](#pen-types)
  - [Creating A Pico Graphics Instance](#creating-a-pico-graphics-instance)
- [Function Reference](#function-reference)
  - [Types](#types)
    - [rect](#rect)
      - [rect.empty](#rectempty)
      - [rect.contains](#rectcontains)
      - [rect.intersects](#rectintersects)
      - [rect.intersection](#rectintersection)
      - [rect.inflate & rect.deflate](#rectinflate--rectdeflate)
    - [point](#point)
      - [point.clamp](#pointclamp)
  - [Pens & Clipping](#pens--clipping)
    - [set_pen](#set_pen)
    - [create_pen](#create_pen)
    - [set_clip & remove_clip](#set_clip--remove_clip)
  - [Palette](#palette)
    - [update_pen](#update_pen)
    - [reset_pen](#reset_pen)
  - [Pixels](#pixels)
    - [pixel](#pixel)
    - [pixel_span](#pixel_span)
  - [Primitives](#primitives)
    - [rectangle](#rectangle)
    - [circle](#circle)
  - [Text](#text)
  - [Change Font](#change-font)


## Overview

Pico Graphics comes in multiple flavours depending on which underlying buffer type you wish to work with.

Your buffer doesn't have to be native to your display. For example a 16-bit ST7789 display can work with P4, P8, RGB332 and RGB565 buffers, with palette lookups handled for you on the fly.

### Pen Types

* `P4` - 4-bit packed, with an 8 colour palette. This is commonly used for 7/8-colour e-ink displays or driving large displays with few colours.
* `P8` - 8-bit, with a 256 colour palette. Great balance of memory usage versus available colours. You can replace palette entries on the fly.
* `RGB332` - 8-bit, with a fixed 256 colour RGB332 palette. Great for quickly porting an RGB565 app to use less RAM. Limits your colour choices, but is easier to grok.
* `RGB565` - 16-bit, 65K "True Colour." Great for rainbows, gradients and images but comes at the cost of RAM!

### Creating A Pico Graphics Instance

To create a Pico Graphics instance to draw into, you should construct an instance of the Pen type class you want to use:

```c++
PicoGraphics_PenP4 graphics(WITH, HEIGHT, nullptr);
PicoGraphics_PenP8 graphics(WITH, HEIGHT, nullptr);
PicoGraphics_PenRGB332 graphics(WITH, HEIGHT, nullptr);
PicoGraphics_PenRGB565 graphics(WITH, HEIGHT, nullptr);
```

To draw something to a display you should create a display driver instance, eg:

```c++
ST7789 st7789(PicoExplorer::WIDTH, PicoExplorer::HEIGHT, ROTATE_0, false, get_spi_pins(BG_SPI_FRONT));
```

And then send it the Pico Graphics instance to draw:

```c++
st7789.update(&graphics);
```

The driver will check your graphics type and act accordingly.

## Function Reference

### Types

#### rect

The `rect` type describes a rectangle in terms of its x, y position, width and height.

##### rect.empty

```c++
bool rect::empty();
```

##### rect.contains

```c++
bool rect::contains(const rect &p);
```

`contains` allows you to check if a `rect` contains a specific `point`. This can be useful for checking collissions (have I clicked on something?):

```c++
point cursor(50, 50);
rect widget(0, 0, 100, 100);
bool hover = widet.contains(cursor);
```

##### rect.intersects

```c++
bool rect::intersects(const rect &r);
```

`intersects` allows you to check if a `rect` intersects or overlaps another `rect`, for example these rectangles do not intersect:

```c++
rect a(10, 10, 10, 10);
rect b(30, 10, 10, 10);
a.intersects(b) == false
```

And these do:

```c++
rect a(10, 10, 10, 10);
rect b(15, 10, 10, 10);
a.intersects(b) == true
```

##### rect.intersection

```c++
rect rect::intersection(const rect &r);
```

`intersection` takes an input `rect` and returns a new `rect` that describes the region in which the two `rect`s overlap. For example:

```c++
rect a(0, 0, 10, 20);
rect b(0, 0, 20, 10);
rect c = a.intersection(b);
```

In this case `c` would equal `rect c(0, 0, 10, 10);` since this is the region that `a` and `b` overlap.


##### rect.inflate & rect.deflate

```c++
void rect::inflate(int32_t v);
void rect::declate(int32_t v);
```

`inflate` will inflate a `rect`, like a balooon, by adding the number of pixels you specify to all sides. For example:

```c++
rect box(10, 10, 10, 10);
box.inflate(10);
```

Would inflate our `box` to start at 0,0 and be 30x30 pixels in size.

`deflate` does the opposite:

```c++
rect box(10, 10, 10, 10);
box.deflate(1);
```

Would deflate our `box` to start at `11,11` and be 8x8 pixels in size.

Since `rectangle` *always* draws a filled rectangle, this can be useful to add an outline of your desired thickness:

```c++
WHITE = screen.create_pen(255, 255, 255);
rect box(10, 10, 100, 100);
box.inflate(1); // Inflate our box by 1px on all sides
screen.set_pen(WHITE); // White outline
screen.rectangle(box);
box.deflate(1); // Return to our original box size
screen.set_pen(0, 0, 0); /// Black fill
screen.rectangle(box);
```

#### point

The `point` type descrives a single point - synonymous with a pixel - in terms of its x and y position.

##### point.clamp

```c++
point point::clamp(const rect &r);
```

A point can be clamped within the confines of a `rect`. This is useful for keeping - for example - a cursor within the bounds of the screen:

```c++
point cursor(10, 1000);       // A point, far outside the bounds of our screen
cursor.clamp(screen.bounds);  // Clamp to the screen
```

### Pens & Clipping

#### set_pen

In order to draw anything with Pico Graphics you must first set the pen to your desired palette colour:

```c++
void PicoGraphics::set_pen(uint8_t p);
```

This value represents an index into the internal colour palette, which has 256 entries and defaults to RGB332 giving an approximation of all RGB888 colours.


#### create_pen

```c++
int PicoGraphics::create_pen(uint8_t r, uint8_t g, uint8_t b);
```

By default create pen takes R, G and B values, clamps them to 3, 3 and 2 bits respectively and returns an index in the RGB332 palette.

You must create pens before using them with `set_pen()` which accepts only a palette index.

#### set_clip & remove_clip

```c++
void PicoGraphics::set_clip(const rect &r);
void PicoGraphics::remove_clip();
```

`set_clip` applies a clipping rectangle to the drawing surface. Any pixels outside of this rectangle will not be drawn. By default drawing operations are clipped to `bounds` since it's impossible to draw outside of the buffer.

`remove_clip` sets the surface clipping rectangle back to the surface `bounds`.

### Palette

By default Pico Graphics uses an `RGB332` palette and clamps all pens to their `RGB332` values so it can give you an approximate colour for every `RGB888` value you request. If you don't want to think about colours and palettes you can leave it as is.

Alternatively `set_palette_mode()` lets you switch into an RGB565 `USER` palette which gives you up to 256 16-bit colours of your choice.

#### update_pen

```c++
int PicoGraphics::update_pen(uint8_t index, uint8_t r, uint8_t g, uint8_t b);
```

Modify a palette entry to the given RGB colour (or nearest supported equivilent.)


#### reset_pen

```c++
void PicoGraphics::reset_pen(uint8_t index);
```

Return a palette entry to its default value. Usually black and marked unused.

### Pixels

#### pixel

```c++
void PicoGraphics::pixel(const point &p);
```

`pixel` sets the pixel at `point p` to the current `pen`.

#### pixel_span

```c++
void PicoGraphics::pixel_span(const point &p, int32_t l)
```

`pixel_span` draws a horizontal line of pixels of length `int32_t l` starting at `point p`.

### Primitives

#### rectangle

```c++
void PicoGraphics::rectangle(const rect &r) ;
```

`rectangle` draws a filled rectangle described by `rect`.

#### circle

```c++
PicoGraphics::circle(const point &p, int32_t radius) 
```

`circle` draws a filled circle centered on `point p` with radius `int32_t radius`.

### Text

```c++
void PicoGraphics::text(const std::string &t, const point &p, int32_t wrap, uint8_t scale);
```

`text` allows you to draw a string at `point p`, with a maximum line-width of `int32_t wrap`.

The 6x6 and 6x8 pixel font characters are encoded in `font6_data.hpp` and `font8_data.hpp` along with their character widths so that text can be drawn variable-width.

You can scale text with `uint8_t scale` for 12x12, 18x18, etc character sizes.

### Change Font

```c++
void PicoGraphics::set_font(const Font *font);
```


`set_font` allows you to change the font that PicoGraphics uses to draw text.

If you:

```c++
#include "font8_data.hpp"
```

Then you can: `set_font(&font8);` to use a font with upper/lowercase characters.