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
    void PicoGraphics_PenRGB565::set_pixel_alpha(const Point &p, const uint8_t a) {
        if(!bounds.contains(p)) return;

        uint16_t *buf = (uint16_t *)frame_buffer;

        RGB565 blended = RGB(buf[p.y * bounds.w + p.x]).blend(RGB(color), a).to_rgb565();

        buf[p.y * bounds.w + p.x] = blended;
    };

    bool PicoGraphics_PenRGB565::render_tile(const Tile *tile) {
        for(int y = 0; y < tile->h; y++) {
            uint8_t *palpha = &tile->data[(y * tile->stride)];
            uint16_t *pdest = &((uint16_t *)frame_buffer)[tile->x + ((tile->y + y) * bounds.w)];
            for(int x = 0; x < tile->w; x++) {
                uint16_t dest = *pdest;
                uint8_t alpha = *palpha;

                if(alpha == 255) {
                  *pdest = color;
                }else if(alpha == 0) {
                }else{
                  // blend tha pixel
                  uint16_t sr = (__builtin_bswap16(color) & 0b1111100000000000) >> 11;
                  uint16_t sg = (__builtin_bswap16(color) & 0b0000011111100000) >> 5;
                  uint16_t sb = (__builtin_bswap16(color) & 0b0000000000011111);

                  uint16_t dr = (__builtin_bswap16(dest) & 0b1111100000000000) >> 11;
                  uint16_t dg = (__builtin_bswap16(dest) & 0b0000011111100000) >> 5;
                  uint16_t db = (__builtin_bswap16(dest) & 0b0000000000011111);

                  uint8_t r = ((sr * alpha) + (dr * (255 - alpha))) >> 8;
                  uint8_t g = ((sg * alpha) + (dg * (255 - alpha))) >> 8;
                  uint8_t b = ((sb * alpha) + (db * (255 - alpha))) >> 8;

                  // recombine the channels
                  *pdest  = __builtin_bswap16((r << 11) | (g << 5) | (b));
                }

                pdest++;
                palpha++;
            }
        }

        return true;
    }
}