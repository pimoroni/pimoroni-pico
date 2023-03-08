#include "pico_graphics.hpp"

namespace pimoroni {

    PicoGraphics_Pen3Bit::PicoGraphics_Pen3Bit(uint16_t width, uint16_t height, void *frame_buffer)
    : PicoGraphics(width, height, frame_buffer) {
        this->pen_type = PEN_3BIT;
        if(this->frame_buffer == nullptr) {
            this->frame_buffer = (void *)(new uint8_t[buffer_size(width, height)]);
        }
        cache_built = false;
    }
    void PicoGraphics_Pen3Bit::_set_pixel(const Point &p, uint col) {
        uint offset = (bounds.w * bounds.h) / 8;
        uint8_t *buf = (uint8_t *)frame_buffer;

        uint bo = 7 - (p.x & 0b111);

        uint8_t *bufA = &buf[(p.x / 8) + (p.y * bounds.w / 8)];
        uint8_t *bufB = bufA + offset;
        uint8_t *bufC = bufA + offset + offset;

        uint8_t cA = (col & 0b100) >> 2;
        *bufA &= ~(1U << bo);
        *bufA |= (cA << bo);

        uint8_t cB = (col & 0b010) >> 1;
        *bufB &= ~(1U << bo);
        *bufB |= (cB << bo);

        uint8_t cC = (col & 0b001);
        *bufC &= ~(1U << bo);
        *bufC |= (cC << bo);
    }
    void PicoGraphics_Pen3Bit::set_pen(uint c) {
        color = c;
    }
    void PicoGraphics_Pen3Bit::set_pen(uint8_t r, uint8_t g, uint8_t b) {
        color = RGB(r, g, b).to_rgb888() | 0x7f000000;
    }
    int PicoGraphics_Pen3Bit::create_pen(uint8_t r, uint8_t g, uint8_t b) {
        return RGB(r, g, b).to_rgb888() | 0x7f000000;
    }
    int PicoGraphics_Pen3Bit::create_pen_hsv(float h, float s, float v) {
        return RGB::from_hsv(h, s, v).to_rgb888() | 0x7f000000;
    }
    void PicoGraphics_Pen3Bit::set_pixel(const Point &p) {
        if ((color & 0x7f000000) == 0x7f000000) {
            set_pixel_dither(p, RGB(color));
        } else {
            _set_pixel(p, color);
        }
    }
    void PicoGraphics_Pen3Bit::set_pixel_span(const Point &p, uint l) {
        Point lp = p;
        while(l--) {
            if ((color & 0x7f000000) == 0x7f000000) {
                set_pixel_dither(lp, RGB(color));
            } else {
                _set_pixel(lp, color);
            }
            lp.x++;
        }
    }
    void PicoGraphics_Pen3Bit::get_dither_candidates(const RGB &col, const RGB *palette, size_t len, std::array<uint8_t, 16> &candidates) {
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

    void PicoGraphics_Pen3Bit::set_pixel_dither(const Point &p, const RGB &c) {
        if(!bounds.contains(p)) return;

        if(!cache_built) {
            for(uint i = 0; i < 512; i++) {
                uint r = (i & 0x1c0) >> 1;
                uint g = (i & 0x38) << 2;
                uint b = (i & 0x7) << 5;
                RGB cache_col(
                    r | (r >> 3) | (r >> 6),
                    g | (g >> 3) | (g >> 6),
                    b | (b >> 3) | (b >> 6)
                );
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
        _set_pixel(p, candidate_cache[cache_key][dither16_pattern[pattern_index]]);
    }
    void PicoGraphics_Pen3Bit::frame_convert(PenType type, conversion_callback_func callback) {
        if(type == PEN_P4) {
            uint8_t row_buf[bounds.w / 2];
            uint offset = (bounds.w * bounds.h) / 8;
            uint8_t *buf = (uint8_t *)frame_buffer;

            for(auto y = 0; y < bounds.h; y++) {
                for(auto x = 0; x < bounds.w; x++) {
                    uint bo = 7 - (x & 0b111);

                    uint8_t *bufA = &buf[(x / 8) + (y * bounds.w / 8)];
                    uint8_t *bufB = bufA + offset;
                    uint8_t *bufC = bufA + offset + offset;

                    uint8_t nibble = (*bufA >> bo) & 1U;
                    nibble <<= 1;
                    nibble |= (*bufB >> bo) & 1U;
                    nibble <<= 1;
                    nibble |= (*bufC >> bo) & 1U;
                    nibble <<= (x & 0b1) ? 0 : 4;

                    row_buf[x / 2] &= (x & 0b1) ? 0b11110000 : 0b00001111;
                    row_buf[x / 2] |= nibble;
                }
                callback(row_buf, bounds.w / 2);
            }
        }
    }
}