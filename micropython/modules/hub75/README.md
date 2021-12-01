# Interstate 75 <!-- omit in toc -->

The Interstate 75 library is intended for the Interstate 75 "HUB75" matrix panel driver board.

It can, in theory, be used with your own custom wiring, though custom pin assignments are not supported yet.

- [Notes On PIO Limitations](#notes-on-pio-limitations)
- [Getting Started](#getting-started)
  - [FM6216A Panels](#fm6216a-panels)
- [Set A Pixel](#set-a-pixel)
  - [RGB](#rgb)
  - [HSV](#hsv)

## Notes On PIO Limitations

The Interstate 75 driver uses the PIO hardware on the RP2040. There are only two PIOs with four state machines each, and i75 uses one of these (`PIO0`) and two state machines- one for clocking out pixels, and another for latching/lighting a row.

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

matrix = hub75.Hub75(WIDTH, HEIGHT, panel_type=hub75.PANEL_FM6126A)
```

## Set A Pixel

You can set the colour of an pixel in either the RGB colourspace, or HSV (Hue, Saturation, Value). HSV is useful for creating rainbow patterns.

### RGB

Set the top left-most LED - `0, 0` - to Purple `255, 0, 255`:

```python
matrixx.set_rgb(0, 0, 255, 0, 255)
```

### HSV

Set the top left-most LED - `0, o` - to Red `0.0`:

```python
matrix.set_hsv(0, 0, 0.0, 1.0, 1.0)
```
