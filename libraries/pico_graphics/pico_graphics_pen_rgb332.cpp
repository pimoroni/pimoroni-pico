#include "pico_graphics.hpp"
#include <string.h>

namespace pimoroni {
    PicoGraphics_PenRGB332::PicoGraphics_PenRGB332(uint16_t width, uint16_t height, void *frame_buffer, uint16_t layers)
    : PicoGraphics(width, height, layers, frame_buffer) {
        this->pen_type = PEN_RGB332;
        if(this->frame_buffer == nullptr) {
            this->frame_buffer = (void *)(new uint8_t[buffer_size(width, height) * layers]);
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
    int PicoGraphics_PenRGB332::create_pen_hsv(float h, float s, float v) {
        return RGB::from_hsv(h, s, v).to_rgb332();
    }
    void PicoGraphics_PenRGB332::set_pixel(const Point &p) {
        uint8_t *buf = (uint8_t *)frame_buffer;
        buf += this->layer_offset;
        buf[p.y * bounds.w + p.x] = color;
    }
    void PicoGraphics_PenRGB332::set_pixel_span(const Point &p, uint l) {
        // pointer to byte in framebuffer that contains this pixel
        uint8_t *buf = (uint8_t *)frame_buffer;
        buf += this->layer_offset;
        buf += p.y * bounds.w + p.x;

        while(l--) {
            *buf++ = color;
        }
    }
    void PicoGraphics_PenRGB332::set_pixel_alpha(const Point &p, const uint8_t a) {
        if(!bounds.contains(p)) return;

        uint8_t *buf = (uint8_t *)frame_buffer;
        buf += this->layer_offset;

        RGB332 blended = RGB(buf[p.y * bounds.w + p.x]).blend(RGB(color), a).to_rgb332();

        buf[p.y * bounds.w + p.x] = blended;
    };
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
    void PicoGraphics_PenRGB332::frame_convert(PenType type, conversion_callback_func callback) {
        if(type == PEN_RGB565) {

            // Treat our void* frame_buffer as uint8_t
            uint8_t *src = (uint8_t *)frame_buffer;

            if(this->layers > 1) {
                // The size of a single layer
                uint offset = this->bounds.w * this->bounds.h;

                frame_convert_rgb565(callback, [&]() {
                    uint8_t c = 0;

                    // Iterate through layers in reverse order
                    // Return the first nonzero (not transparent) pixel
                    for(auto layer = this->layers; layer > 0; layer--) {
                        c = *(src + offset * (layer - 1));
                        if (c) break;
                    }

                    src++;

                    return rgb332_to_rgb565_lut[c];
                });
            } else {
                frame_convert_rgb565(callback, [&]() {
                    return rgb332_to_rgb565_lut[*src++];
                });
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
    bool PicoGraphics_PenRGB332::render_tile(const Tile *tile) {
        for(int y = 0; y < tile->h; y++) {
            uint8_t *palpha = &tile->data[(y * tile->stride)];

            uint8_t *p_dest = &((uint8_t *)frame_buffer)[tile->x + ((tile->y + y) * bounds.w)];
            p_dest += this->layer_offset;

            uint8_t *p_layer0 = &((uint8_t *)frame_buffer)[tile->x + ((tile->y + y) * bounds.w)];


            for(int x = 0; x < tile->w; x++) {
                uint8_t alpha = *palpha;
                uint8_t dest = *p_dest;
                if(dest == 0) {
                    dest = *p_layer0;
                }

                // TODO: Try to alpha blend RGB332... somewhat?
                if(alpha == 255) {
                  *p_dest = color;
                }else if(alpha == 0) {
                }else{
                  // blend tha pixel
                  uint16_t sr = (color & 0b11100000) >> 5;
                  uint16_t sg = (color & 0b00011100) >> 2;
                  uint16_t sb = (color & 0b00000011);

                  uint16_t dr = (dest & 0b11100000) >> 5;
                  uint16_t dg = (dest & 0b00011100) >> 2;
                  uint16_t db = (dest & 0b00000011);

                  uint8_t r = ((sr * alpha) + (dr * (255 - alpha))) >> 8;
                  uint8_t g = ((sg * alpha) + (dg * (255 - alpha))) >> 8;
                  uint8_t b = ((sb * alpha) + (db * (255 - alpha))) >> 8;

                  // recombine the channels
                  *p_dest  = (r << 5) | (g << 2) | (b);
                }

                p_dest++;
                palpha++;
            }
        }

        return true;
    }
}
