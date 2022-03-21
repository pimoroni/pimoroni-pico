#include "servo_cluster.hpp"
#include "pwm.hpp"
#include <cstdio>

namespace servo {
  ServoCluster::ServoCluster(PIO pio, uint sm, uint pin_mask, CalibrationType default_type, float freq, bool auto_phase, PWMCluster::Sequence *seq_buffer, PWMCluster::TransitionData *dat_buffer)
    : pwms(pio, sm, pin_mask, seq_buffer, dat_buffer), pwm_frequency(freq) {
    create_servo_states(default_type, auto_phase);
  }

  ServoCluster::ServoCluster(PIO pio, uint sm, uint pin_base, uint pin_count, CalibrationType default_type, float freq, bool auto_phase, PWMCluster::Sequence *seq_buffer, PWMCluster::TransitionData *dat_buffer)
    : pwms(pio, sm, pin_base, pin_count, seq_buffer, dat_buffer), pwm_frequency(freq) {
    create_servo_states(default_type, auto_phase);
  }

  ServoCluster::ServoCluster(PIO pio, uint sm, const uint8_t *pins, uint32_t length, CalibrationType default_type, float freq, bool auto_phase, PWMCluster::Sequence *seq_buffer, PWMCluster::TransitionData *dat_buffer)
    : pwms(pio, sm, pins, length, seq_buffer, dat_buffer), pwm_frequency(freq) {
    create_servo_states(default_type, auto_phase);
  }

  ServoCluster::ServoCluster(PIO pio, uint sm, std::initializer_list<uint8_t> pins, CalibrationType default_type, float freq, bool auto_phase, PWMCluster::Sequence *seq_buffer, PWMCluster::TransitionData *dat_buffer)
    : pwms(pio, sm, pins, seq_buffer, dat_buffer), pwm_frequency(freq) {
    create_servo_states(default_type, auto_phase);
  }

  ServoCluster::ServoCluster(PIO pio, uint sm, uint pin_mask, const Calibration& calibration, float freq, bool auto_phase, PWMCluster::Sequence *seq_buffer, PWMCluster::TransitionData *dat_buffer)
    : pwms(pio, sm, pin_mask, seq_buffer, dat_buffer), pwm_frequency(freq) {
    create_servo_states(calibration, auto_phase);
  }

  ServoCluster::ServoCluster(PIO pio, uint sm, uint pin_base, uint pin_count, const Calibration& calibration, float freq, bool auto_phase, PWMCluster::Sequence *seq_buffer, PWMCluster::TransitionData *dat_buffer)
    : pwms(pio, sm, pin_base, pin_count, seq_buffer, dat_buffer), pwm_frequency(freq) {
    create_servo_states(calibration, auto_phase);
  }

  ServoCluster::ServoCluster(PIO pio, uint sm, const uint8_t *pins, uint32_t length, const Calibration& calibration, float freq, bool auto_phase, PWMCluster::Sequence *seq_buffer, PWMCluster::TransitionData *dat_buffer)
    : pwms(pio, sm, pins, length, seq_buffer, dat_buffer), pwm_frequency(freq) {
    create_servo_states(calibration, auto_phase);
  }

  ServoCluster::ServoCluster(PIO pio, uint sm, std::initializer_list<uint8_t> pins, const Calibration& calibration, float freq, bool auto_phase, PWMCluster::Sequence *seq_buffer, PWMCluster::TransitionData *dat_buffer)
    : pwms(pio, sm, pins, seq_buffer, dat_buffer), pwm_frequency(freq) {
    create_servo_states(calibration, auto_phase);
  }

  ServoCluster::~ServoCluster() {
    delete[] states;
    delete[] servo_phases;
  }

  bool ServoCluster::init() {
    bool success = false;

    if(pwms.init()) {
      // Calculate a suitable pwm wrap period for this frequency
      uint32_t period; uint32_t div256;
      if(pimoroni::PWMCluster::calculate_pwm_factors(pwm_frequency, period, div256)) {
        pwm_period = period;

        // Update the pwm before setting the new wrap
        uint8_t servo_count = pwms.get_chan_count();
        for(uint servo = 0; servo < servo_count; servo++) {
          pwms.set_chan_level(servo, 0, false);
          pwms.set_chan_offset(servo, (uint32_t)(servo_phases[servo] * (float)pwm_period), false);
        }

        // Set the new wrap (should be 1 less than the period to get full 0 to 100%)
        pwms.set_wrap(pwm_period, true); // NOTE Minus 1 not needed here. Maybe should change Wrap behaviour so it is needed, for consistency with hardware pwm?

        // Apply the new divider
        // This is done after loading new PWM values to avoid a lockup condition
        uint8_t div = div256 >> 8;
        uint8_t mod = div256 % 256;
        pwms.set_clkdiv_int_frac(div, mod);

        success = true;
      }
    }

    return success;
  }

  uint8_t ServoCluster::get_count() const {
    return pwms.get_chan_count();
  }

  uint8_t ServoCluster::get_pin(uint8_t servo) const {
    return pwms.get_chan_pin(servo);
  }

  void ServoCluster::enable(uint servo, bool load) {
    assert(servo < pwms.get_chan_count());
    float new_pulse = states[servo].enable();
    apply_pulse(servo, new_pulse, load);
  }

  void ServoCluster::enable(const uint8_t *servos, uint8_t length, bool load) {
    assert(servos != nullptr);
    for(uint8_t i = 0; i < length; i++) {
      enable(servos[i], false);
    }
    if(load)
      pwms.load_pwm();
  }

  void ServoCluster::enable(std::initializer_list<uint8_t> servos, bool load) {
    for(auto servo : servos) {
      enable(servo, false);
    }
    if(load)
      pwms.load_pwm();
  }

  void ServoCluster::enable_all(bool load) {
    uint8_t servo_count = pwms.get_chan_count();
    for(uint8_t servo = 0; servo < servo_count; servo++) {
      enable(servo, false);
    }
    if(load)
      pwms.load_pwm();
  }

  void ServoCluster::disable(uint servo, bool load) {
    assert(servo < pwms.get_chan_count());
    float new_pulse = states[servo].disable();
    apply_pulse(servo, new_pulse, load);
  }

  void ServoCluster::disable(const uint8_t *servos, uint8_t length, bool load) {
    assert(servos != nullptr);
    for(uint8_t i = 0; i < length; i++) {
      disable(servos[i], false);
    }
    if(load)
      pwms.load_pwm();
  }

  void ServoCluster::disable(std::initializer_list<uint8_t> servos, bool load) {
    for(auto servo : servos) {
      disable(servo, false);
    }
    if(load)
      pwms.load_pwm();
  }

  void ServoCluster::disable_all(bool load) {
    uint8_t servo_count = pwms.get_chan_count();
    for(uint8_t servo = 0; servo < servo_count; servo++) {
      disable(servo, false);
    }
    if(load)
      pwms.load_pwm();
  }

  bool ServoCluster::is_enabled(uint servo) const {
    assert(servo < pwms.get_chan_count());
    return states[servo].is_enabled();
  }

  float ServoCluster::get_pulse(uint servo) const {
    assert(servo < pwms.get_chan_count());
    return states[servo].get_pulse();
  }

  void ServoCluster::set_pulse(uint servo, float pulse, bool load) {
    assert(servo < pwms.get_chan_count());
    float new_pulse = states[servo].set_pulse(pulse);
    apply_pulse(servo, new_pulse, load);
  }

  void ServoCluster::set_pulse(const uint8_t *servos, uint8_t length, float pulse, bool load) {
    assert(servos != nullptr);
    for(uint8_t i = 0; i < length; i++) {
      set_pulse(servos[i], pulse, false);
    }
    if(load)
      pwms.load_pwm();
  }

  void ServoCluster::set_pulse(std::initializer_list<uint8_t> servos, float pulse, bool load) {
    for(auto servo : servos) {
      set_pulse(servo, pulse, false);
    }
    if(load)
      pwms.load_pwm();
  }

  void ServoCluster::set_all_pulses(float pulse, bool load) {
    uint8_t servo_count = pwms.get_chan_count();
    for(uint8_t servo = 0; servo < servo_count; servo++) {
      set_pulse(servo, pulse, false);
    }
    if(load)
      pwms.load_pwm();
  }

  float ServoCluster::get_value(uint servo) const {
    assert(servo < pwms.get_chan_count());
    return states[servo].get_value();
  }

  void ServoCluster::set_value(uint servo, float value, bool load) {
    assert(servo < pwms.get_chan_count());
    float new_pulse = states[servo].set_value(value);
    apply_pulse(servo, new_pulse, load);
  }

  void ServoCluster::set_value(const uint8_t *servos, uint8_t length, float value, bool load) {
    assert(servos != nullptr);
    for(uint8_t i = 0; i < length; i++) {
      set_value(servos[i], value, false);
    }
    if(load)
      pwms.load_pwm();
  }

  void ServoCluster::set_value(std::initializer_list<uint8_t> servos, float value, bool load) {
    for(auto servo : servos) {
      set_value(servo, value, false);
    }
    if(load)
      pwms.load_pwm();
  }

  void ServoCluster::set_all_values(float value, bool load) {
    uint8_t servo_count = pwms.get_chan_count();
    for(uint8_t servo = 0; servo < servo_count; servo++) {
      set_value(servo, value, false);
    }
    if(load)
      pwms.load_pwm();
  }

  float ServoCluster::get_phase(uint servo) const {
    assert(servo < pwms.get_chan_count());
    return servo_phases[servo];
  }

  void ServoCluster::set_phase(uint servo, float phase, bool load) {
    assert(servo < pwms.get_chan_count());
    servo_phases[servo] = MIN(MAX(phase, 0.0f), 1.0f);
    pwms.set_chan_offset(servo, (uint32_t)(servo_phases[servo] * (float)pwms.get_wrap()), load);
  }

  void ServoCluster::set_phase(const uint8_t *servos, uint8_t length, float phase, bool load) {
    assert(servos != nullptr);
    for(uint8_t i = 0; i < length; i++) {
      set_phase(servos[i], phase, false);
    }
    if(load)
      pwms.load_pwm();
  }

  void ServoCluster::set_phase(std::initializer_list<uint8_t> servos, float phase, bool load) {
    for(auto servo : servos) {
      set_phase(servo, phase, false);
    }
    if(load)
      pwms.load_pwm();
  }

  void ServoCluster::set_all_phases(float phase, bool load) {
    uint8_t servo_count = pwms.get_chan_count();
    for(uint8_t servo = 0; servo < servo_count; servo++) {
      set_phase(servo, phase, false);
    }
    if(load)
      pwms.load_pwm();
  }

  float ServoCluster::get_frequency() const {
    return pwm_frequency;
  }

  bool ServoCluster::set_frequency(float freq) {
    bool success = false;

    if((freq >= ServoState::MIN_FREQUENCY) && (freq <= ServoState::MAX_FREQUENCY)) {
      // Calculate a suitable pwm wrap period for this frequency
      uint32_t period; uint32_t div256;
      if(pimoroni::PWMCluster::calculate_pwm_factors(freq, period, div256)) {

        pwm_period = period;
        pwm_frequency = freq;

        // Update the pwm before setting the new wrap
        uint8_t servo_count = pwms.get_chan_count();
        for(uint servo = 0; servo < servo_count; servo++) {
          if(states[servo].is_enabled()) {
            apply_pulse(servo, states[servo].get_pulse(), false);
          }
          pwms.set_chan_offset(servo, (uint32_t)(servo_phases[servo] * (float)pwm_period), false);
        }

        // Set the new wrap (should be 1 less than the period to get full 0 to 100%)
        pwms.set_wrap(pwm_period, true);

        // Apply the new divider
        uint16_t div = div256 >> 8;
        uint8_t mod = div256 % 256;
        pwms.set_clkdiv_int_frac(div, mod);

        success = true;
      }
    }
    return success;
  }

  float ServoCluster::get_min_value(uint servo) const {
    assert(is_assigned(servo));
    return states[servo].get_min_value();
  }

  float ServoCluster::get_mid_value(uint servo) const {
    assert(is_assigned(servo));
    return states[servo].get_mid_value();
  }

  float ServoCluster::get_max_value(uint servo) const {
    assert(is_assigned(servo));
    return states[servo].get_max_value();
  }

  void ServoCluster::to_min(uint servo, bool load) {
    assert(is_assigned(servo));
    float new_pulse = states[servo].to_min();
    apply_pulse(servo, new_pulse, load);
  }

  void ServoCluster::to_min(const uint8_t *servos, uint8_t length, bool load) {
    assert(servos != nullptr);
    for(uint8_t i = 0; i < length; i++) {
      to_min(servos[i], false);
    }
    if(load)
      pwms.load_pwm();
  }

  void ServoCluster::to_min(std::initializer_list<uint8_t> servos, bool load) {
    for(auto servo : servos) {
      to_min(servo, false);
    }
    if(load)
      pwms.load_pwm();
  }

  void ServoCluster::all_to_min(bool load) {
    uint8_t servo_count = pwms.get_chan_count();
    for(uint8_t servo = 0; servo < servo_count; servo++) {
      to_min(servo, false);
    }
    if(load)
      pwms.load_pwm();
  }

  void ServoCluster::to_mid(uint servo, bool load) {
    assert(is_assigned(servo));
    float new_pulse = states[servo].to_mid();
    apply_pulse(servo, new_pulse, load);
  }

  void ServoCluster::to_mid(const uint8_t *servos, uint8_t length, bool load) {
    assert(servos != nullptr);
    for(uint8_t i = 0; i < length; i++) {
      to_mid(servos[i], false);
    }
    if(load)
      pwms.load_pwm();
  }

  void ServoCluster::to_mid(std::initializer_list<uint8_t> servos, bool load) {
    for(auto servo : servos) {
      to_mid(servo, false);
    }
    if(load)
      pwms.load_pwm();
  }

  void ServoCluster::all_to_mid(bool load) {
    uint8_t servo_count = pwms.get_chan_count();
    for(uint8_t servo = 0; servo < servo_count; servo++) {
      to_mid(servo, false);
    }
    if(load)
      pwms.load_pwm();
  }

  void ServoCluster::to_max(uint servo, bool load) {
    assert(is_assigned(servo));
    float new_pulse = states[servo].to_max();
    apply_pulse(servo, new_pulse, load);
  }

  void ServoCluster::to_max(const uint8_t *servos, uint8_t length, bool load) {
    assert(servos != nullptr);
    for(uint8_t i = 0; i < length; i++) {
      to_max(servos[i], false);
    }
    if(load)
      pwms.load_pwm();
  }

  void ServoCluster::to_max(std::initializer_list<uint8_t> servos, bool load) {
    for(auto servo : servos) {
      to_max(servo, false);
    }
    if(load)
      pwms.load_pwm();
  }

  void ServoCluster::all_to_max(bool load) {
    uint8_t servo_count = pwms.get_chan_count();
    for(uint8_t servo = 0; servo < servo_count; servo++) {
      to_max(servo, false);
    }
    if(load)
      pwms.load_pwm();
  }

  void ServoCluster::to_percent(uint servo, float in, float in_min, float in_max, bool load) {
    assert(is_assigned(servo));
    float new_pulse = states[servo].to_percent(in, in_min, in_max);
    apply_pulse(servo, new_pulse, load);
  }

  void ServoCluster::to_percent(const uint8_t *servos, uint8_t length, float in, float in_min, float in_max, bool load) {
    assert(servos != nullptr);
    for(uint8_t i = 0; i < length; i++) {
      to_percent(servos[i], in, in_min, in_max, false);
    }
    if(load)
      pwms.load_pwm();
  }

  void ServoCluster::to_percent(std::initializer_list<uint8_t> servos, float in, float in_min, float in_max, bool load) {
    for(auto servo : servos) {
      to_percent(servo, in, in_min, in_max, false);
    }
    if(load)
      pwms.load_pwm();
  }

  void ServoCluster::all_to_percent(float in, float in_min, float in_max, bool load) {
    uint8_t servo_count = pwms.get_chan_count();
    for(uint8_t servo = 0; servo < servo_count; servo++) {
      to_percent(servo, in, in_min, in_max, false);
    }
    if(load)
      pwms.load_pwm();
  }

  void ServoCluster::to_percent(uint servo, float in, float in_min, float in_max, float value_min, float value_max, bool load) {
    assert(is_assigned(servo));
    float new_pulse = states[servo].to_percent(in, in_min, in_max, value_min, value_max);
    apply_pulse(servo, new_pulse, load);
  }

  void ServoCluster::to_percent(const uint8_t *servos, uint8_t length, float in, float in_min, float in_max, float value_min, float value_max, bool load) {
    assert(servos != nullptr);
    for(uint8_t i = 0; i < length; i++) {
      to_percent(servos[i], in, in_min, in_max, value_min, value_max, false);
    }
    if(load)
      pwms.load_pwm();
  }

  void ServoCluster::to_percent(std::initializer_list<uint8_t> servos, float in, float in_min, float in_max, float value_min, float value_max, bool load) {
    for(auto servo : servos) {
      to_percent(servo, in, in_min, in_max, value_min, value_max, false);
    }
    if(load)
      pwms.load_pwm();
  }

  void ServoCluster::all_to_percent(float in, float in_min, float in_max, float value_min, float value_max, bool load) {
    uint8_t servo_count = pwms.get_chan_count();
    for(uint8_t servo = 0; servo < servo_count; servo++) {
      to_percent(servo, in, in_min, in_max, value_min, value_max, false);
    }
    if(load)
      pwms.load_pwm();
  }

  Calibration& ServoCluster::calibration(uint servo) {
    assert(is_assigned(servo));
    return states[servo].calibration();
  }

  const Calibration& ServoCluster::calibration(uint servo) const {
    assert(is_assigned(servo));
    return states[servo].calibration();
  }

  void ServoCluster::load() {
    pwms.load_pwm();
  }

  void ServoCluster::apply_pulse(uint servo, float pulse, bool load) {
    pwms.set_chan_level(servo, ServoState::pulse_to_level(pulse, pwm_period, pwm_frequency), load);
  }

  void ServoCluster::create_servo_states(CalibrationType default_type, bool auto_phase) {
    uint8_t servo_count = pwms.get_chan_count();
    if(servo_count > 0) {
      states = new ServoState[servo_count];
      servo_phases = new float[servo_count];

      for(uint servo = 0; servo < servo_count; servo++) {
        states[servo] = ServoState(default_type);
        servo_phases[servo] = (auto_phase) ? (float)servo / (float)servo_count : 0.0f;
      }
    }
  }

  void ServoCluster::create_servo_states(const Calibration& calibration, bool auto_phase) {
    uint8_t servo_count = pwms.get_chan_count();
    if(servo_count > 0) {
      states = new ServoState[servo_count];
      servo_phases = new float[servo_count];

      for(uint servo = 0; servo < servo_count; servo++) {
        states[servo] = ServoState(calibration);
        servo_phases[servo] = (auto_phase) ? (float)servo / (float)servo_count : 0.0f;
      }
    }
  }
};