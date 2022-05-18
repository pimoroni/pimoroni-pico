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
#include "generic_st7789.hpp"
#include "rgbled.hpp"
#include "button.hpp"

uint16_t buffer[PicoDisplay::WIDTH * PicoDisplay::HEIGHT];

// Swap WIDTH and HEIGHT to rotate 90 degrees
ST7789Generic display(PicoDisplay::WIDTH, PicoDisplay::HEIGHT, buffer);

// Create an RGB LED
RGBLED led(PicoDisplay::LED_R, PicoDisplay::LED_G, PicoDisplay::LED_B);

// And each button
Button button_a(PicoDisplay::A);
Button button_b(PicoDisplay::B);
Button button_x(PicoDisplay::X);
Button button_y(PicoDisplay::Y);

int main() {

    // set the backlight to a value between 0 and 255
    // the backlight is driven via PWM and is gamma corrected by our
    // library to give a gorgeous linear brightness range.
    display.set_backlight(100);

    while(true) {
        // detect if the A button is pressed (could be A, B, X, or Y)
        if(button_a.raw(display.A)) {
            // make the led glow green
            // parameters are red, green, blue all between 0 and 255
            // these are also gamma corrected
            led.set_rgb(0, 255, 0);
        }

        // set the colour of the pen
        // parameters are red, green, blue all between 0 and 255
        display.set_pen(30, 40, 50);

        // fill the screen with the current pen colour
        display.clear();

        // draw a box to put some text in
        display.set_pen(10, 20, 30);
        Rect text_rect(10, 10, 150, 150);
        display.rectangle(text_rect);

        // write some text inside the box with 10 pixels of margin
        // automatically word wrapping
        text_rect.deflate(10);
        display.set_pen(110, 120, 130);
        display.text("This is a message", Point(text_rect.x, text_rect.y), text_rect.w);

        // now we've done our drawing let's update the screen
        display.update();
    }
}
```

## Function Reference

### PicoGraphics

Pico Display uses our Pico Graphics library to draw graphics and text. For more information [read the Pico Graphics function reference.](../pico_graphics/README.md#function-reference).

### configure_display

Configures an ST7789 display. Done by default, but you can use this to set 180 degree rotation like so:

```c++
display.configure_display(true);
```

### flip

Deprecated: calls `configure_display(true);`

### set_backlight

Set the display backlight from 0-255.

```c++
display.set_backlight(brightness);
```

Uses hardware PWM to dim the display backlight, dimming values are gamma-corrected to provide smooth brightness transitions across the full range of intensity. This may result in some low values mapping as "off."

### update

To display your changes on Pico Display's screen you need to call `update`:

```c++
display.update();
```
