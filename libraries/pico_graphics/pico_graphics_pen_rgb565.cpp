#include "pico_graphics.hpp"

namespace pimoroni {
    PicoGraphics_PenRGB565::PicoGraphics_PenRGB565(uint16_t width, uint16_t height, void *frame_buffer)
    : PicoGraphics(width, height, frame_buffer) {
        this->pen_type = PEN_RGB565;
        if(this->frame_buffer == nullptr) {
            this->frame_buffer = (void *)(new uint8_t[buffer_size(width, height)]);
        }
    }
    void PicoGraphics_PenRGB565::set_pen(uint c) {
        color = c;
    }
    void PicoGraphics_PenRGB565::set_pen(uint8_t r, uint8_t g, uint8_t b) {
        src_color = {r, g, b};
        color = src_color.to_rgb565(); 
    }
    int PicoGraphics_PenRGB565::create_pen(uint8_t r, uint8_t g, uint8_t b) {
        return RGB(r, g, b).to_rgb565();
    }
    int PicoGraphics_PenRGB565::create_pen_hsv(float h, float s, float v) {
        return RGB::from_hsv(h, s, v).to_rgb565();
    }
    void PicoGraphics_PenRGB565::set_pixel(const Point &p) {
        uint16_t *buf = (uint16_t *)frame_buffer;
        buf[p.y * bounds.w + p.x] = color;
    }
    void PicoGraphics_PenRGB565::set_pixel_span(const Point &p, uint l) {
        // pointer to byte in framebuffer that contains this pixel
        uint16_t *buf = (uint16_t *)frame_buffer;
        buf = &buf[p.y * bounds.w + p.x];

        while(l--) {
            *buf++ = color;
        }
    }
}