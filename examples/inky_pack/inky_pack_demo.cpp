#include <string.h>
#include <math.h>
#include <vector>
#include <cstdlib>

#include "drivers/uc8151/uc8151.hpp"
#include "libraries/pico_graphics/pico_graphics.hpp"
#include "button.hpp"


using namespace pimoroni;

enum Pin {
    A           = 12,
    B           = 13,
    C           = 14,
    CS          = 17,
    CLK         = 18,
    MOSI        = 19,
    DC          = 20,
    RESET       = 21,
    BUSY        = 26,
};


UC8151 uc8151(296, 128, ROTATE_0);
PicoGraphics_Pen1BitY graphics(uc8151.width, uc8151.height, nullptr);

Button button_a(Pin::A);
Button button_b(Pin::B);
Button button_c(Pin::C);


int main() {
    graphics.set_pen(0);
    graphics.clear();

    graphics.set_pen(15);
    graphics.set_font("bitmap8");
    graphics.text("Hello World", {0, 0}, 296);
    graphics.text("Has this worked?!", {0, 16}, 296);

    uc8151.update(&graphics);

    return 0;
}
