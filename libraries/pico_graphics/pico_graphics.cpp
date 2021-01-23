#include "pico_graphics.hpp"

extern uint8_t font_data[96][6];
extern uint8_t character_widths[96];

namespace pimoroni {

  void PicoGraphics::set_pen(uint8_t r, uint8_t g, uint8_t b) {
    pen = create_pen(r, g, b);
  }

  void PicoGraphics::set_pen(Pen p) {
    pen = p;
  }

  void PicoGraphics::set_clip(const Rect &r) {
    clip = bounds.intersection(r);
  }

  void PicoGraphics::remove_clip() {
    clip = bounds;
  }

  uint16_t* PicoGraphics::ptr(const Rect &r) {
    return frame_buffer + r.x + r.y * bounds.w;
  }

  uint16_t* PicoGraphics::ptr(const Point &p) {
    return frame_buffer + p.x + p.y * bounds.w;
  }

  uint16_t* PicoGraphics::ptr(int32_t x, int32_t y) {
    return frame_buffer + x + y * bounds.w;
  }

  void PicoGraphics::clear() {
    rectangle(clip);
  }

  void PicoGraphics::pixel(const Point &p) {
    if(!clip.contains(p)) return;
    *ptr(p) = pen;
  }

  void PicoGraphics::pixel_span(const Point &p, int32_t l) {
    // check if span in bounds
    if( p.x + l < clip.x || p.x >= clip.x + clip.w ||
        p.y     < clip.y || p.y >= clip.y + clip.h) return;

    // clamp span horizontally
    Point clipped = p;
    if(clipped.x     <  clip.x)           {l += clipped.x - clip.x; clipped.x = clip.x;}
    if(clipped.x + l >= clip.x + clip.w)  {l  = clip.x + clip.w - clipped.x;}

    uint16_t *dest = ptr(clipped);
    while(l--) {
      *dest++ = pen;
    }
  }

  void PicoGraphics::rectangle(const Rect &r) {
    // clip and/or discard depending on rectangle visibility
    Rect clipped = r.intersection(clip);

    if(clipped.empty()) return;

    uint16_t *dest = ptr(clipped);
    while(clipped.h--) {
      // draw span of pixels for this row
      for(uint32_t i = 0; i < clipped.w; i++) {
        *dest++ = pen;
      }

      // move to next scanline
      dest += bounds.w - clipped.w;
    }
  }

  void PicoGraphics::circle(const Point &p, int32_t radius) {
    // circle in screen bounds?
    Rect bounds = Rect(p.x - radius, p.y - radius, radius * 2, radius * 2);
    if(!bounds.intersects(clip)) return;

    int ox = radius, oy = 0, err = -radius;
    while (ox >= oy)
    {
      int last_oy = oy;

      err += oy; oy++; err += oy;

      pixel_span(Point(p.x - ox, p.y + last_oy), ox * 2 + 1);
      if (last_oy != 0) {
        pixel_span(Point(p.x - ox, p.y - last_oy), ox * 2 + 1);
      }

      if(err >= 0 && ox != last_oy) {
        pixel_span(Point(p.x - last_oy, p.y + ox), last_oy * 2 + 1);
        if (ox != 0) {
          pixel_span(Point(p.x - last_oy, p.y - ox), last_oy * 2 + 1);
        }

        err -= ox; ox--; err -= ox;
      }
    }
  }

  void PicoGraphics::character(const char c, const Point &p, uint8_t scale) {
    uint8_t char_index = c - 32;
    Rect char_bounds(p.x, p.y, character_widths[char_index] * scale, 6 * scale);

    if(!clip.intersects(char_bounds)) return;

    const uint8_t *d = &font_data[char_index][0];
    for(uint8_t cx = 0; cx < character_widths[char_index]; cx++) {
      for(uint8_t cy = 0; cy < 6; cy++) {
        if((1U << cy) & *d) {
          rectangle(Rect(p.x + (cx * scale), p.y + (cy * scale), scale, scale));
        }
      }

      d++;
    }
  }

  void PicoGraphics::text(const std::string &t, const Point &p, int32_t wrap, uint8_t scale) {
    uint32_t co = 0, lo = 0; // character and line (if wrapping) offset

    size_t i = 0;
    while(i < t.length()) {
      // find length of current word
      size_t next_space = t.find(' ', i + 1);

      if(next_space == std::string::npos) {
        next_space = t.length();
      }

      uint16_t word_width = 0;
      for(size_t j = i; j < next_space; j++) {
        word_width += character_widths[t[j] - 32] * scale;
      }

      // if this word would exceed the wrap limit then
      // move to the next line
      if(co != 0 && co + word_width > wrap) {
        co = 0;
        lo += 7 * scale;
      }

      // draw word
      for(size_t j = i; j < next_space; j++) {
        character(t[j], Point(p.x + co, p.y + lo), scale);
        co += character_widths[t[j] - 32] * scale;
      }

      // move character offset to end of word and add a space
      co += character_widths[0] * scale;
      i = next_space + 1;
    }
  }

}