#pragma once

#include "hardware/pio.h"

namespace pimoroni {

  // Whether any claimed state machine on a PIO, other than own_sm, drives one of the GPIOs in
  // pin_mask. Judged by the out, set and side-set pins each one's PINCTRL names, which stay
  // accurate after a driver releases its pins, unlike the GPIO function.
  inline bool pio_pins_driven_by_others(PIO pio, uint own_sm, uint64_t pin_mask) {
    const uint gpio_base = pio_get_gpio_base(pio);
    for(uint sm = 0; sm < NUM_PIO_STATE_MACHINES; sm++) {
      if(sm == own_sm || !pio_sm_is_claimed(pio, sm)) {
        continue;
      }
      const uint32_t pinctrl = pio->sm[sm].pinctrl;
      uint sideset_count = (pinctrl & PIO_SM0_PINCTRL_SIDESET_COUNT_BITS) >> PIO_SM0_PINCTRL_SIDESET_COUNT_LSB;

      // With an optional side-set, the count includes the enable bit, which drives no pin
      if((pio->sm[sm].execctrl & PIO_SM0_EXECCTRL_SIDE_EN_BITS) && sideset_count > 0) {
        sideset_count--;
      }

      const uint ranges[3][2] = {
        {(pinctrl & PIO_SM0_PINCTRL_OUT_BASE_BITS) >> PIO_SM0_PINCTRL_OUT_BASE_LSB,
         (pinctrl & PIO_SM0_PINCTRL_OUT_COUNT_BITS) >> PIO_SM0_PINCTRL_OUT_COUNT_LSB},
        {(pinctrl & PIO_SM0_PINCTRL_SET_BASE_BITS) >> PIO_SM0_PINCTRL_SET_BASE_LSB,
         (pinctrl & PIO_SM0_PINCTRL_SET_COUNT_BITS) >> PIO_SM0_PINCTRL_SET_COUNT_LSB},
        {(pinctrl & PIO_SM0_PINCTRL_SIDESET_BASE_BITS) >> PIO_SM0_PINCTRL_SIDESET_BASE_LSB,
         sideset_count},
      };
      for(const auto &range : ranges) {
        for(uint i = 0; i < range[1]; i++) {
          // A range runs on past the window's last pin and wraps to its first
          const uint gpio = gpio_base + ((range[0] + i) & 31);
          if(pin_mask & (1llu << gpio)) {
            return true;
          }
        }
      }
    }
    return false;
  }

}
