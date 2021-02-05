# Pico Display Pack <!-- omit in toc -->

Our Pico Display Pack offers a vibrant 1.14" (240x135) IPS LCD screen for your Raspberry Pi Pico it also includes four switches and and an RGB LED!

We've included helper functions to handle every aspect of drawing to the screen and interfacing with the buttons and LED. See the [function reference](#function-reference) for details.

- [Example Program](#example-program)
- [Function Reference](#function-reference)
  - [PicoGraphics](#picographics)
  - [init](#init)
  - [set_backlight](#set_backlight)
  - [set_led](#set_led)
  - [is_pressed](#is_pressed)
  - [update](#update)

## Example Program

The following example sets up Pico Display, displays some basic demo text and graphics and will illuminate the RGB LED green if the A button is pressed.

```c++
#include "pico_display.hpp"

using namespace pimoroni;

uint16_t buffer[PicoDisplay::WIDTH * PicoDisplay::HEIGHT];
PicoDisplay pico_display(buffer);

int main() {
    pico_display.init();

    // set the backlight to a value between 0 and 255
    // the backlight is driven via PWM and is gamma corrected by our
    // library to give a gorgeous linear brightness range.
    pico_display.set_backlight(100);

    while(true) {
        // detect if the A button is pressed (could be A, B, X, or Y)
        if(pico_display.is_pressed(pico_display.A)) {
            // make the led glow green
            // parameters are red, green, blue all between 0 and 255
            // these are also gamma corrected
            pico_display.set_led(0, 255, 0);
        }

        // set the colour of the pen
        // parameters are red, green, blue all between 0 and 255
        pico_display.set_pen(30, 40, 50);

        // fill the screen with the current pen colour
        pico_display.clear();

        // draw a box to put some text in
        pico_display.set_pen(10, 20, 30);
        Rect text_rect(10, 10, 150, 150);
        pico_display.rectangle(text_rect);

        // write some text inside the box with 10 pixels of margin
        // automatically word wrapping
        text_rect.deflate(10);
        pico_display.text("This is a message", Point(text_rect.x, text_rect.y), text_rect.w);

        // now we've done our drawing let's update the screen
        pico_display.update();
    }
}
```

## Function Reference

### PicoGraphics

Pico Display uses our Pico Graphics library to draw graphics and text. For more information [read the Pico Graphics function reference.](../pico_graphics/README.md#function-reference).

### init

Sets up Pico Display. `init` must be called before any other functions since it configures the required PWM and GPIO:

```c++
pico_display.init();
```

### set_backlight

Set the display backlight from 0-255.

```c++
pico_display.set_backlight(brightness);
```

Uses hardware PWM to dim the display backlight, dimming values are gamma-corrected to provide smooth brightness transitions across the full range of intensity. This may result in some low values mapping as "off."

### set_led

Sets the RGB LED on Pico Display with an RGB triplet:

```c++
pico_display.set_led(r, g, b);
```

Uses hardware PWM to drive the LED. Values are automatically gamma-corrected to provide smooth brightness transitions and low values may map as "off."

### is_pressed

Reads the GPIO pin connected to one of Pico Display's buttons, returning a `bool` - `true` if it's pressed and `false` if it is released.

```c++
pico_display.is_pressed(button);
```

The button vaule should be a `uint8_t` denoting a pin, and constants `A`, `B`, `X` and `Y` are supplied to make it easier. e:

```c++
bool is_a_button_pressed = pico_display.is_pressed(PicoDisplay::A)
```

### update

To display your changes on Pico Display's screen you need to call `update`:

```c++
pico_display.update();
```
