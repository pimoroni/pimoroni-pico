# Pico Unicorn Pack - MicroPython <!-- omit in toc -->

Our Pico Unicorn Pack offers 7x17 bright RGB LEDs driven by Pico's PIO.

Pico Unicorn uses SM0 of PIO0.

We've included helper functions to handle every aspect of drawing to the display and interfacing with the buttons. See the [function reference](#function-reference) for details.

- [Example Program](#example-program)
- [Reference](#reference)
  - [Constants](#constants)
    - [Buttons](#buttons)
    - [WIDTH / HEIGHT](#width--height)
  - [Functions](#functions)
    - [set\_pixel](#set_pixel)
    - [is\_pressed](#is_pressed)

## Example Program

The following example sets up Pico Unicorn, displays some basic demo text and graphics and will illuminate the RGB LED green if the A button is pressed.

```c++

```

## Reference

### Constants

#### Buttons

The four buttons, A, B, X and Y have correponding constants set to their respective GPIO pins. For example:

```c++
bool a_is_pressed = pico_unicorn.is_pressed(pico_unicorn.A);
```

#### WIDTH / HEIGHT

The width and height of Pico Unicorn are available in constants `WIDTH` and `HEIGHT`.

For example:

```c++
int num_pixels = pico_unicorn.WIDTH * pico_unicorn.HEIGHT;
```

### Functions

#### set_pixel

```c++
void set_pixel(uint8_t x, uint8_t y, uint8_t r, uint8_t g, uint8_t b);
void set_pixel(uint8_t x, uint8_t y, uint8_t v);
```

Sets an RGB LED on Pico Unicorn with an RGB triplet:

```c++
pico_unicorn.set_pixel(x, y, r, g, b);
```

Uses hardware PWM to drive the LED. Values are automatically gamma-corrected to provide smooth brightness transitions and low values may map as "off."

Alternatively you can use:

```c++
pico_unicorn.set_pixel(x, y, v);
```

Which sets the R, G and B elements of the pixel to the same value- lighting it up white at your chosen intensity.

#### is_pressed

```c++
bool is_pressed(uint8_t button);
```

Reads the GPIO pin connected to one of Pico Unicorn's buttons, returning a `bool` - `true` if it's pressed and `false` if it is released.

```c++
pico_unicorn.is_pressed(button);
```

The button vaule should be a `uint8_t` denoting a pin, and constants `A`, `B`, `X` and `Y` are supplied to make it easier. e:

```c++
bool is_a_button_pressed = pico_unicorn.is_pressed(PicoUnicorn::A)
```