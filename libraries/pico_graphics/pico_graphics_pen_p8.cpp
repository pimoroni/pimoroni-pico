#include "pico_graphics.hpp"

namespace pimoroni {
    PicoGraphics_PenP8::PicoGraphics_PenP8(uint16_t width, uint16_t height, void *frame_buffer)
    : PicoGraphics(width, height, frame_buffer) {
        this->pen_type = PEN_P8;
        if(this->frame_buffer == nullptr) {
            this->frame_buffer = (void *)(new uint8_t[buffer_size(width, height)]);
        }
        for(auto i = 0u; i < 256; i++) {
            palette[i] = {0, 0, 0};
            used[i] = false;
        }
    }
    void PicoGraphics_PenP8::set_pen(uint c) {
        color = c;
    }
    void PicoGraphics_PenP8::set_pen(uint8_t r, uint8_t g, uint8_t b) {
        int pen = RGB(r, g, b).closest(palette, 16);
        if(pen != -1) color = pen;
    }
    int PicoGraphics_PenP8::update_pen(uint8_t i, uint8_t r, uint8_t g, uint8_t b) {
        i &= 0xff;
        palette[i] = {r, g, b};
        return i;
    }
    int PicoGraphics_PenP8::create_pen(uint8_t r, uint8_t g, uint8_t b) {
        // Create a colour and place it in the palette if there's space
        for(auto i = 0u; i < 256u; i++) {
            if(!used[i]) {
                palette[i] = {r, g, b};
                used[i] = true;
                return i;
            }
        }
        return -1;
    }
    int PicoGraphics_PenP8::reset_pen(uint8_t i) {
        palette[i] = {0, 0, 0};
        used[i] = false;
        return i;
    }
    void PicoGraphics_PenP8::set_pixel(const Point &p) {
        uint8_t *buf = (uint8_t *)frame_buffer;
        buf[p.y * bounds.w + p.x] = color;
    }
    void PicoGraphics_PenP8::scanline_convert(PenType type, conversion_callback_func callback) {
        if(type == PEN_RGB565) {
            // Cache the RGB888 palette as RGB565
            RGB565 cache[256];
            for(auto i = 0u; i < 256; i++) {
                cache[i] = palette[i].to_rgb565();
            }

            // Treat our void* frame_buffer as uint8_t
            uint8_t *src = (uint8_t *)frame_buffer;

            // Allocate a per-row temporary buffer
            uint16_t row_buf[bounds.w];
            for(auto y = 0; y < bounds.h; y++) {
                for(auto x = 0; x < bounds.w; x++) {
                    row_buf[x] = cache[src[bounds.w * y + x]];
                }
                // Callback to the driver with the row data
                callback(row_buf, bounds.w * sizeof(uint8_t));
            }
        }
    }
}