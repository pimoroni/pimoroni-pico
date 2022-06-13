#include "pico_graphics.hpp"

namespace pimoroni {
    PicoGraphics_PenP4::PicoGraphics_PenP4(uint16_t width, uint16_t height, void *frame_buffer)
    : PicoGraphics(width, height, frame_buffer) {
        this->pen_type = PEN_P4;
        if(this->frame_buffer == nullptr) {
            this->frame_buffer = (void *)(new uint8_t[buffer_size(width, height)]);
        }
        for(auto i = 0u; i < 16; i++) {
            palette[i] = {
                uint8_t(i << 4),
                uint8_t(i << 4),
                uint8_t(i << 4)
            };
        }
    }
    void PicoGraphics_PenP4::set_pen(uint c) {
        color = c & 0xf;
     }
    void PicoGraphics_PenP4::set_pen(uint8_t r, uint8_t g, uint8_t b) {
        int pen = RGB(r, g, b).closest(palette, 16);
        if(pen != -1) color = pen;
    }
    int PicoGraphics_PenP4::update_pen(uint8_t i, uint8_t r, uint8_t g, uint8_t b) {
        i &= 0xf;
        palette[i] = {r, g, b};
        return i;
    }
    void PicoGraphics_PenP4::set_pixel(const Point &p) {
        // pointer to byte in framebuffer that contains this pixel
        uint8_t *buf = (uint8_t *)frame_buffer;
        uint8_t *f = &buf[(p.x / 2) + (p.y * bounds.w / 2)];

        uint8_t  o = (~p.x & 0b1) * 4; // bit offset within byte
        uint8_t  m = ~(0b1111 << o);   // bit mask for byte
        uint8_t  b = color << o;       // bit value shifted to position

        *f &= m; // clear bits
        *f |= b; // set value
    }
    void PicoGraphics_PenP4::set_pixel_dither(const Point &p, const RGB &c) {
        if(!bounds.contains(p)) return;
        static uint pattern[16] = // dither pattern
            {0, 8, 2, 10, 12, 4, 14, 6, 3, 11, 1, 9, 15, 7, 13, 5};

        static std::array<uint8_t, 16> candidates;
        get_dither_candidates(c, palette, 256, candidates);

        // find the pattern coordinate offset
        uint pattern_index = (p.x & 0b11) | ((p.y & 0b11) << 2);

        // set the pixel
        color = candidates[pattern[pattern_index]];
        set_pixel(p);
    }
    void PicoGraphics_PenP4::scanline_convert(PenType type, conversion_callback_func callback) {
        if(type == PEN_RGB565) {
            // Cache the RGB888 palette as RGB565
            RGB565 cache[16];
            for(auto i = 0u; i < 16; i++) {
                cache[i] = palette[i].to_rgb565();
            }

            // Treat our void* frame_buffer as uint8_t
            uint8_t *src = (uint8_t *)frame_buffer;

            // Allocate a per-row temporary buffer
            uint16_t row_buf[bounds.w];
            for(auto y = 0; y < bounds.h; y++) {
                /*if(scanline_interrupt != nullptr) {
                    scanline_interrupt(y);
                    // Cache the RGB888 palette as RGB565
                    for(auto i = 0u; i < 16; i++) {
                    cache[i] = palette[i].to_rgb565();
                    }
                }*/

                for(auto x = 0; x < bounds.w; x++) {
                    uint8_t c = src[(bounds.w * y / 2) + (x / 2)];
                    uint8_t  o = (~x & 0b1) * 4; // bit offset within byte
                    uint8_t  b = (c >> o) & 0xf; // bit value shifted to position
                    row_buf[x] = cache[b];
                }
                // Callback to the driver with the row data
                callback(row_buf, bounds.w * sizeof(RGB565));
            }
        }
    }
}