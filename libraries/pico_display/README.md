# Pico Display Pack <!-- omit in toc -->

Our Pico Display Pack offers a vibrant 1.14" (240x135) IPS LCD screen for your Raspberry Pi Pico it also includes four switches and and an RGB LED!

We've included helper functions to handle every aspect of drawing to the screen and interfacing with the buttons and LED. See the [function reference](#function-reference) for details.

- [Example Program](#example-program)
- [Function Reference](#function-reference)
  - [PicoGraphics](#picographics)
  - [ST7789](#st7789)

## Example Program

The following example sets up Pico Display, displays some basic demo text and graphics and will illuminate the RGB LED green if the A button is pressed.

```c++
#include "pico_display.hpp"
#include "drivers/st7789/st7789.hpp"
#include "libraries/pico_graphics/pico_graphics.hpp"
#include "rgbled.hpp"
#include "button.hpp"

// Display driver
ST7789 st7789(PicoDisplay::WIDTH, PicoDisplay::HEIGHT, ROTATE_0, false, get_spi_pins(BG_SPI_FRONT));

// Graphics library - in RGB332 mode you get 256 colours and optional dithering for ~32K RAM.
PicoGraphics_PenRGB332 graphics(st7789.width, st7789.height, nullptr);

// RGB LED
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
    st7789.set_backlight(100);

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
        graphics.set_pen(30, 40, 50);

        // fill the screen with the current pen colour
        graphics.clear();

        // draw a box to put some text in
        graphics.set_pen(10, 20, 30);
        Rect text_rect(10, 10, 150, 150);
        graphics.rectangle(text_rect);

        // write some text inside the box with 10 pixels of margin
        // automatically word wrapping
        text_rect.deflate(10);
        graphics.set_pen(110, 120, 130);
        graphics.text("This is a message", Point(text_rect.x, text_rect.y), text_rect.w);

        // now we've done our drawing let's update the screen
        st7789.update(&graphics);
    }
}
```

## Function Reference

### PicoGraphics

Pico Display uses our Pico Graphics library to draw graphics and text. For more information [read the Pico Graphics function reference.](../pico_graphics/README.md#function-reference).

### ST7789

Pico Display uses the ST7789 display driver to handle the LCD. For more information [read the ST7789 README.](../../drivers/st7789/README.md).