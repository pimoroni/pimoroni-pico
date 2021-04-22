#pragma once

#include <cstdint>

namespace pimoroni {
  struct Font {
    const uint8_t height;
    const uint8_t max_width;
    const uint8_t widths[96];
    const uint8_t data[];
  };
}