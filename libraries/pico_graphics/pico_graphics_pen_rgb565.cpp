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

    void PicoGraphics_PenRGB565::set_pixel_alpha(const Point &p, const uint8_t a) {
        if(!bounds.contains(p)) return;

        uint16_t *buf = (uint16_t *)frame_buffer;
        buf += this->layer_offset;

        RGB565 blended = RGB(buf[p.y * bounds.w + p.x]).blend(RGB(color), a).to_rgb565();

        buf[p.y * bounds.w + p.x] = blended;
    };

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

    bool PicoGraphics_PenRGB565::render_tile(const Tile *tile) {
        for(int y = 0; y < tile->h; y++) {
            uint8_t *p_alpha = &tile->data[(y * tile->stride)];

            uint16_t *p_dest = &((uint16_t *)frame_buffer)[tile->x + ((tile->y + y) * bounds.w)];
            p_dest += this->layer_offset;

            uint16_t *p_layer0 = &((uint16_t *)frame_buffer)[tile->x + ((tile->y + y) * bounds.w)];

            for(int x = 0; x < tile->w; x++) {
                uint16_t dest = *p_dest;
                if(dest == 0 && this->layers > 1) {
                    dest = *p_layer0;
                }
                uint8_t alpha = *p_alpha;

                if(alpha == 255) {
                  *p_dest = color;
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
                  *p_dest  = __builtin_bswap16((r << 11) | (g << 5) | (b));
                }

                p_layer0++;
                p_dest++;
                p_alpha++;
            }
        }

        return true;
    }
}
