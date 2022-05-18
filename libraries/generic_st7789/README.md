# Genereic ST7789 - Pico Display Pack & Pico Display Pack 2.0" and 240x240 Square & Round LCD Breakouts <!-- omit in toc -->

Our Pico Display Packs offers a vibrant 1.14" (240x135) or 2.0" (320x240) IPS LCD screen for your Raspberry Pi Pico it also includes four switches and and an RGB LED!

We've included helper functions to handle every aspect of drawing to the screen and interfacing with the buttons and LED. See the [function reference](#function-reference) for details.

- [Example Program](#example-program)
- [Function Reference](#function-reference)
  - [PicoGraphics](#picographics)
  - [set_backlight](#set_backlight)
  - [update](#update)

## Example Program

The following example sets up Pico Display, displays some basic demo text and graphics and will illuminate the RGB LED green if the A button is pressed.

```c++
#include "pico_display.hpp"
#include "generic_st7789.hpp"
#include "rgbled.hpp"
#include "button.hpp"

using namespace pimoroni;

const bool ROTATE_180 = false;

uint16_t buffer[PicoDisplay::WIDTH * PicoDisplay::HEIGHT];

// Swap WIDTH and HEIGHT to rotate 90 degrees
ST7789Generic pico_display(PicoDisplay::WIDTH, PicoDisplay::HEIGHT, buffer);

// RGB LED controller
RGBLED led(PicoDisplay::LED_R, PicoDisplay::LED_G, PicoDisplay::LED_B);

// Buttons
Button button_a(PicoDisplay::A);
Button button_b(PicoDisplay::B);
Button button_x(PicoDisplay::X);
Button button_y(PicoDisplay::Y);

int main() {
    pico_display.configure_display(ROTATE_180);

    // set the backlight to a value between 0 and 255
    // the backlight is driven via PWM and is gamma corrected by our
    // library to give a gorgeous linear brightness range.
    pico_display.set_backlight(100);

    while(true) {
        // detect if the A button is pressed (could be A, B, X, or Y)
        if(button_a.raw()) {
            // make the led glow green
            // parameters are red, green, blue all between 0 and 255
            // these are also gamma corrected
            led.set_rgb(0, 255, 0);
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
        pico_display.set_pen(110, 120, 130);
        pico_display.text("This is a message", Point(text_rect.x, text_rect.y), text_rect.w);

        // now we've done our drawing let's update the screen
        pico_display.update();
    }
}
```

## Function Reference

### PicoGraphics

The generic ST7789 driver uses our Pico Graphics library to draw graphics and text. For more information [read the Pico Graphics function reference.](../pico_graphics/README.md#function-reference).

You will also need to use the RGBLED library to drive the RGB LED, and the Button library for the four buttons.

### set_backlight

Set the display backlight from 0-255.

```c++
pico_display.set_backlight(brightness);
```

Uses hardware PWM to dim the display backlight, dimming values are gamma-corrected to provide smooth brightness transitions across the full range of intensity. This may result in some low values mapping as "off."

### update

To display your changes on Pico Display's screen you need to call `update`:

```c++
pico_display.update();
```
