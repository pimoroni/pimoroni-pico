#include "brushless.hpp"
#include "pwm.hpp"
#include "hardware/clocks.h"

namespace brushless {

  Brushless::Brushless(const pin_trio &pins, Direction direction, float freq)
    : signal_pins(pins), inverse_pins(), inverse_pins_used(false)
    , pwm_period(1), pwm_frequency(freq), state(direction) {
    find_conflicts();
  }

  Brushless::Brushless(const bridge_trio &bridges, Direction direction, float freq)
    : signal_pins(bridges.u.first, bridges.v.first, bridges.w.first)
    , inverse_pins(bridges.u.second, bridges.v.second, bridges.w.second)
    , inverse_pins_used(true)
    , pwm_period(1), pwm_frequency(freq), state(direction) {
    find_conflicts();
  }

  Brushless::~Brushless() {
    uint8_t pins[6];
    uint8_t pin_count = owned_pins(pins);
    hw_clear_bits(&pwm_hw->en, slice_mask());
    for(uint8_t i = 0; i < pin_count; i++) {
      gpio_set_function(pins[i], GPIO_FUNC_NULL);
    }
  }

  bool Brushless::init() {
    if(conflict_pin_a != PIN_UNUSED) {
      return false;
    }
    return configure_slices(pwm_frequency);
  }

  bool Brushless::pin_conflict(uint &pin_a, uint &pin_b) const {
    pin_a = conflict_pin_a;
    pin_b = conflict_pin_b;
    return conflict_pin_a != PIN_UNUSED;
  }

  pin_trio Brushless::pins() const {
    return signal_pins;
  }

  bool Brushless::has_inverses() const {
    return inverse_pins_used;
  }

  void Brushless::enable() {
    state.enable();
    apply_duties();
  }

  void Brushless::disable() {
    state.disable();
    apply_duties();
  }

  bool Brushless::is_enabled() const {
    return state.is_enabled();
  }

  float Brushless::u_duty() const {
    return state.get_duty(0);
  }

  float Brushless::v_duty() const {
    return state.get_duty(1);
  }

  float Brushless::w_duty() const {
    return state.get_duty(2);
  }

  void Brushless::duties(float u, float v, float w) {
    state.set_duties(u, v, w);
    apply_duties();
  }

  float Brushless::frequency() const {
    return pwm_frequency;
  }

  bool Brushless::frequency(float freq) {
    if((freq < BrushlessState::MIN_FREQUENCY) || (freq > BrushlessState::MAX_FREQUENCY)) {
      return false;
    }
    if(!configure_slices(freq)) {
      return false;
    }
    pwm_frequency = freq;
    return true;
  }

  Direction Brushless::direction() const {
    return state.get_direction();
  }

  void Brushless::direction(Direction direction) {
    state.set_direction(direction);
    apply_duties();
  }

  uint8_t Brushless::owned_pins(uint8_t pins_out[6]) const {
    uint8_t count = 0;
    pins_out[count++] = signal_pins.u;
    pins_out[count++] = signal_pins.v;
    pins_out[count++] = signal_pins.w;
    if(inverse_pins_used) {
      pins_out[count++] = inverse_pins.u;
      pins_out[count++] = inverse_pins.v;
      pins_out[count++] = inverse_pins.w;
    }
    return count;
  }

  void Brushless::find_conflicts() {
    uint8_t pins[6];
    uint8_t pin_count = owned_pins(pins);
    for(uint8_t i = 0; i < pin_count; i++) {
      for(uint8_t j = i + 1; j < pin_count; j++) {
        // Pins sharing a slice channel emit the same signal, so cannot carry two outputs
        if(pwm_gpio_to_slice_num(pins[i]) == pwm_gpio_to_slice_num(pins[j]) &&
           pwm_gpio_to_channel(pins[i]) == pwm_gpio_to_channel(pins[j])) {
          conflict_pin_a = pins[i];
          conflict_pin_b = pins[j];
          return;
        }
      }
    }
  }

  uint32_t Brushless::slice_mask() const {
    uint8_t pins[6];
    uint8_t pin_count = owned_pins(pins);
    uint32_t mask = 0;
    for(uint8_t i = 0; i < pin_count; i++) {
      mask |= (1u << pwm_gpio_to_slice_num(pins[i]));
    }
    return mask;
  }

  bool Brushless::configure_slices(float freq) {
    // Phase-correct counting doubles the period, so target twice the frequency
    uint16_t period; uint16_t div16;
    if(!pimoroni::calculate_pwm_factors(freq * 2.0f, period, div16)) {
      return false;
    }
    pwm_period = period;

    uint32_t mask = slice_mask();
    hw_clear_bits(&pwm_hw->en, mask);

    // Phase-correct pulses are symmetric about the counter apex, so channels
    // on synchronised counters are concentric whatever their levels
    pwm_config pwm_cfg = pwm_get_default_config();
    pwm_config_set_phase_correct(&pwm_cfg, true);
    pwm_config_set_wrap(&pwm_cfg, period - 1);
    pwm_config_set_clkdiv_int_frac(&pwm_cfg, div16 >> 4, div16 & 0xF);

    for(uint slice = 0; slice < NUM_PWM_SLICES; slice++) {
      if(mask & (1u << slice)) {
        pwm_init(slice, &pwm_cfg, false);  // also zeroes the counter, for the synchronised start
      }
    }

    uint8_t pins[6];
    uint8_t pin_count = owned_pins(pins);
    for(uint8_t i = 0; i < pin_count; i++) {
      gpio_set_function(pins[i], GPIO_FUNC_PWM);
    }

    // An inverse leg is its signal's compare level with the output inverted,
    // an exact complement with coincident edges and no deadtime: the gate
    // driver owns its own. Only owned channels have their invert bit set
    if(inverse_pins_used) {
      const uint8_t inverses[] = {inverse_pins.u, inverse_pins.v, inverse_pins.w};
      for(auto pin : inverses) {
        uint32_t invert_bit = (pwm_gpio_to_channel(pin) == PWM_CHAN_A) ? PWM_CH0_CSR_A_INV_BITS : PWM_CH0_CSR_B_INV_BITS;
        hw_set_bits(&pwm_hw->slice[pwm_gpio_to_slice_num(pin)].csr, invert_bit);
      }
    }

    apply_duties();

    // One register write starts every owned slice's counter together,
    // without touching slices owned by others
    hw_set_bits(&pwm_hw->en, mask);
    return true;
  }

  void Brushless::apply_duties() {
    const uint8_t signals[] = {signal_pins.u, signal_pins.v, signal_pins.w};
    const uint8_t inverses[] = {inverse_pins.u, inverse_pins.v, inverse_pins.w};
    for(uint8_t leg = 0; leg < BrushlessState::NUM_LEGS; leg++) {
      uint32_t level = BrushlessState::duty_to_level(state.applied_duty(leg), pwm_period);
      pwm_set_gpio_level(signals[leg], level);
      if(inverse_pins_used) {
        pwm_set_gpio_level(inverses[leg], level);
      }
    }
  }

}
