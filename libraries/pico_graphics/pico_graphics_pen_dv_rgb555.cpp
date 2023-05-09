#include "pico_graphics.hpp"

namespace pimoroni {
    PicoGraphics_PenDV_RGB555::PicoGraphics_PenDV_RGB555(uint16_t width, uint16_t height, IDirectDisplayDriver<uint16_t> &direct_display_driver)
      : PicoGraphics(width, height, nullptr),
        driver(direct_display_driver) 
    {
        this->pen_type = PEN_DV_RGB555;
    }
    void PicoGraphics_PenDV_RGB555::set_pen(uint c) {
        color = c;
    }
    void PicoGraphics_PenDV_RGB555::set_pen(uint8_t r, uint8_t g, uint8_t b) {
        RGB src_color{r, g, b};
        color = src_color.to_rgb555(); 
    }
    int PicoGraphics_PenDV_RGB555::create_pen(uint8_t r, uint8_t g, uint8_t b) {
        return RGB(r, g, b).to_rgb555();
    }
    int PicoGraphics_PenDV_RGB555::create_pen_hsv(float h, float s, float v) {
        return RGB::from_hsv(h, s, v).to_rgb555();
    }
    void PicoGraphics_PenDV_RGB555::set_pixel(const Point &p) {
        driver.write_pixel(p, color);
    }
    void PicoGraphics_PenDV_RGB555::set_pixel_span(const Point &p, uint l) {
        driver.write_pixel_span(p, l, color);
    }
}