#include "pico_graphics.hpp"

namespace pimoroni {
    PicoGraphics_PenRGB888::PicoGraphics_PenRGB888(uint16_t width, uint16_t height, void *frame_buffer, uint16_t layers)
    : PicoGraphics(width, height, layers, frame_buffer) {
        this->pen_type = PEN_RGB888;
        if(this->frame_buffer == nullptr) {
            this->frame_buffer = (void *)(new uint8_t[buffer_size(width, height)]);
        }
    }
    void PicoGraphics_PenRGB888::set_pen(uint c) {
        color = c;
    }
    void PicoGraphics_PenRGB888::set_pen(uint8_t r, uint8_t g, uint8_t b) {
        src_color = {r, g, b};
        color = src_color.to_rgb888();
    }
    int PicoGraphics_PenRGB888::create_pen(uint8_t r, uint8_t g, uint8_t b) {
        return RGB(r, g, b).to_rgb888();
    }
    int PicoGraphics_PenRGB888::create_pen_hsv(float h, float s, float v) {
        return RGB::from_hsv(h, s, v).to_rgb888();
    }
    void PicoGraphics_PenRGB888::set_pixel(const Point &p) {
        uint32_t *buf = (uint32_t *)frame_buffer;
        buf += this->layer_offset;
        buf[p.y * bounds.w + p.x] = color;
    }
    void PicoGraphics_PenRGB888::set_pixel_span(const Point &p, uint l) {
        // pointer to byte in framebuffer that contains this pixel
        uint32_t *buf = (uint32_t *)frame_buffer;
        buf += this->layer_offset;
        buf = &buf[p.y * bounds.w + p.x];

        while(l--) {
            *buf++ = color;
        }
    }
    bool PicoGraphics_PenRGB888::render_tile(const Tile *tile) {
        // Unpack our pen colour
        uint32_t sr = (color >> 16) & 0xff;
        uint32_t sg = (color >> 8) & 0xff;
        uint32_t sb = (color >> 0) & 0xff;

        for(int y = 0; y < tile->h; y++) {
            uint8_t *p_alpha = &tile->data[(y * tile->stride)];
            uint32_t *p_dest = &((uint32_t *)frame_buffer)[tile->x + ((tile->y + y) * bounds.w)];
            for(int x = 0; x < tile->w; x++) {
                uint32_t dest = *p_dest;
                uint8_t alpha = *p_alpha;

                // TODO: Alpha blending
                if(alpha == 255) {
                  *p_dest = color;
                }else if(alpha == 0) {
                } else {
                    // blend the pixel
                    uint32_t dr = (dest >> 16) & 0xff;
                    uint32_t dg = (dest >> 8) & 0xff;
                    uint32_t db = (dest >> 0) & 0xff;

                    uint8_t r = ((sr * alpha) + (dr * (255 - alpha))) >> 8;
                    uint8_t g = ((sg * alpha) + (dg * (255 - alpha))) >> 8;
                    uint8_t b = ((sb * alpha) + (db * (255 - alpha))) >> 8;

                    *p_dest = (r << 16) | (g << 8) | b;
                }

                p_dest++;
                p_alpha++;
            }
        }

        return true;
    }
}