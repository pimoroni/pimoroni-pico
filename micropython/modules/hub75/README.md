# HUB75 <!-- omit in toc -->

This library can be used with the Interstate 75 and 75W bypasses the use of picographics and can be used for situations where RAM is constrained or for custom display configurations like 2x2 HUB75 panels.

For most cases we recommend using the picographics based module for Interstate 75 and 75W as it contains a lot of helper functions to draw text and shapes, further information on its usage can be found here: [Interstate75](../../modules_py/interstate75.md) 

The Interstate 75 library is intended for the Interstate 75 and Interstate 75 W "HUB75" matrix panel driver board.

It can, in theory, be used with your own custom wiring, though custom pin assignments are not supported yet.

- [Notes On PIO \& DMA Limitations](#notes-on-pio--dma-limitations)
- [Getting Started](#getting-started)
  - [FM6216A Panels](#fm6216a-panels)
  - [Setting Colour Order](#setting-colour-order)
- [Quick Reference](#quick-reference)
  - [Set A Pixel](#set-a-pixel)
  - [Clear the display](#clear-the-display)

## Notes On PIO & DMA Limitations

The Hub 75 driver uses the PIO hardware on the RP2040. There are only two PIOs with four state machines each, and hub75 uses one of these (`PIO0`) and two state machines- one for clocking out pixels, and another for latching/lighting a row.

It also uses two DMA channels, one to copy pixel data from the back buffer back to the front buffer and one to supply the row driving PIO with row data.

## Getting Started

Construct a new `Hub75` instance, specifying the width/height of the display and any additional options.

```python
import hub75

WIDTH = 64
HEIGHT = 64

matrix = hub75.Hub75(WIDTH, HEIGHT, stb_invert=True)
```

Use `stb_invert` if you see a missing middle row corruption on the top row.

Start the matrix strip by calling start. This sets up DMA and PIO to drive your panel, pulling rows from the back buffer and refreshing as fast as it can.

```python
matrix.start()
```

### FM6216A Panels

Some panels - based on the FM6126A chips - require a couple of register settings for them to display anything at all. Interstate 75 will set these for you if you specify `panel_type=hub75.PANEL_FM6126A`. Eg:

```python
import hub75

WIDTH = 64
HEIGHT = 64

matrix = hub75.Hub75(WIDTH, HEIGHT, panel_type=hub75.PANEL_FM6126A)
```

### Setting Colour Order

Some hub 75 panels have varying colour orders. A keyword argument is supplied to configure this:

```python
matrix = hub75.Hub75(WIDTH, HEIGHT, color_order=hub75.COLOR_ORDER_RBG)
```

The available orders are defined as constants in `hub75`:

* `COLOR_ORDER_RGB`
* `COLOR_ORDER_RBG`
* `COLOR_ORDER_GRB`
* `COLOR_ORDER_GBR`
* `COLOR_ORDER_BRG`
* `COLOR_ORDER_BGR`

## Quick Reference

### Set A Pixel

You can set the colour of a pixel using RGB values. This will instantly update the pixel on the matrix display.

Set the top left-most LED - `0, 0` - to Purple `255, 0, 255`:

```python
matrix.set_pixel(0, 0, 255, 0, 255)
```

### Clear the display

Calling `.clear()` will clear the whole contents of the display

```python
matrix.clear()
```