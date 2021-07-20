#include "button.hpp"

namespace pimoroni {
    bool Button::raw() {
      if(polarity == Polarity::ACTIVE_LOW){
        return !gpio_get(pin);
      } else {
        return gpio_get(pin);
      }
    }

    bool Button::read() {
      auto time = millis();
      bool state = raw();
      bool changed = state != last_state;
      last_state = state;

      if(changed) {
        if(state) {
          pressed_time = time;
          pressed = true;
          last_time = time;
          return true;
        }
        else {
          pressed_time = 0;
          pressed = false;
          last_time = 0;
        }
      }
      // Shortcut for no auto-repeat
      if(repeat_time == 0) return false;

      if(pressed) {
        uint32_t repeat_rate = repeat_time;
        if(hold_time > 0 && time - pressed_time > hold_time) {
          repeat_rate /= 3;
        }
        if(time - last_time > repeat_rate) {
          last_time = time;
          return true;
        }
      }

      return false;
    }
};