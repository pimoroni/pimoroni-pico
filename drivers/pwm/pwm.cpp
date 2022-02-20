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
      uint32_t div16_top = (uint32_t)((float)(source_hz << 4) / freq);
      uint32_t top = 1;

      while(true) {
          // Try a few small prime factors to get close to the desired frequency.
          if((div16_top >= (5 << 4)) && (div16_top % 5 == 0) && (top * 5 <= MAX_PWM_WRAP)) {
              div16_top /= 5;
              top *= 5;
          }
          else if((div16_top >= (3 << 4)) && (div16_top % 3 == 0) && (top * 3 <= MAX_PWM_WRAP)) {
              div16_top /= 3;
              top *= 3;
          }
          else if((div16_top >= (2 << 4)) && (top * 2 <= MAX_PWM_WRAP)) {
              div16_top /= 2;
              top *= 2;
          }
          else {
              break;
          }
      }

      // Only return valid factors if the divisor is actually achievable
      if(div16_top >= 16 && div16_top <= (UINT8_MAX << 4)) {
        top_out = top;
        div16_out = div16_top;

        success = true;
      }
    }
    return success;
  }

};