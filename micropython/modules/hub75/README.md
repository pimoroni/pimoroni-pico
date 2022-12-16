# Interstate 75 <!-- omit in toc -->

This library bypasses the use of picographics an can be used for situation where RAM is constrained or custom display configurations like 2x2 HUB75 panels.

For most cases we recommend using the picographics based module for Interstate 75 and 75W as i t contain alot of helper functions to draw text and shapes, further information on its usage can be found here: [Interstate75](../../modules_py/interstate75.md) 

The Interstate 75 library is intended for the Interstate 75 and Interstate 75 W "HUB75" matrix panel driver board.

It can, in theory, be used with your own custom wiring, though custom pin assignments are not supported yet.

- [Notes On PIO & DMA Limitations](#notes-on-pio--dma-limitations)
- [Getting Started](#getting-started)
  - [FM6216A Panels](#fm6216a-panels)
- [Quick Reference](#quick-reference)
  - [Set A Pixel](#set-a-pixel)
  - [Clear The Display](#clear-the-display)

## Notes On PIO & DMA Limitations

The Hub 75 driver uses the PIO hardware on the RP2040. There are only two PIOs with four state machines each, and hub75 uses one of these (`PIO0`) and two state machines- one for clocking out pixels, and another for latching/lighting a row.

It also uses two DMA channels, one to copy pixel data from the back buffer back to the front buffer and one to supply the row driving PIO with row data.

## Getting Started

Contruct a new `Hub75` instance, specifying the width/height of the display and any additional options.

```python
import hub75

WIDTH = 64
HEIGHT = 64

matrix = hub75.Hub75(WIDTH, HEIGHT, stb_invert=True)
```

Use `stb_invert` if you see a missing middle row corruption on the top row.

### FM6216A Panels

Some panels - based on the FM6126A chips - require a couple of register settings in order for them to display anything at all. Interstate 75 will set these for you if you specify `panel_type=hub75.PANEL_FM6126A`. Eg:

```python
import hub75

WIDTH = 64
HEIGHT = 64

matrix = hub75.Hub75(WIDTH, HEIGHT,panel_type=hub75.PANEL_FM6126A)
```

## Quick Reference

### Set A Pixel

You can set the colour of a pixel using RGB values. This will instanly update the pixel on the matrix display.

Set the top left-most LED - `0, 0` - to Purple `255, 0, 255`:

```python
matrix.set_pixel(0, 0, 255, 0, 255)
```

### Clear the display

Calling `.clear()` this will clear the whole contents of the display

```python

matrix.clear()
```