#include <string.h>
#include <math.h>
#include <vector>
#include <cstdlib>

#include "libraries/pico_display_2/pico_display_2.hpp"
#include "drivers/st7789/st7789.hpp"
#include "libraries/pico_graphics/pico_graphics.hpp"
#include "libraries/pico_vector/pico_vector.hpp"


using namespace pimoroni;

ST7789 st7789(320, 240, ROTATE_180, false, get_spi_pins(BG_SPI_FRONT));
PicoGraphics_PenRGB332 graphics(st7789.width, st7789.height, nullptr);

uint8_t vector_mem[PicoVector::pretty_poly_buffer_size()];

PicoVector vector(&graphics);

extern char _binary_DynaPuff_Medium_af_start[];
extern size_t _binary_DynaPuff_Medium_af_size;

int main() {
    stdio_init_all();

    Pen BG = graphics.create_pen(120, 40, 60);
    Pen TEXT = graphics.create_pen(255, 255, 255);

    st7789.set_backlight(255);

    vector.set_font(_binary_DynaPuff_Medium_af_start, 30);

    unsigned int a = 0;

    while (true) {
        Point text_location(0, 0);
        graphics.set_pen(BG);
        graphics.clear();
        graphics.set_pen(TEXT);
        graphics.text("Hello World", text_location, 320);

        pp_point_t outline[] = {{-64, -64}, {64, -64}, {64, 64}, {-64, 64}};
        pp_point_t hole[]    = {{ -32,   32}, { 32,   32}, { 32, -32}, { -32, -32}};

        pp_poly_t *poly = pp_poly_new();
        pp_path_add_points(pp_poly_add_path(poly), outline, sizeof(outline) / sizeof(pp_point_t));
        pp_path_add_points(pp_poly_add_path(poly), hole, sizeof(hole) / sizeof(pp_point_t));

        pp_mat3_t pos = pp_mat3_identity();
        pp_mat3_translate(&pos, 50, 50);
        pp_mat3_rotate(&pos, a);
        vector.draw(poly);
        vector.text("Hello World", 320, 240, &pos);

        // update screen
        st7789.update(&graphics);
        a += 1;
        if (a > 359) {
            a = 0;
        }

        pp_poly_free(poly);
    }

    return 0;
}
