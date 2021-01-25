#include "pico_graphics.hpp"

extern uint8_t font_data[];
//extern uint8_t character_widths[97];

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

  Pen* PicoGraphics::ptr(const Rect &r) {
    return frame_buffer + r.x + r.y * bounds.w;
  }

  Pen* PicoGraphics::ptr(const Point &p) {
    return frame_buffer + p.x + p.y * bounds.w;
  }

  Pen* PicoGraphics::ptr(int32_t x, int32_t y) {
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

    Pen *dest = ptr(clipped);
    while(l--) {
      *dest++ = pen;
    }
  }

  void PicoGraphics::rectangle(const Rect &r) {
    // clip and/or discard depending on rectangle visibility
    Rect clipped = r.intersection(clip);

    if(clipped.empty()) return;

    Pen *dest = ptr(clipped);
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

  enum flags {BOLD = 1, ITALIC = 2, UNDERLINE = 4, STRIKETHROUGH = 8, BULLET = 16};

  uint8_t *character_data(uint8_t i) {
    uint32_t bytes_per_character = (font_data[0] * font_data[1]) + 1;
    return &font_data[(bytes_per_character * (i - 32)) + 2];
  }

  uint8_t character_width(uint8_t i) {
    return character_data(i)[0] + 2;
  }

  uint8_t character_height() {
    return font_data[1] - 2;
  }

  void PicoGraphics::character(const char c, const Point &p, uint32_t flags) {
    uint32_t character_height = font_data[1];
    uint32_t bytes_per_row = font_data[0];

    // check the character is visible at all before continuing
    Rect char_bounds(p.x, p.y, character_width(c), character_height);
    if(!clip.intersects(char_bounds)) return;

    for(int8_t cy = 0; cy < character_height; cy++) {
      uint8_t *data = character_data(c) + (cy * bytes_per_row) + 1;
      uint8_t  bit = 0;

      for(int8_t cx = 0; cx < character_width(c); cx++) {
        // move on to next byte of character data
        if(cx != 0 && cx % 8 == 0) {
          data++;
          bit = 0;
        }

        if((1U << (7 - bit)) & *data) {
          int8_t italic = 0;
          if(flags & flags::ITALIC) {
            italic = ((character_height - cy) >> 1) - 1;
          }
          pixel(Point(p.x + cx + italic, p.y + cy));
          if(flags & flags::BOLD) {
            pixel(Point(p.x + cx + 1, p.y + cy));
          }
        }

        bit++;
      }
    }

    if(flags & flags::UNDERLINE || flags & flags::STRIKETHROUGH) {
      for(int8_t cx = -1; cx < character_width(c) + 1; cx++) {
        if(flags & flags::UNDERLINE) {
          pixel(Point(p.x + cx, p.y + character_height - 1));
        }

        if(flags & flags::STRIKETHROUGH) {
          pixel(Point(p.x + cx, p.y + character_height / 2));
        }
      }
    }
  }

  void PicoGraphics::text(const std::string &text, const Point &p, int32_t wrap) {
    uint32_t flags = 0;      // style flags
    uint32_t co = 0, lo = 0; // character and line (if wrapping) offset

    for (std::string::size_type i = 0; i < text.size(); i++) {
      char c = text[i];
      // handle special characters
      switch(c) {
        case '\n': { // line break
          co = 0;
          lo += character_height();
          flags &= ~flags::BULLET;
          continue;
        } break;
        case '-': { // bulleted list
          if(co == 0) { // only if first character on line
            co += character_height(); // line height as indent for bullet
            character(249, Point(p.x + co, p.y + lo), flags); // kinda bullet point thingy
            co += character_width(249) + character_width(32); // bullet + space
            flags |= flags::BULLET;
            continue;
          }
        } break;
        case '*': { // bold
          flags ^= flags::BOLD;
          continue;
        } break;
        case '/': { // italic
          flags ^= flags::ITALIC;
          continue;
        } break;
        case '_': { // underline
          flags ^= flags::UNDERLINE;
          continue;
        } break;
        case '~': { // strikethrough
          flags ^= flags::STRIKETHROUGH;
          continue;
        } break;
        case '\\': { // escaped character
          // skip forward to escaped character so that it is
          // rendered in this iteration of the loop and
          // isn't evaluated for special behaviour
          if(i < text.size() - 1) {
            i++;
            c = text[i + 1];
          }
        } break;
        case ' ': { // space
          // determine if we need to wordwrap
          size_t next_whitespace = std::min(text.find(' ', i + 1), text.find('\n', i + 1));
          if(next_whitespace == std::string::npos) { // not found, use end of text
            next_whitespace = text.size();
          }

          // calculate width of the word including this leading space
          uint16_t word_width = character_width(' ');
          for(std::string::size_type j = i + 1; j < next_whitespace; j++) {
            if(text[j] == '\\') {
              if(j < next_whitespace - 1) {
                j++;
                // skip forward to escaped character so that it is
                // rendered in this iteration of the loop and
                // isn't evaluated for special behaviour
                word_width += character_width(text[j]);
                continue;
              }
            }

            if(text[j] == '*' || text[j] == '~') {
              continue;
            }

            word_width += character_width(c);
          }

          // if word too long for this line then wrap
          if(co + word_width >= wrap) {
            if(flags & flags::BULLET) {
              co = character_height() + character_width(249) + character_width(32); // bullet + space
            }else{
              co = 0;
            }

            lo += character_height();
            continue;
          }
        } break;
      }

      character(text[i], Point(p.x + co, p.y + lo), flags);
      co += character_width(c);
    }
  }

  int32_t orient2d(Point p1, Point p2, Point p3) {
    return (p2.x - p1.x) * (p3.y - p1.y) - (p2.y - p1.y) * (p3.x - p1.x);
  }

  bool is_top_left(const Point &p1, const Point &p2) {
    return (p1.y == p2.y && p1.x > p2.x) || (p1.y < p2.y);
  }

  void PicoGraphics::triangle(Point p1, Point p2, Point p3) {
    Rect triangle_bounds(
      Point(std::min(p1.x, std::min(p2.x, p3.x)), std::min(p1.y, std::min(p2.y, p3.y))),
      Point(std::max(p1.x, std::max(p2.x, p3.x)), std::max(p1.y, std::max(p2.y, p3.y))));

    // clip extremes to frame buffer size
    triangle_bounds = clip.intersection(triangle_bounds);

    // if triangle completely out of bounds then don't bother!
    if (triangle_bounds.empty()) {
      return;
    }

    // fix "winding" of vertices if needed
    int32_t winding = orient2d(p1, p2, p3);
    if (winding < 0) {
      Point t;
      t = p1; p1 = p3; p3 = t;
    }

    // bias ensures no overdraw between neighbouring triangles
    int8_t bias0 = is_top_left(p2, p3) ? 0 : -1;
    int8_t bias1 = is_top_left(p3, p1) ? 0 : -1;
    int8_t bias2 = is_top_left(p1, p2) ? 0 : -1;

    int32_t a01 = p1.y - p2.y;
    int32_t b01 = p2.x - p1.x;
    int32_t a12 = p2.y - p3.y;
    int32_t b12 = p3.x - p2.x;
    int32_t a20 = p3.y - p1.y;
    int32_t b20 = p1.x - p3.x;

    Point tl(triangle_bounds.x, triangle_bounds.y);
    int32_t w0row = orient2d(p2, p3, tl) + bias0;
    int32_t w1row = orient2d(p3, p1, tl) + bias1;
    int32_t w2row = orient2d(p1, p2, tl) + bias2;

    for (uint32_t y = 0; y < triangle_bounds.h; y++) {
      int32_t w0 = w0row;
      int32_t w1 = w1row;
      int32_t w2 = w2row;

      Pen *dest = ptr(triangle_bounds.x, triangle_bounds.y + y);
      for (uint32_t x = 0; x < triangle_bounds.w; x++) {
        if ((w0 | w1 | w2) >= 0) {
          *dest = pen;
        }

        dest++;

        w0 += a12;
        w1 += a20;
        w2 += a01;
      }

      w0row += b12;
      w1row += b20;
      w2row += b01;
    }
  }

  void PicoGraphics::polygon(const std::vector<Point> &points) {
    static int32_t nodes[64]; // maximum allowed number of nodes per scanline for polygon rendering

    int32_t miny = points[0].y, maxy = points[0].y;

    for (uint16_t i = 1; i < points.size(); i++) {
      miny = std::min(miny, points[i].y);
      maxy = std::max(maxy, points[i].y);
    }

    // for each scanline within the polygon bounds (clipped to clip rect)
    Point p;

    for (p.y = std::max(clip.y, miny); p.y <= std::min(clip.y + clip.h, maxy); p.y++) {
      uint8_t n = 0;
      for (uint16_t i = 0; i < points.size(); i++) {
        uint16_t j = (i + 1) % points.size();
        int32_t sy = points[i].y;
        int32_t ey = points[j].y;
        int32_t fy = p.y;
        if ((sy < fy && ey >= fy) || (ey < fy && sy >= fy)) {
          int32_t sx = points[i].x;
          int32_t ex = points[j].x;
          int32_t px = int32_t(sx + float(fy - sy) / float(ey - sy) * float(ex - sx));

          nodes[n++] = px < clip.x ? clip.x : (px >= clip.x + clip.w ? clip.x + clip.w - 1 : px);// clamp(int32_t(sx + float(fy - sy) / float(ey - sy) * float(ex - sx)), clip.x, clip.x + clip.w);
        }
      }

      uint16_t i = 0;
      while (i < n - 1) {
        if (nodes[i] > nodes[i + 1]) {
          int32_t s = nodes[i]; nodes[i] = nodes[i + 1]; nodes[i + 1] = s;
          if (i) i--;
        }
        else {
          i++;
        }
      }

      for (uint16_t i = 0; i < n; i += 2) {
        pixel_span(Point(nodes[i], p.y), nodes[i + 1] - nodes[i] + 1);
      }
    }
  }

  void PicoGraphics::line(Point p1, Point p2) {
    // fast horizontal line
    if(p1.y == p2.y) {
      int32_t start = std::max(clip.x, std::min(p1.x, p2.x));
      int32_t end   = std::min(clip.x + clip.w, std::max(p1.x, p2.x));
      pixel_span(Point(start, p1.y), end - start);
      return;
    }

    // fast vertical line
    if(p1.x == p2.x) {
      int32_t start  = std::max(clip.y, std::min(p1.y, p2.y));
      int32_t length = std::min(clip.y + clip.h, std::max(p1.y, p2.y)) - start;
      Pen *dest = ptr(p1.x, start);
      while(length--) {
        *dest = pen;
        dest += bounds.w;
      }
      return;
    }

    // general purpose line
    // lines are either "shallow" or "steep" based on whether the x delta
    // is greater than the y delta
    int32_t dx = p2.x - p1.x;
    int32_t dy = p2.y - p1.y;
    bool shallow = std::abs(dx) > std::abs(dy);
    if(shallow) {
      // shallow version
      int32_t s = std::abs(dx);       // number of steps
      int32_t sx = dx < 0 ? -1 : 1;   // x step value
      int32_t sy = (dy << 16) / s;    // y step value in fixed 16:16
      int32_t x = p1.x;
      int32_t y = p1.y << 16;
      while(s--) {
        pixel(Point(x, y >> 16));
        y += sy;
        x += sx;
      }
    }else{
      // steep version
      int32_t s = std::abs(dy);       // number of steps
      int32_t sy = dy < 0 ? -1 : 1;   // x step value
      int32_t sx = (dx << 16) / s;    // y step value in fixed 16:16
      int32_t y = p1.y;
      int32_t x = p1.x << 16;
      while(s--) {
        pixel(Point(x >> 16, y));
        y += sy;
        x += sx;
      }
    }
  }
}