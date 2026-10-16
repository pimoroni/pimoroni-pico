#include "pwm.hpp"
#include "hardware/clocks.h"

namespace pimoroni {
  static const uint32_t MAX_PWM_WRAP = UINT16_MAX;

  // Derived from the rp2 Micropython implementation: https://github.com/micropython/micropython/blob/master/ports/rp2/machine_pwm.c
  bool calculate_pwm_factors(float freq, uint16_t& top_out, uint16_t& div16_out) {
    bool success = false;
    uint32_t source_hz = clock_get_hz(clk_sys);

    // Check the provided frequency is valid
    if((freq >= 1.0f) && (freq <= (float)(source_hz >> 1))) {
      uint64_t div16_top = (uint64_t)((float)((uint64_t)source_hz << 4) / freq);
      // The smallest divider that fits the wrap limit gives the largest top, and the finest duty resolution
      uint64_t div16 = MAX((div16_top + MAX_PWM_WRAP - 1) / MAX_PWM_WRAP, 16u);
      // Round the top to the nearest count, for the closest frequency
      uint64_t top = MAX((div16_top + (div16 >> 1)) / div16, 1u);

      // Only return valid factors if the divisor is actually achievable
      if(div16 <= (UINT8_MAX << 4) && top <= MAX_PWM_WRAP) {
        top_out = (uint16_t)top;
        div16_out = (uint16_t)div16;

        success = true;
      }
    }
    return success;
  }

};