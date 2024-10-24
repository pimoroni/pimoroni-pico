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


     void PicoGraphics_PenRGB565::sprite(void* data, const Point &sprite, const Point &dest, const int scale, const int transparent) {
         //int sprite_x = (sprite & 0x0f) << 3;
         //int sprite_y = (sprite & 0xf0) >> 1;
         Point s {
             sprite.x << 3,
             sprite.y << 3
         };
         RGB565 *ptr = (RGB565 *)data;
         Point o = {0, 0};
         for(o.y = 0; o.y < 8 * scale; o.y++) {
             Point so = {
                 0,
                 o.y / scale
             };
             for(o.x = 0; o.x < 8 * scale; o.x++) {
                 so.x = o.x / scale;
                 color = ptr[(s.y + so.y) * 128 + (s.x + so.x)];
                 if(color != transparent) pixel(dest + o);
             }
         }
     }
}
