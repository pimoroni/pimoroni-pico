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

    bool PicoGraphics_PenRGB565::render_pico_vector_tile(const pp_tile_t *tile) {
        // fix pico vector clipping here
        
        for(int y = tile->y; y < tile->y + tile->h; y++) {
            uint8_t *palpha = &tile.data[((y - tile->y) * tile.stride)];            
            uint16_t *pdest = &((uint16_t *)frame_buffer)[tile->x + (tile->y * bounds.w)];
            for(int x = tile->x; x < tile->x + tile->w; x++) {
                uint16_t dest = *pdest;
                uint8_t alpha = *palpha >> 3;

                // blend tha pixel                
                // expand the pen colour to provide space to multiple all 
                // channels at once
                uint32_t cs = 0;
                cs |= ((color & 0b1111100000000000) << 11); // red
                cs |= ((color & 0b0000011111100000) <<  5); // green
                cs |= ((color & 0b0000000000011111)      ); // blue

                uint32_t cd = 0;
                cd |= ((dest  & 0b1111100000000000) << 11); // red
                cd |= ((dest  & 0b0000011111100000) <<  5); // green
                cd |= ((dest  & 0b0000000000011111)      ); // blue

                uint32_t cr = ((cs * alpha) + (cd * (31 - alpha)))>> 5;

                // recombine the channels
                *pdest  = ((cr >> 27) & 0b1111100000000000);
                *pdest |= ((cr >> 11) & 0b0000011111100000);
                *pdest |= ((cr >> 5) & 0b0000000000011111);

                pdest++;
                palpha++;
            }
        }

        return true;
    }    
}