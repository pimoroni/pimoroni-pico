#include "servo_cluster.hpp"

namespace servo {
  ServoCluster::ServoCluster(PIO pio, uint sm, uint channel_mask)
    : multi_pwm(pio, sm, channel_mask) {
    multi_pwm.set_wrap(20000);
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

  void ServoCluster::enable(uint servo, bool load) {
    if(servo < NUM_BANK0_GPIOS) {
      float new_pulse = servos[servo].enable();
      multi_pwm.set_chan_level(servo, Converter::pulse_to_level(new_pulse, 20000), load);
    }
  }

  void ServoCluster::disable(uint servo, bool load) {
    if(servo < NUM_BANK0_GPIOS) {
      float new_pulse = servos[servo].disable();
      multi_pwm.set_chan_level(servo, Converter::pulse_to_level(new_pulse, 20000), load);
    }
  }

  bool ServoCluster::is_enabled(uint servo) {
    if(servo < NUM_BANK0_GPIOS)
      return servos[servo].is_enabled();
    else
      return false;
  }

  float ServoCluster::get_value(uint servo) {
    if(servo < NUM_BANK0_GPIOS)
      return servos[servo].get_value();
    else
      return 0.0f;
  }

  void ServoCluster::set_value(uint servo, float value, bool load) {
    if(servo < NUM_BANK0_GPIOS) {
      float new_pulse = servos[servo].set_value(value);
      multi_pwm.set_chan_level(servo, Converter::pulse_to_level(new_pulse, 20000), load);
    }
  }

  float ServoCluster::get_pulse(uint servo) {
    if(servo < NUM_BANK0_GPIOS)
      return servos[servo].get_pulse();
    else
      return 0.0f;
  }

  void ServoCluster::set_pulse(uint servo, float pulse, bool load) {
    if(servo < NUM_BANK0_GPIOS) {
      float new_pulse = servos[servo].set_pulse(pulse);
      multi_pwm.set_chan_level(servo, Converter::pulse_to_level(new_pulse, 20000), load);
    }
  }

  void ServoCluster::to_min(uint servo, bool load) {
    if(servo < NUM_BANK0_GPIOS) {
      float new_pulse = servos[servo].to_min();
      multi_pwm.set_chan_level(servo, Converter::pulse_to_level(new_pulse, 20000), load);
    }
  }

  void ServoCluster::to_mid(uint servo, bool load) {
    if(servo < NUM_BANK0_GPIOS) {
      float new_pulse = servos[servo].to_mid();
      multi_pwm.set_chan_level(servo, Converter::pulse_to_level(new_pulse, 20000), load);
    }
  }

  void ServoCluster::to_max(uint servo, bool load) {
    if(servo < NUM_BANK0_GPIOS) {
      float new_pulse = servos[servo].to_max();
      multi_pwm.set_chan_level(servo, Converter::pulse_to_level(new_pulse, 20000), load);
    }
  }

  void ServoCluster::to_percent(uint servo, float in, float in_min, float in_max, bool load) {
    if(servo < NUM_BANK0_GPIOS) {
      float new_pulse = servos[servo].to_percent(in, in_min, in_max);
      multi_pwm.set_chan_level(servo, Converter::pulse_to_level(new_pulse, 20000), load);
    }
  }

  void ServoCluster::to_percent(uint servo, float in, float in_min, float in_max, float value_min, float value_max, bool load) {
    if(servo < NUM_BANK0_GPIOS) {
      float new_pulse = servos[servo].to_percent(in, in_min, in_max, value_min, value_max);
      multi_pwm.set_chan_level(servo, Converter::pulse_to_level(new_pulse, 20000), load);
    }
  }

  Calibration* ServoCluster::calibration(uint servo) {
    if(servo < NUM_BANK0_GPIOS)
        return &servos[servo].calibration();
    else
        return nullptr;
  }
};