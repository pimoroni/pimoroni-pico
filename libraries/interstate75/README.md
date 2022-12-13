# Pico GFX Pack (C++) <!-- omit in toc -->

This library offers convenient functions for interacting with [Pico GFX Pack](https://shop.pimoroni.com/products/gfxpack) - The Pico GFX Pack adds a 128x64 LCD Matrix display to your headered Raspberry Pi Pico or PicoW, with RGBW backlight and 5 input buttons for all your display and control needs.

- [Example Program](#example-program)
- [Function Reference](#function-reference)
  - [PicoGraphics](#picographics)
  - [ST7789](#st7789)

## Example Program

The following example sets up Pico Display, displays some basic demo text and graphics and will illuminate the backlight green if the A button is pressed.

```c++
#include "gfx_pack.hpp"
#include "drivers/st7567/st7576.hpp"
#include "libraries/pico_graphics/pico_graphics.hpp"
#include "rgbled.hpp"
#include "button.hpp"

// Display driver
ST7567 st7567(128, 64, GfxPack::gfx_pack_pins);

// Graphics library - in 1 Bit mode you get 16 shades with dithering.
PicoGraphics_Pen1Bit graphics(st7567.width, st7567.height, nullptr);

// RGB backlight elements
RGBLED backlight_rgb(GfxPack::BL_R, GfxPack::BL_G, GfxPack::BL_B, Polarity::ACTIVE_HIGH);

// And each button
Button button_a(GfxPack::A);
Button button_b(GfxPack::B);
Button button_c(GfxPack::C);
Button button_d(GfxPack::D);
Button button_e(GfxPack::E);


// RGB LED
RGBLED led(GfxPack::LED_R, GfxPack::LED_G, GfxPack::LED_B);

int main() {

    // set the backlight to a value between 0 and 255
    // This controls the white elements of the RGBW backlight
    st7567.set_backlight(100);

    while(true) {
        // detect if the A button is pressed (could be A, B, C, D or E)
        if(button_a.raw(display.A)) {
            // make the LCD glow green
            // parameters are red, green, blue all between 0 and 255
            // these are also gamma corrected
            backlight_rgb.set_rgb(0, 255, 0);
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
        st7567.update(&graphics);
    }
}
```

## Function Reference

### PicoGraphics

Pico GFX Pack uses our Pico Graphics library to draw graphics and text. For more information [read the Pico Graphics function reference.](../pico_graphics/README.md#function-reference).

### ST7567

Pico Display uses the ST7567 display driver to handle the LCD. For more information [read the ST7567 README.](../../drivers/st7789/README.md).