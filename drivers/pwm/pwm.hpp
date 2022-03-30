#pragma once

#include "pico/stdlib.h"

namespace pimoroni {
  bool calculate_pwm_factors(float freq, uint16_t& top_out, uint16_t& div16_out);
}