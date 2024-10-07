#include "pico_graphics.hpp"

namespace pimoroni {
    PicoGraphics_PenRGB565::PicoGraphics_PenRGB565(uint16_t width, uint16_t height, void *frame_buffer, uint16_t layers)
    : PicoGraphics(width, height, layers, frame_buffer) {
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
        // We can't use buffer_size because our pointer is uint16_t
        buf += this->layer_offset;
        buf[p.y * bounds.w + p.x] = color;
    }
    void PicoGraphics_PenRGB565::set_pixel_span(const Point &p, uint l) {
        // pointer to byte in framebuffer that contains this pixel
        uint16_t *buf = (uint16_t *)frame_buffer;
        // We can't use buffer_size because our pointer is uint16_t
        buf += this->layer_offset;
        buf = &buf[p.y * bounds.w + p.x];

        while(l--) {
            *buf++ = color;
        }
    }
    void PicoGraphics_PenRGB565::frame_convert(PenType type, conversion_callback_func callback) {
        if(type == PEN_RGB565) {
            // Treat our void* frame_buffer as uint8_t
            uint16_t *src = (uint16_t *)frame_buffer;

            if(layers > 1) {
                // Assume only two layers for now
                // We can't use buffer_size because our pointer is uint16_t
                uint16_t *src_layer2 = src + this->bounds.w * this->bounds.h;

                frame_convert_rgb565(callback, [&]() {
                    RGB565 c1 = *src++;
                    RGB565 c2 = *src_layer2++;
                    return c2 ? c2 : c1;
                });
            } else {
                frame_convert_rgb565(callback, [&]() {
                    return *src++;
                });
            }
        }
    }
}