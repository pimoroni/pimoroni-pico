# Interstate 75 (C++) <!-- omit in toc -->

This library offers convenient functions for interacting with [Interstate75](https://shop.pimoroni.com/products/interstate-75) and [Interstate75W](https://shop.pimoroni.com/products/interstate-75-w) - Interstate75 and Interstate75W offer an convenient way and 2 input buttons for all your display and control needs.

- [Example Program](#example-program)
- [Function Reference](#function-reference)
  - [PicoGraphics](#picographics)
  - [HUB75](#hub75)

## Example Program

The following example sets up Pico Display, displays some basic demo text and graphics and will illuminate the LED green if the A button is pressed.

```c++
#include "libraries/pico_graphics/pico_graphics.hpp"
#include "libraries/interstate75/interstate75.hpp"
#include "rgbled.hpp"
#include "button.hpp"

using namespace pimoroni;

// Display driver for a single 32x32 hub75 matrix
Hub75 hub75(32, 32, nullptr, PANEL_GENERIC, false);

// Graphics library - in 24Bit mode with 16M colours
PicoGraphics_PenRGB888 graphics(hub75.width, hub75.height, nullptr);

// And each button
Button button_a(Interstate75::A);
// For the Interstate75
Button button_b(Interstate75::BOOT);
// Or for the Interstate75W
Button button_b(Interstate75::B);

// RGB LED
RGBLED led(Interstate75::LED_R, Interstate75::LED_G, Interstate75::LED_B);

// Interrupt callback required function 
void __isr dma_complete() {
  hub75.dma_complete();
}

int main() {
    hub75.start(dma_complete);

    while(true) {
        // detect if the A button is pressed (could be A, B, C, D or E)
        if(button_a.raw()) {
            // make the led glow green
            // parameters are red, green, blue all between 0 and 255
            // these are also gamma corrected
            led.set_rgb(0, 255, 0);
        }

        // set the colour of the pen
        // parameters are red, green, blue all between 0 and 255
        graphics.set_pen(100, 40, 50);

        // fill the screen with the current pen colour
        graphics.clear();

        // draw a box to put some text in
        graphics.set_pen(10, 20, 100);
        Rect text_rect(1, 1, hub75.width-2, hub75.height-2);
        graphics.rectangle(text_rect);

        // write some text inside the box with 10 pixels of margin
        // automatically word wrapping
        text_rect.deflate(1);
        graphics.set_pen(110, 120, 130);
        graphics.text("This is text", Point(text_rect.x, text_rect.y), text_rect.w, 1.0f);

        // now we've done our drawing let's update the screen
        hub75.update(&graphics);
    }
}
```

## Function Reference

### PicoGraphics

Pico GFX Pack uses our Pico Graphics library to draw graphics and text. For more information [read the Pico Graphics function reference.](../pico_graphics/README.md#function-reference).

### HUB75

Pico Display uses the HUB75 display driver to handle the led matrix. For more information [read the HUB75 README.](../../drivers/hub75/README.md).