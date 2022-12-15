# Interstate 75 <!-- omit in toc -->

The Interstate 75 library is intended for the Interstate 75 "HUB75" matrix panel driver board.

It can, in theory, be used with your own custom wiring, though custom pin assignments are not supported yet.

- [Notes On PIO & DMA Limitations](#notes-on-pio--dma-limitations)
- [Getting Started](#getting-started)
  - [FM6216A Panels](#fm6216a-panels)
- [Quick Reference](#quick-reference)
  - [Set A Pixel](#set-a-pixel)
  - [Update The Display](#update-the-display)

## Notes On PIO & DMA Limitations

The Interstate 75 driver uses the PIO hardware on the RP2040. There are only two PIOs with four state machines each, and i75 uses one of these (`PIO0`) and two state machines- one for clocking out pixels, and another for latching/lighting a row.

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

Start the matrix strip by calling `start`. This sets up DMA and PIO to drive your panel, pulling rows from the back buffer and refreshing as fast as it can.

```python
matrix.start()
```

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

You can set the colour of a pixel using RGB values.

Set the top left-most LED - `0, 0` - to Purple `255, 0, 255`:

```python
matrix.set_pixel(0, 0, 255, 0, 255)
```

### Update The Display

You can update the back buffer - the framebuffer used by the driver to drive the screen - by calling `flip`:

```python
matrix.flip()
```

`flip` will swap the front buffer (the one you draw into) with the back buffer (the one the screen is refreshed from) so that the display can start drawing your changes immediately.

Since the back buffer contains your *previous* frame it then blocks and copies across the contents of the buffer you've just flipped.

If you want fresh, clear buffer to draw into at the start of your next frame you can use `flip_and_clear` instead:

```python
background_color = hub75.color(0, 0, 0)
matrix.flip_and_clear(background_color)
```

This will fill your buffer with the background colour, so you don't need to call `clear`.
