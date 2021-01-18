#include <cstdint>
#include <algorithm>

#include "pico_graphics.hpp"

namespace pimoroni {

  point point::clamp(const rect &r) const {
    return point(
      std::min(std::max(x, r.x), r.x + r.w),
      std::min(std::max(y, r.y), r.y + r.h)
    );
  }

  point operator- (point lhs, const point &rhs) {
    lhs -= rhs;
    return lhs;
  }

  point operator- (const point &rhs) {
    return point(-rhs.x, -rhs.y);
  }

  point operator+ (point lhs, const point &rhs) {
    lhs += rhs;
    return lhs;
  }

  bool rect::empty() const {
    return w <= 0 || h <= 0;
  }

  bool rect::contains(const point &p) const {
    return p.x >= x && p.y >= y && p.x < x + w && p.y < y + h;
  }

  bool rect::contains(const rect &p) const {
    return p.x >= x && p.y >= y && p.x + p.w < x + w && p.y + p.h < y + h;
  }

  bool rect::intersects(const rect &r) const {
    return !(x > r.x + r.w || x + w < r.x || y > r.y + r.h || y + h < r.y);
  }

  rect rect::intersection(const rect &r) const {
    return rect(std::max(x, r.x),
                std::max(y, r.y),
                std::min(x + w, r.x + r.w) - std::max(x, r.x),
                std::min(y + h, r.y + r.h) - std::max(y, r.y));
  }

  void rect::inflate(int32_t v) {
    x -= v; y -= v; w += v * 2; h += v * 2;
  }

  void rect::deflate(int32_t v) {
    x += v; y += v; w -= v * 2; h -= v * 2;
  }

}