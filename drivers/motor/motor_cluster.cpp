#include "motor_cluster.hpp"
#include "pwm.hpp"
#include <cstdio>

namespace motor {
  MotorCluster::MotorCluster(PIO pio, uint sm, uint pin_mask, CalibrationType default_type, float freq, bool auto_phase, PWMCluster::Sequence *seq_buffer, PWMCluster::TransitionData *dat_buffer)
    : pwms(pio, sm, pin_mask, seq_buffer, dat_buffer), pwm_frequency(freq) {
    create_motor_states(default_type, auto_phase);
  }

  MotorCluster::MotorCluster(PIO pio, uint sm, uint pin_base, uint pin_count, CalibrationType default_type, float freq, bool auto_phase, PWMCluster::Sequence *seq_buffer, PWMCluster::TransitionData *dat_buffer)
    : pwms(pio, sm, pin_base, pin_count, seq_buffer, dat_buffer), pwm_frequency(freq) {
    create_motor_states(default_type, auto_phase);
  }

  MotorCluster::MotorCluster(PIO pio, uint sm, const uint8_t *pins, uint32_t length, CalibrationType default_type, float freq, bool auto_phase, PWMCluster::Sequence *seq_buffer, PWMCluster::TransitionData *dat_buffer)
    : pwms(pio, sm, pins, length, seq_buffer, dat_buffer), pwm_frequency(freq) {
    create_motor_states(default_type, auto_phase);
  }

  MotorCluster::MotorCluster(PIO pio, uint sm, std::initializer_list<uint8_t> pins, CalibrationType default_type, float freq, bool auto_phase, PWMCluster::Sequence *seq_buffer, PWMCluster::TransitionData *dat_buffer)
    : pwms(pio, sm, pins, seq_buffer, dat_buffer), pwm_frequency(freq) {
    create_motor_states(default_type, auto_phase);
  }

  MotorCluster::MotorCluster(PIO pio, uint sm, uint pin_mask, const Calibration& calibration, float freq, bool auto_phase, PWMCluster::Sequence *seq_buffer, PWMCluster::TransitionData *dat_buffer)
    : pwms(pio, sm, pin_mask, seq_buffer, dat_buffer), pwm_frequency(freq) {
    create_motor_states(calibration, auto_phase);
  }

  MotorCluster::MotorCluster(PIO pio, uint sm, uint pin_base, uint pin_count, const Calibration& calibration, float freq, bool auto_phase, PWMCluster::Sequence *seq_buffer, PWMCluster::TransitionData *dat_buffer)
    : pwms(pio, sm, pin_base, pin_count, seq_buffer, dat_buffer), pwm_frequency(freq) {
    create_motor_states(calibration, auto_phase);
  }

  MotorCluster::MotorCluster(PIO pio, uint sm, const uint8_t *pins, uint32_t length, const Calibration& calibration, float freq, bool auto_phase, PWMCluster::Sequence *seq_buffer, PWMCluster::TransitionData *dat_buffer)
    : pwms(pio, sm, pins, length, seq_buffer, dat_buffer), pwm_frequency(freq) {
    create_motor_states(calibration, auto_phase);
  }

  MotorCluster::MotorCluster(PIO pio, uint sm, std::initializer_list<uint8_t> pins, const Calibration& calibration, float freq, bool auto_phase, PWMCluster::Sequence *seq_buffer, PWMCluster::TransitionData *dat_buffer)
    : pwms(pio, sm, pins, seq_buffer, dat_buffer), pwm_frequency(freq) {
    create_motor_states(calibration, auto_phase);
  }

  MotorCluster::~MotorCluster() {
    delete[] states;
    delete[] motor_phases;
  }

  bool MotorCluster::init() {
    bool success = false;

    if(pwms.init()) {
      // Calculate a suitable pwm wrap period for this frequency
      uint32_t period; uint32_t div256;
      if(pimoroni::PWMCluster::calculate_pwm_factors(pwm_frequency, period, div256)) {
        pwm_period = period;

        // Update the pwm before setting the new wrap
        uint8_t motor_count = pwms.get_chan_count();
        for(uint8_t motor = 0; motor < motor_count; motor++) {
          pwms.set_chan_level(motor, 0, false);
          pwms.set_chan_offset(motor, (uint32_t)(motor_phases[motor] * (float)pwm_period), false);
        }

        // Set the new wrap (should be 1 less than the period to get full 0 to 100%)
        pwms.set_wrap(pwm_period, true); // NOTE Minus 1 not needed here. Maybe should change Wrap behaviour so it is needed, for consistency with hardware pwm?

        // Apply the new divider
        // This is done after loading new PWM speeds to avoid a lockup condition
        uint8_t div = div256 >> 8;
        uint8_t mod = div256 % 256;
        pwms.set_clkdiv_int_frac(div, mod);

        success = true;
      }
    }

    return success;
  }

  uint8_t MotorCluster::count() const {
    return pwms.get_chan_count();
  }

  uint8_t MotorCluster::pin(uint8_t motor) const {
    return pwms.get_chan_pin(motor);
  }

  void MotorCluster::enable(uint8_t motor, bool load) {
    assert(motor < pwms.get_chan_count());
    float new_pulse = states[motor].enable();
    apply_pulse(motor, new_pulse, load);
  }

  void MotorCluster::enable(const uint8_t *motors, uint8_t length, bool load) {
    assert(motors != nullptr);
    for(uint8_t i = 0; i < length; i++) {
      enable(motors[i], false);
    }
    if(load)
      pwms.load_pwm();
  }

  void MotorCluster::enable(std::initializer_list<uint8_t> motors, bool load) {
    for(auto motor : motors) {
      enable(motor, false);
    }
    if(load)
      pwms.load_pwm();
  }

  void MotorCluster::enable_all(bool load) {
    uint8_t motor_count = pwms.get_chan_count();
    for(uint8_t motor = 0; motor < motor_count; motor++) {
      enable(motor, false);
    }
    if(load)
      pwms.load_pwm();
  }

  void MotorCluster::disable(uint8_t motor, bool load) {
    assert(motor < pwms.get_chan_count());
    float new_pulse = states[motor].disable();
    apply_pulse(motor, new_pulse, load);
  }

  void MotorCluster::disable(const uint8_t *motors, uint8_t length, bool load) {
    assert(motors != nullptr);
    for(uint8_t i = 0; i < length; i++) {
      disable(motors[i], false);
    }
    if(load)
      pwms.load_pwm();
  }

  void MotorCluster::disable(std::initializer_list<uint8_t> motors, bool load) {
    for(auto motor : motors) {
      disable(motor, false);
    }
    if(load)
      pwms.load_pwm();
  }

  void MotorCluster::disable_all(bool load) {
    uint8_t motor_count = pwms.get_chan_count();
    for(uint8_t motor = 0; motor < motor_count; motor++) {
      disable(motor, false);
    }
    if(load)
      pwms.load_pwm();
  }

  bool MotorCluster::is_enabled(uint8_t motor) const {
    assert(motor < pwms.get_chan_count());
    return states[motor].is_enabled();
  }

  float MotorCluster::pulse(uint8_t motor) const {
    assert(motor < pwms.get_chan_count());
    return states[motor].get_pulse();
  }

  void MotorCluster::pulse(uint8_t motor, float pulse, bool load) {
    assert(motor < pwms.get_chan_count());
    float new_pulse = states[motor].set_pulse_with_return(pulse);
    apply_pulse(motor, new_pulse, load);
  }

  void MotorCluster::pulse(const uint8_t *motors, uint8_t length, float pulse, bool load) {
    assert(motors != nullptr);
    for(uint8_t i = 0; i < length; i++) {
      this->pulse(motors[i], pulse, false);
    }
    if(load)
      pwms.load_pwm();
  }

  void MotorCluster::pulse(std::initializer_list<uint8_t> motors, float pulse, bool load) {
    for(auto motor : motors) {
      this->pulse(motor, pulse, false);
    }
    if(load)
      pwms.load_pwm();
  }

  void MotorCluster::all_to_pulse(float pulse, bool load) {
    uint8_t motor_count = pwms.get_chan_count();
    for(uint8_t motor = 0; motor < motor_count; motor++) {
      this->pulse(motor, pulse, false);
    }
    if(load)
      pwms.load_pwm();
  }

  float MotorCluster::speed(uint8_t motor) const {
    assert(motor < pwms.get_chan_count());
    return states[motor].get_speed();
  }

  void MotorCluster::speed(uint8_t motor, float speed, bool load) {
    assert(motor < pwms.get_chan_count());
    float new_pulse = states[motor].set_speed_with_return(speed);
    apply_pulse(motor, new_pulse, load);
  }

  void MotorCluster::speed(const uint8_t *motors, uint8_t length, float speed, bool load) {
    assert(motors != nullptr);
    for(uint8_t i = 0; i < length; i++) {
      this->speed(motors[i], speed, false);
    }
    if(load)
      pwms.load_pwm();
  }

  void MotorCluster::speed(std::initializer_list<uint8_t> motors, float speed, bool load) {
    for(auto motor : motors) {
      this->speed(motor, speed, false);
    }
    if(load)
      pwms.load_pwm();
  }

  void MotorCluster::all_to_speed(float speed, bool load) {
    uint8_t motor_count = pwms.get_chan_count();
    for(uint8_t motor = 0; motor < motor_count; motor++) {
      this->speed(motor, speed, false);
    }
    if(load)
      pwms.load_pwm();
  }

  float MotorCluster::phase(uint8_t motor) const {
    assert(motor < pwms.get_chan_count());
    return motor_phases[motor];
  }

  void MotorCluster::phase(uint8_t motor, float phase, bool load) {
    assert(motor < pwms.get_chan_count());
    motor_phases[motor] = MIN(MAX(phase, 0.0f), 1.0f);
    pwms.set_chan_offset(motor, (uint32_t)(motor_phases[motor] * (float)pwms.get_wrap()), load);
  }

  void MotorCluster::phase(const uint8_t *motors, uint8_t length, float phase, bool load) {
    assert(motors != nullptr);
    for(uint8_t i = 0; i < length; i++) {
      this->phase(motors[i], phase, false);
    }
    if(load)
      pwms.load_pwm();
  }

  void MotorCluster::phase(std::initializer_list<uint8_t> motors, float phase, bool load) {
    for(auto motor : motors) {
      this->phase(motor, phase, false);
    }
    if(load)
      pwms.load_pwm();
  }

  void MotorCluster::all_to_phase(float phase, bool load) {
    uint8_t motor_count = pwms.get_chan_count();
    for(uint8_t motor = 0; motor < motor_count; motor++) {
      this->phase(motor, phase, false);
    }
    if(load)
      pwms.load_pwm();
  }

  float MotorCluster::frequency() const {
    return pwm_frequency;
  }

  bool MotorCluster::frequency(float freq) {
    bool success = false;

    if((freq >= MotorState::MIN_FREQUENCY) && (freq <= MotorState::MAX_FREQUENCY)) {
      // Calculate a suitable pwm wrap period for this frequency
      uint32_t period; uint32_t div256;
      if(pimoroni::PWMCluster::calculate_pwm_factors(freq, period, div256)) {

        pwm_period = period;
        pwm_frequency = freq;

        // Update the pwm before setting the new wrap
        uint8_t motor_count = pwms.get_chan_count();
        for(uint motor = 0; motor < motor_count; motor++) {
          if(states[motor].is_enabled()) {
            apply_pulse(motor, states[motor].get_pulse(), false);
          }
          pwms.set_chan_offset(motor, (uint32_t)(motor_phases[motor] * (float)pwm_period), false);
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

  float MotorCluster::min_speed(uint8_t motor) const {
    assert(is_assigned(motor));
    return states[motor].get_min_speed();
  }

  float MotorCluster::mid_speed(uint8_t motor) const {
    assert(is_assigned(motor));
    return states[motor].get_mid_speed();
  }

  float MotorCluster::max_speed(uint8_t motor) const {
    assert(is_assigned(motor));
    return states[motor].get_max_speed();
  }

  void MotorCluster::to_min(uint8_t motor, bool load) {
    assert(is_assigned(motor));
    float new_pulse = states[motor].to_min_with_return();
    apply_pulse(motor, new_pulse, load);
  }

  void MotorCluster::to_min(const uint8_t *motors, uint8_t length, bool load) {
    assert(motors != nullptr);
    for(uint8_t i = 0; i < length; i++) {
      to_min(motors[i], false);
    }
    if(load)
      pwms.load_pwm();
  }

  void MotorCluster::to_min(std::initializer_list<uint8_t> motors, bool load) {
    for(auto motor : motors) {
      to_min(motor, false);
    }
    if(load)
      pwms.load_pwm();
  }

  void MotorCluster::all_to_min(bool load) {
    uint8_t motor_count = pwms.get_chan_count();
    for(uint8_t motor = 0; motor < motor_count; motor++) {
      to_min(motor, false);
    }
    if(load)
      pwms.load_pwm();
  }

  void MotorCluster::to_mid(uint8_t motor, bool load) {
    assert(is_assigned(motor));
    float new_pulse = states[motor].to_mid_with_return();
    apply_pulse(motor, new_pulse, load);
  }

  void MotorCluster::to_mid(const uint8_t *motors, uint8_t length, bool load) {
    assert(motors != nullptr);
    for(uint8_t i = 0; i < length; i++) {
      to_mid(motors[i], false);
    }
    if(load)
      pwms.load_pwm();
  }

  void MotorCluster::to_mid(std::initializer_list<uint8_t> motors, bool load) {
    for(auto motor : motors) {
      to_mid(motor, false);
    }
    if(load)
      pwms.load_pwm();
  }

  void MotorCluster::all_to_mid(bool load) {
    uint8_t motor_count = pwms.get_chan_count();
    for(uint8_t motor = 0; motor < motor_count; motor++) {
      to_mid(motor, false);
    }
    if(load)
      pwms.load_pwm();
  }

  void MotorCluster::to_max(uint8_t motor, bool load) {
    assert(is_assigned(motor));
    float new_pulse = states[motor].to_max_with_return();
    apply_pulse(motor, new_pulse, load);
  }

  void MotorCluster::to_max(const uint8_t *motors, uint8_t length, bool load) {
    assert(motors != nullptr);
    for(uint8_t i = 0; i < length; i++) {
      to_max(motors[i], false);
    }
    if(load)
      pwms.load_pwm();
  }

  void MotorCluster::to_max(std::initializer_list<uint8_t> motors, bool load) {
    for(auto motor : motors) {
      to_max(motor, false);
    }
    if(load)
      pwms.load_pwm();
  }

  void MotorCluster::all_to_max(bool load) {
    uint8_t motor_count = pwms.get_chan_count();
    for(uint8_t motor = 0; motor < motor_count; motor++) {
      to_max(motor, false);
    }
    if(load)
      pwms.load_pwm();
  }

  void MotorCluster::to_percent(uint8_t motor, float in, float in_min, float in_max, bool load) {
    assert(is_assigned(motor));
    float new_pulse = states[motor].to_percent_with_return(in, in_min, in_max);
    apply_pulse(motor, new_pulse, load);
  }

  void MotorCluster::to_percent(const uint8_t *motors, uint8_t length, float in, float in_min, float in_max, bool load) {
    assert(motors != nullptr);
    for(uint8_t i = 0; i < length; i++) {
      to_percent(motors[i], in, in_min, in_max, false);
    }
    if(load)
      pwms.load_pwm();
  }

  void MotorCluster::to_percent(std::initializer_list<uint8_t> motors, float in, float in_min, float in_max, bool load) {
    for(auto motor : motors) {
      to_percent(motor, in, in_min, in_max, false);
    }
    if(load)
      pwms.load_pwm();
  }

  void MotorCluster::all_to_percent(float in, float in_min, float in_max, bool load) {
    uint8_t motor_count = pwms.get_chan_count();
    for(uint8_t motor = 0; motor < motor_count; motor++) {
      to_percent(motor, in, in_min, in_max, false);
    }
    if(load)
      pwms.load_pwm();
  }

  void MotorCluster::to_percent(uint8_t motor, float in, float in_min, float in_max, float speed_min, float speed_max, bool load) {
    assert(is_assigned(motor));
    float new_pulse = states[motor].to_percent_with_return(in, in_min, in_max, speed_min, speed_max);
    apply_pulse(motor, new_pulse, load);
  }

  void MotorCluster::to_percent(const uint8_t *motors, uint8_t length, float in, float in_min, float in_max, float speed_min, float speed_max, bool load) {
    assert(motors != nullptr);
    for(uint8_t i = 0; i < length; i++) {
      to_percent(motors[i], in, in_min, in_max, speed_min, speed_max, false);
    }
    if(load)
      pwms.load_pwm();
  }

  void MotorCluster::to_percent(std::initializer_list<uint8_t> motors, float in, float in_min, float in_max, float speed_min, float speed_max, bool load) {
    for(auto motor : motors) {
      to_percent(motor, in, in_min, in_max, speed_min, speed_max, false);
    }
    if(load)
      pwms.load_pwm();
  }

  void MotorCluster::all_to_percent(float in, float in_min, float in_max, float speed_min, float speed_max, bool load) {
    uint8_t motor_count = pwms.get_chan_count();
    for(uint8_t motor = 0; motor < motor_count; motor++) {
      to_percent(motor, in, in_min, in_max, speed_min, speed_max, false);
    }
    if(load)
      pwms.load_pwm();
  }

  Calibration& MotorCluster::calibration(uint8_t motor) {
    assert(is_assigned(motor));
    return states[motor].calibration();
  }

  const Calibration& MotorCluster::calibration(uint8_t motor) const {
    assert(is_assigned(motor));
    return states[motor].calibration();
  }

  void MotorCluster::load() {
    pwms.load_pwm();
  }

  void MotorCluster::apply_pulse(uint8_t motor, float pulse, bool load) {
    pwms.set_chan_level(motor, MotorState::pulse_to_level(pulse, pwm_period, pwm_frequency), load);
  }

  void MotorCluster::create_motor_states(CalibrationType default_type, bool auto_phase) {
    uint8_t motor_count = pwms.get_chan_count();
    if(motor_count > 0) {
      states = new MotorState[motor_count];
      motor_phases = new float[motor_count];

      for(uint motor = 0; motor < motor_count; motor++) {
        states[motor] = MotorState(default_type);
        motor_phases[motor] = (auto_phase) ? (float)motor / (float)motor_count : 0.0f;
      }
    }
  }

  void MotorCluster::create_motor_states(const Calibration& calibration, bool auto_phase) {
    uint8_t motor_count = pwms.get_chan_count();
    if(motor_count > 0) {
      states = new MotorState[motor_count];
      motor_phases = new float[motor_count];

      for(uint motor = 0; motor < motor_count; motor++) {
        states[motor] = MotorState(calibration);
        motor_phases[motor] = (auto_phase) ? (float)motor / (float)motor_count : 0.0f;
      }
    }
  }
};