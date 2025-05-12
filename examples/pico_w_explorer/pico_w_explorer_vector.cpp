#include <string.h>
#include <math.h>
#include <vector>
#include <cstdlib>

#include "drivers/st7789/st7789.hpp"
#include "libraries/pico_graphics/pico_graphics.hpp"
#include "libraries/pico_vector/pico_vector.hpp"

using namespace pimoroni;


ST7789 st7789(320, 240, ROTATE_0, false, {PIMORONI_SPI_DEFAULT_INSTANCE, 17, SPI_DEFAULT_SCK, SPI_DEFAULT_MOSI, PIN_UNUSED, SPI_DEFAULT_MISO, 9});
PicoGraphics_PenRGB332 graphics(st7789.width, st7789.height, nullptr);
PicoVector vector(&graphics);

int main() {
  st7789.set_backlight(255);

  Pen WHITE = graphics.create_pen(255, 255, 255);
  Pen BLACK = graphics.create_pen(0, 0, 0);

  float angle = 0.0f;

  while(true) {
    graphics.set_pen(BLACK);
    graphics.clear();

    graphics.set_pen(WHITE);
    graphics.text("Hello World", Point(0, 0), 320);

    pp_point_t outline[] = {{-128, -128}, {128, -128}, {128, 128}, {-128, 128}};
    pp_point_t hole[]    = {{ -64,   64}, { 64,   64}, { 64, -64}, { -64, -64}};

    pp_poly_t *poly = pp_poly_new();
    pp_path_add_points(pp_poly_add_path(poly), outline, sizeof(outline) / sizeof(pp_point_t));
    pp_path_add_points(pp_poly_add_path(poly), hole, sizeof(hole) / sizeof(pp_point_t));

    vector.rotate(poly, {0, 0}, angle);
    vector.translate(poly, {160, 120});

    vector.draw(poly);

    //pp_mat3_t t = pp_mat3_identity();
    //vector.text("Hello World", {0, 0}, &t);

    // update screen
    st7789.update(&graphics);

    angle += 1.0f;

    pp_poly_free(poly);
  }

    return 0;
}
