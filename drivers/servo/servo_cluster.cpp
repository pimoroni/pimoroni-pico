#include "servo_cluster.hpp"
#include "pwm.hpp"
#include <cstdio>

namespace servo {
  ServoCluster::ServoCluster(PIO pio, uint sm, uint channel_mask)
    : pwms(pio, sm, channel_mask) {

    // Calculate a suitable pwm wrap period for this frequency
    uint32_t period; uint16_t div16;
    if(pimoroni::PWMCluster::calculate_pwm_factors(pwm_frequency, period, div16)) {
      pwm_period = period;

      // Update the pwm before setting the new wrap
      for(uint servo = 0; servo < NUM_BANK0_GPIOS; servo++) {
        pwms.set_chan_level(servo, 0, false);
      }

      // Set the new wrap (should be 1 less than the period to get full 0 to 100%)
      pwms.set_wrap(pwm_period); // NOTE Minus 1 not needed here. Maybe should change Wrap behaviour so it is needed, for consistency with hardware pwm?

      // Apply the new divider
      // This is done after loading new PWM values to avoid a lockup condition
      uint8_t div = div16 >> 4;
      uint8_t mod = div16 % 16;
      pwms.set_clkdiv_int_frac(div, mod);
    }
  }

  ServoCluster::~ServoCluster() {
  }

  bool ServoCluster::init() {
    // pwm_cfg = pwm_get_default_config();
    // pwm_config_set_wrap(&pwm_cfg, 20000 - 1);

    // float div = clock_get_hz(clk_sys) / 1000000;
    // pwm_config_set_clkdiv(&pwm_cfg, div);

    // pwm_init(pwm_gpio_to_slice_num(pin), &pwm_cfg, true);
    // gpio_set_function(pin, GPIO_FUNC_PWM);

    // pwm_set_gpio_level(pin, 0);

    return true;
  }

  uint ServoCluster::get_pin_mask() const {
    return pwms.get_chan_mask();
  }

  void ServoCluster::enable(uint servo, bool load) {
    if(servo < NUM_BANK0_GPIOS) {
      float new_pulse = servos[servo].enable();
      apply_pulse(servo, new_pulse, load);
    }
  }

  void ServoCluster::disable(uint servo, bool load) {
    if(servo < NUM_BANK0_GPIOS) {
      float new_pulse = servos[servo].disable();
      apply_pulse(servo, new_pulse, load);
    }
  }

  bool ServoCluster::is_enabled(uint servo) const {
    if(servo < NUM_BANK0_GPIOS)
      return servos[servo].is_enabled();
    else
      return false;
  }

  float ServoCluster::get_value(uint servo) const {
    if(servo < NUM_BANK0_GPIOS)
      return servos[servo].get_value();
    else
      return 0.0f;
  }

  void ServoCluster::set_value(uint servo, float value, bool load) {
    if(servo < NUM_BANK0_GPIOS) {
      float new_pulse = servos[servo].set_value(value);
      apply_pulse(servo, new_pulse, load);
    }
  }

  float ServoCluster::get_pulse(uint servo) const {
    if(servo < NUM_BANK0_GPIOS)
      return servos[servo].get_pulse();
    else
      return 0.0f;
  }

  void ServoCluster::set_pulse(uint servo, float pulse, bool load) {
    if(servo < NUM_BANK0_GPIOS) {
      float new_pulse = servos[servo].set_pulse(pulse);
      apply_pulse(servo, new_pulse, load);
    }
  }

 float ServoCluster::get_frequency() const {
    return pwm_frequency;
  }

  bool ServoCluster::set_frequency(float freq) {
    bool success = false;

    if((freq >= ServoState::MIN_FREQUENCY) && (freq <= ServoState::MAX_FREQUENCY)) {
      // Calculate a suitable pwm wrap period for this frequency
      uint32_t period; uint16_t div16;
      if(pimoroni::PWMCluster::calculate_pwm_factors(freq, period, div16)) {

        pwm_period = period;
        pwm_frequency = freq;

        // Update the pwm before setting the new wrap
        for(uint servo = 0; servo < NUM_BANK0_GPIOS; servo++) {
          float current_pulse = servos[servo].get_pulse();
          apply_pulse(servo, current_pulse, false);
        }

        // Set the new wrap (should be 1 less than the period to get full 0 to 100%)
        pwms.set_wrap(pwm_period, true);

        // Apply the new divider
        // This is done after loading new PWM values to avoid a lockup condition
        uint8_t div = div16 >> 4;
        uint8_t mod = div16 % 16;
        pwms.set_clkdiv_int_frac(div, mod);

        success = true;
      }
    }
    return success;
  }

  float ServoCluster::get_min_value(uint servo) const {
    if(servo < NUM_BANK0_GPIOS)
      return servos[servo].get_min_value();
    else
      return 0.0f;
  }

  float ServoCluster::get_mid_value(uint servo) const {
    if(servo < NUM_BANK0_GPIOS)
      return servos[servo].get_mid_value();
    else
      return 0.0f;
  }

  float ServoCluster::get_max_value(uint servo) const {
    if(servo < NUM_BANK0_GPIOS)
      return servos[servo].get_max_value();
    else
      return 0.0f;
  }

  void ServoCluster::to_min(uint servo, bool load) {
    if(servo < NUM_BANK0_GPIOS) {
      float new_pulse = servos[servo].to_min();
      apply_pulse(servo, new_pulse, load);
    }
  }

  void ServoCluster::to_mid(uint servo, bool load) {
    if(servo < NUM_BANK0_GPIOS) {
      float new_pulse = servos[servo].to_mid();
      apply_pulse(servo, new_pulse, load);
    }
  }

  void ServoCluster::to_max(uint servo, bool load) {
    if(servo < NUM_BANK0_GPIOS) {
      float new_pulse = servos[servo].to_max();
      apply_pulse(servo, new_pulse, load);
    }
  }

  void ServoCluster::to_percent(uint servo, float in, float in_min, float in_max, bool load) {
    if(servo < NUM_BANK0_GPIOS) {
      float new_pulse = servos[servo].to_percent(in, in_min, in_max);
      apply_pulse(servo, new_pulse, load);
    }
  }

  void ServoCluster::to_percent(uint servo, float in, float in_min, float in_max, float value_min, float value_max, bool load) {
    if(servo < NUM_BANK0_GPIOS) {
      float new_pulse = servos[servo].to_percent(in, in_min, in_max, value_min, value_max);
      apply_pulse(servo, new_pulse, load);
    }
  }

  Calibration* ServoCluster::calibration(uint servo) {
    if(servo < NUM_BANK0_GPIOS)
        return &servos[servo].calibration();
    else
        return nullptr;
  }

  const Calibration* ServoCluster::calibration(uint servo) const {
    if(servo < NUM_BANK0_GPIOS)
        return &servos[servo].calibration();
    else
        return nullptr;
  }

  void ServoCluster::apply_pulse(uint servo, float pulse, bool load) {
    pwms.set_chan_level(servo, ServoState::pulse_to_level(pulse, pwm_period, pwm_frequency), load);
  }
};