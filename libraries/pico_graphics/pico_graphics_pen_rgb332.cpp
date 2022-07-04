#include "pico_graphics.hpp"
#include <string.h>

namespace pimoroni {
    PicoGraphics_PenRGB332::PicoGraphics_PenRGB332(uint16_t width, uint16_t height, void *frame_buffer)
    : PicoGraphics(width, height, frame_buffer) {
        this->pen_type = PEN_RGB332;
        if(this->frame_buffer == nullptr) {
            this->frame_buffer = (void *)(new uint8_t[buffer_size(width, height)]);
        }
    }
    void PicoGraphics_PenRGB332::set_pen(uint c) {
        color = c;
    }
    void PicoGraphics_PenRGB332::set_pen(uint8_t r, uint8_t g, uint8_t b) {
        color = rgb_to_rgb332(r, g, b);
    }
    int PicoGraphics_PenRGB332::create_pen(uint8_t r, uint8_t g, uint8_t b) {
        return rgb_to_rgb332(r, g, b);
    }
    void PicoGraphics_PenRGB332::set_pixel(const Point &p) {
        uint8_t *buf = (uint8_t *)frame_buffer;
        buf[p.y * bounds.w + p.x] = color;
    }
    void PicoGraphics_PenRGB332::set_pixel_span(const Point &p, uint l) {
        // pointer to byte in framebuffer that contains this pixel
        uint8_t *buf = (uint8_t *)frame_buffer;
        buf = &buf[p.y * bounds.w + p.x];

        while(l--) {
            *buf++ = color;
        }
    }
    void PicoGraphics_PenRGB332::set_pixel_dither(const Point &p, const RGB &c) {
        if(!bounds.contains(p)) return;
        uint8_t _dmv = dither16_pattern[(p.x & 0b11) | ((p.y & 0b11) << 2)];

        uint8_t red = c.r & 0b11000000;        // Two bits red
        uint8_t red_r = c.r & 0b111111;        // Remaining six bits red
        red_r >>= 2;                           // Discard down to four bit

        uint8_t grn = (c.g & 0b11000000) >> 3; // Two bits green
        uint8_t grn_r = c.g & 0b111111;        // Remaining six bits green
        grn_r >>= 2;                           // Discard down to four bit

        uint8_t blu = (c.b & 0b10000000) >> 6; // One bit blue
        uint8_t blu_r = c.b & 0b1111111;       // Remaining seven bits green
        blu_r >>= 3;                           // Discard down to four bit

        color = red | grn | blu;
        if(red_r > _dmv) color |= 0b00100000;
        if(grn_r > _dmv) color |= 0b00000100;
        if(blu_r > _dmv) color |= 0b00000001;

        set_pixel(p);
    }
    void PicoGraphics_PenRGB332::set_pixel_dither(const Point &p, const RGB565 &c) {
        if(!bounds.contains(p)) return;
        RGB565 cs = __builtin_bswap16(c);

        uint8_t _dmv = dither16_pattern[(p.x & 0b11) | ((p.y & 0b11) << 2)];

        //                      RRRRRGGGGGGBBBBB
        uint8_t red   = (cs & 0b1100000000000000) >> 8;  // Two bits grn
        uint8_t red_r = (cs & 0b0011100000000000) >> 10; // Four bits cmp

        uint8_t grn   = (cs & 0b0000011000000000) >> 6;  // Two bit grn
        uint8_t grn_r = (cs & 0b0000000111100000) >> 5;  // Four bit cmp

        uint8_t blu   = (cs & 0b0000000000010000) >> 3;  // Two bit blu
        uint8_t blu_r = (cs & 0b0000000000001111);       // Four bit cmp

        color = red | grn | blu;
        //                          RRRGGGBB
        if(red_r > _dmv) color |= 0b00100000;
        if(grn_r > _dmv) color |= 0b00000100;
        if(blu_r > _dmv) color |= 0b00000001;

        set_pixel(p);
    }
    void PicoGraphics_PenRGB332::scanline_convert(PenType type, conversion_callback_func callback) {
        if(type == PEN_RGB565) {

            // Treat our void* frame_buffer as uint8_t
            uint8_t *src = (uint8_t *)frame_buffer;

            // Allocate a per-row temporary buffer
            uint16_t row_buf[bounds.w];
            for(auto y = 0; y < bounds.h; y++) {
                for(auto x = 0; x < bounds.w; x++) {
                    row_buf[x] = rgb332_to_rgb565_lut[*src];

                    src++;
                }
                // Callback to the driver with the row data
                callback(row_buf, bounds.w * sizeof(RGB565));
            }
        }
    }
    void PicoGraphics_PenRGB332::sprite(void* data, const Point &sprite, const Point &dest, const int scale, const int transparent) {
        //int sprite_x = (sprite & 0x0f) << 3;
        //int sprite_y = (sprite & 0xf0) >> 1;
        Point s {
            sprite.x << 3,
            sprite.y << 3
        };
        RGB332 *ptr = (RGB332 *)data;
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