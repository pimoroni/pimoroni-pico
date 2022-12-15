#include "pico_graphics.hpp"

namespace pimoroni {
    PicoGraphics_PenRGB888::PicoGraphics_PenRGB888(uint16_t width, uint16_t height, void *frame_buffer)
    : PicoGraphics(width, height, frame_buffer) {
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
    void PicoGraphics_PenRGB888::set_pixel(const Point &p) {
        uint32_t *buf = (uint32_t *)frame_buffer;
        buf[p.y * bounds.w + p.x] = color;
    }
    void PicoGraphics_PenRGB888::set_pixel_span(const Point &p, uint l) {
        // pointer to byte in framebuffer that contains this pixel
        uint32_t *buf = (uint32_t *)frame_buffer;
        buf = &buf[p.y * bounds.w + p.x];

        while(l--) {
            *buf++ = color;
        }
    }
    PicoGraphics_PenRGB888_HF::PicoGraphics_PenRGB888_HF(uint16_t width, uint16_t height, void *frame_buffer)
    : PicoGraphics(width, height, frame_buffer) {
        this->pen_type = PEN_RGB888;
        if(this->frame_buffer == nullptr) {
            this->frame_buffer = (void *)(new uint8_t[buffer_size(width, height)]);
        }
    }
    void PicoGraphics_PenRGB888_HF::set_pen(uint c) {
        color = c;
    }
    void PicoGraphics_PenRGB888_HF::set_pen(uint8_t r, uint8_t g, uint8_t b) {
        src_color = {r, g, b};
        color = src_color.to_rgb888();
    }
    int PicoGraphics_PenRGB888_HF::create_pen(uint8_t r, uint8_t g, uint8_t b) {
        return RGB(r, g, b).to_rgb888();
    }
    void PicoGraphics_PenRGB888_HF::set_pixel(const Point &p) {
        uint32_t *buf = (uint32_t *)frame_buffer;
        if (p.x > bounds.w/2){
            Point flip((bounds.w - (p.x)) + (bounds.w / 2), bounds.h - p.y);
            buf[flip.y * bounds.w + flip.x] = color;
        }

        else{
            buf[p.y * bounds.w + p.x] = color;
        }
    }
    void PicoGraphics_PenRGB888_HF::set_pixel_span(const Point &p, uint l) {
        // pointer to byte in framebuffer that contains this pixel
        uint32_t *buf = (uint32_t *)frame_buffer;
        if (p.x > bounds.w/2){
            Point flip((bounds.w - (p.x)) + (bounds.w / 2), bounds.h - p.y);
            buf = buf[flip.y * bounds.w + flip.x] = color;
        }
        else{
            buf = &buf[p.y * bounds.w + p.x];
        }
        while(l--) {
            *buf++ = color;
        }
    
    }
}