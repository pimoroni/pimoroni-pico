#include "pico_graphics.hpp"

namespace pimoroni {
    PicoGraphics_PenP8::PicoGraphics_PenP8(uint16_t width, uint16_t height, void *frame_buffer, uint16_t layers)
    : PicoGraphics(width, height, layers, frame_buffer) {
        this->pen_type = PEN_P8;
        if(this->frame_buffer == nullptr) {
            this->frame_buffer = (void *)(new uint8_t[buffer_size(width, height)]);
        }
        for(auto i = 0u; i < palette_size; i++) {
            palette[i] = {uint8_t(i), uint8_t(i), uint8_t(i)};
            used[i] = false;
        }
        cache_built = false;
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
        used[i] = true;
        palette[i] = {r, g, b};
        cache_built = false;
        return i;
    }
    int PicoGraphics_PenP8::create_pen(uint8_t r, uint8_t g, uint8_t b) {
        // Create a colour and place it in the palette if there's space
        for(auto i = 0u; i < palette_size; i++) {
            if(!used[i]) {
                palette[i] = {r, g, b};
                used[i] = true;
                cache_built = false;
                return i;
            }
        }
        return -1;
    }
    int PicoGraphics_PenP8::create_pen_hsv(float h, float s, float v) {
        RGB p = RGB::from_hsv(h, s, v);
        return create_pen(p.r, p.g, p.b);
    }
    int PicoGraphics_PenP8::reset_pen(uint8_t i) {
        palette[i] = {0, 0, 0};
        used[i] = false;
        cache_built = false;
        return i;
    }
    void PicoGraphics_PenP8::set_pixel(const Point &p) {
        uint8_t *buf = (uint8_t *)frame_buffer;
        buf += this->layer_offset;
        buf[p.y * bounds.w + p.x] = color;
    }
    
    void PicoGraphics_PenP8::set_pixel_span(const Point &p, uint l) {
        // pointer to byte in framebuffer that contains this pixel
        uint8_t *buf = (uint8_t *)frame_buffer;
        buf += this->layer_offset;
        buf = &buf[p.y * bounds.w + p.x];

        while(l--) {
            *buf++ = color;
        }
    }

    void PicoGraphics_PenP8::get_dither_candidates(const RGB &col, const RGB *palette, size_t len, std::array<uint8_t, 16> &candidates) {
        RGB error;
        for(size_t i = 0; i < candidates.size(); i++) {
            candidates[i] = (col + error).closest(palette, len);
            error += (col - palette[candidates[i]]);
        }

        // sort by a rough approximation of luminance, this ensures that neighbouring
        // pixels in the dither matrix are at extreme opposites of luminence
        // giving a more balanced output
        std::sort(candidates.begin(), candidates.end(), [palette](int a, int b) {
            return palette[a].luminance() > palette[b].luminance();
        });
    }

    void PicoGraphics_PenP8::set_pixel_dither(const Point &p, const RGB &c) {
        if(!bounds.contains(p)) return;

        if(!cache_built) {
            for(uint i = 0; i < 512; i++) {
                RGB cache_col((i & 0x1C0) >> 1, (i & 0x38) << 2, (i & 0x7) << 5);
                get_dither_candidates(cache_col, palette, palette_size, candidate_cache[i]);
            }
            cache_built = true;
        }

        uint cache_key = ((c.r & 0xE0) << 1) | ((c.g & 0xE0) >> 2) | ((c.b & 0xE0) >> 5);
        //get_dither_candidates(c, palette, 256, candidates);

        // find the pattern coordinate offset
        uint pattern_index = (p.x & 0b11) | ((p.y & 0b11) << 2);

        // set the pixel
        //color = candidates[pattern[pattern_index]];
        color = candidate_cache[cache_key][dither16_pattern[pattern_index]];
        set_pixel(p);
    }

    void PicoGraphics_PenP8::frame_convert(PenType type, conversion_callback_func callback) {
        // Treat our void* frame_buffer as uint8_t
        uint8_t *src = (uint8_t *)frame_buffer;

        if(layers > 1) {
            // The size of a single layer
            uint offset = this->bounds.w * this->bounds.h;

            if(type == PEN_RGB565) {
                // Cache the RGB888 palette as RGB565
                RGB565 cache[palette_size];
                for(auto i = 0u; i < palette_size; i++) {
                    cache[i] = palette[i].to_rgb565();
                }

                frame_convert_rgb565(callback, [&]() {
                    // Check the *palette* index, rather than the colour
                    // Thus palette entry 0 is *always* transparent
                    uint8_t c = 0;

                    // Iterate through layers in reverse order
                    // Return the first nonzero (not transparent) pixel
                    for(auto layer = this->layers; layer > 0; layer--) {
                        c = *(src + offset * (layer - 1));
                        if (c) break;
                    }

                    src++;

                    return cache[c];
                });
            } else if (type == PEN_RGB888) {
                frame_convert_rgb888(callback, [&]() {
                    // Check the *palette* index, rather than the colour
                    // Thus palette entry 0 is *always* transparent
                    uint8_t c = 0;

                    // Iterate through layers in reverse order
                    // Return the first nonzero (not transparent) pixel
                    for(auto layer = this->layers; layer > 0; layer--) {
                        c = *(src + offset * (layer - 1));
                        if (c) break;
                    }

                    src++;

                    return palette[c].to_rgb888();
                });
            }
        } else {
            if(type == PEN_RGB565) {
                // Cache the RGB888 palette as RGB565
                RGB565 cache[palette_size];
                for(auto i = 0u; i < palette_size; i++) {
                    cache[i] = palette[i].to_rgb565();
                }

                frame_convert_rgb565(callback, [&]() {
                    return cache[*src++];
                });
            } else if (type == PEN_RGB888) {
                frame_convert_rgb888(callback, [&]() {
                    return palette[*src++].to_rgb888();
                });
            }
        }
    }

    bool PicoGraphics_PenP8::render_tile(const Tile *tile) {
        for(int y = 0; y < tile->h; y++) {
            uint8_t *palpha = &tile->data[(y * tile->stride)];
            uint8_t *pdest = &((uint8_t *)frame_buffer)[tile->x + ((tile->y + y) * bounds.w)];
            for(int x = 0; x < tile->w; x++) {
                uint8_t alpha = *palpha;

                if(alpha == 0) {
                } else {
                  *pdest = color;
                }

                pdest++;
                palpha++;
            }
        }

        return true;
    }
}
