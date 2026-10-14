#pragma once

#include "hardware/pio.h"

namespace pimoroni {

  // Whether GPIOs pin_min to pin_max fit the PIO's 32-pin window, based at 0 or 16. The base
  // is set only while no other state machine is claimed, then read back, as the SDK can refuse
  inline bool pio_take_window(PIO pio, uint own_sm, uint pin_min, uint pin_max) {
    bool pio_in_use = false;
    for(uint sm = 0; sm < NUM_PIO_STATE_MACHINES; sm++) {
      pio_in_use |= (sm != own_sm) && pio_sm_is_claimed(pio, sm);
    }
    if(!pio_in_use) {
      pio_set_gpio_base(pio, pin_max >= 32 ? 16 : 0);
    }
    const uint base = pio_get_gpio_base(pio);
    return pin_min >= base && pin_max < base + 32;
  }

  // Whether another claimed state machine on the PIO drives a GPIO in pin_mask, judged by its
  // PINCTRL, as a released driver's pins keep their GPIO function
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
