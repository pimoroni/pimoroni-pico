#include <cstdint>
#include <algorithm>

#include "pico_graphics.hpp"

namespace pimoroni {

  Point Point::clamp(const Rect &r) const {
    return Point(
      std::min(std::max(x, r.x), r.x + r.w),
      std::min(std::max(y, r.y), r.y + r.h)
    );
  }

  bool Rect::empty() const {
    return w <= 0 || h <= 0;
  }

  bool Rect::contains(const Point &p) const {
    return p.x >= x && p.y >= y && p.x < x + w && p.y < y + h;
  }

  bool Rect::contains(const Rect &p) const {
    return p.x >= x && p.y >= y && p.x + p.w < x + w && p.y + p.h < y + h;
  }

  bool Rect::intersects(const Rect &r) const {
    return !(x > r.x + r.w || x + w < r.x || y > r.y + r.h || y + h < r.y);
  }

  Rect Rect::intersection(const Rect &r) const {
    return Rect(std::max(x, r.x),
                std::max(y, r.y),
                std::min(x + w, r.x + r.w) - std::max(x, r.x),
                std::min(y + h, r.y + r.h) - std::max(y, r.y));
  }

  void Rect::inflate(int32_t v) {
    x -= v; y -= v; w += v * 2; h += v * 2;
  }

  void Rect::deflate(int32_t v) {
    x += v; y += v; w -= v * 2; h -= v * 2;
  }

}