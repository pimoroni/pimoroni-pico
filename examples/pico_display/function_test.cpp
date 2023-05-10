#include "pico/stdlib.h"
#include "pico_display.hpp"
#include "drivers/st7789/st7789.hpp"
#include "libraries/pico_graphics/pico_graphics.hpp"
#include "rgbled.hpp"
#include "button.hpp"

using namespace pimoroni;

ST7789 st7789(PicoDisplay::WIDTH, PicoDisplay::HEIGHT, ROTATE_0, false, get_spi_pins(BG_SPI_FRONT));
PicoGraphics_PenRGB332 graphics(st7789.width, st7789.height, nullptr);

RGBLED led(PicoDisplay::LED_R, PicoDisplay::LED_G, PicoDisplay::LED_B);

int main() {

    // Set the backlight!
    st7789.set_backlight(255);

    // Pens
    Pen BLACK = graphics.create_pen(0, 0, 0);
    Pen GREEN = graphics.create_pen(0, 255, 0);
    Pen BLUE = graphics.create_pen(0, 0, 255);
    Pen RED = graphics.create_pen(255, 0, 0);
    Pen WHITE = graphics.create_pen(255, 255, 255);

    // Buttons
    Button button_a(12);
    Button button_b(13);
    Button button_x(14);
    Button button_y(15);

    // Title Screen
    graphics.set_pen(WHITE);
    graphics.clear();
    graphics.set_pen(BLACK);
    graphics.text("Pico Display Test", Point(10, 50), 4);


    // Checks buttons and updates screen
    while(true) {

        if(button_a.read()) {
            graphics.set_pen(GREEN);
            graphics.clear();
            graphics.set_pen(WHITE);
            graphics.character('A', Point(100, 50), 8);
            led.set_rgb(0, 255, 0);
        }

        if(button_b.read()) {
            graphics.set_pen(BLUE);
            graphics.clear();
            graphics.set_pen(WHITE);
            graphics.character('B', Point(100, 50), 8);
            led.set_rgb(0, 0, 255);
        }

        if(button_x.read()) {
            graphics.set_pen(WHITE);
            graphics.clear();
            graphics.set_pen(BLACK);
            graphics.character('X', Point(100, 50), 8);
            led.set_rgb(255,255,255);
        }

        if(button_y.read()) {
            graphics.set_pen(RED);
            graphics.clear();
            graphics.set_pen(WHITE);
            graphics.character('Y', Point(100, 50), 8);
            led.set_rgb(255, 0, 0);
        }


        st7789.update(&graphics);

    }

        return 0;
}