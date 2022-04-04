#include "motor_cluster.hpp"
#include "pwm.hpp"
#include <cstdio>

namespace motor {
  MotorCluster::MotorCluster(PIO pio, uint sm, uint pin_mask, float speed_scale, bool inverted, float freq, bool auto_phase, PWMCluster::Sequence *seq_buffer, PWMCluster::TransitionData *dat_buffer)
    : pwms(pio, sm, pin_mask, seq_buffer, dat_buffer), pwm_frequency(freq) {
    create_motor_states(speed_scale, inverted, auto_phase);
  }

  MotorCluster::MotorCluster(PIO pio, uint sm, uint pin_base, uint pin_count, float speed_scale, bool inverted, float freq, bool auto_phase, PWMCluster::Sequence *seq_buffer, PWMCluster::TransitionData *dat_buffer)
    : pwms(pio, sm, pin_base, pin_count, seq_buffer, dat_buffer), pwm_frequency(freq) {
    create_motor_states(speed_scale, inverted, auto_phase);
  }

  MotorCluster::MotorCluster(PIO pio, uint sm, const uint8_t *pins, uint32_t length, float speed_scale, bool inverted, float freq, bool auto_phase, PWMCluster::Sequence *seq_buffer, PWMCluster::TransitionData *dat_buffer)
    : pwms(pio, sm, pins, length, seq_buffer, dat_buffer), pwm_frequency(freq) {
    create_motor_states(speed_scale, inverted, auto_phase);
  }

  MotorCluster::MotorCluster(PIO pio, uint sm, std::initializer_list<uint8_t> pins, float speed_scale, bool inverted, float freq, bool auto_phase, PWMCluster::Sequence *seq_buffer, PWMCluster::TransitionData *dat_buffer)
    : pwms(pio, sm, pins, seq_buffer, dat_buffer), pwm_frequency(freq) {
    create_motor_states(speed_scale, inverted, auto_phase);
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
    float new_duty = states[motor].enable_with_return();
    apply_duty(motor, new_duty, load);
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
    float new_duty = states[motor].disable_with_return();
    apply_duty(motor, new_duty, load);
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

  float MotorCluster::duty(uint8_t motor) const {
    assert(motor < pwms.get_chan_count());
    return states[motor].get_duty();
  }

  void MotorCluster::duty(uint8_t motor, float duty, bool load) {
    assert(motor < pwms.get_chan_count());
    float new_duty = states[motor].set_duty_with_return(duty);
    apply_duty(motor, new_duty, load);
  }

  void MotorCluster::duty(const uint8_t *motors, uint8_t length, float duty, bool load) {
    assert(motors != nullptr);
    for(uint8_t i = 0; i < length; i++) {
      this->duty(motors[i], duty, false);
    }
    if(load)
      pwms.load_pwm();
  }

  void MotorCluster::duty(std::initializer_list<uint8_t> motors, float duty, bool load) {
    for(auto motor : motors) {
      this->duty(motor, duty, false);
    }
    if(load)
      pwms.load_pwm();
  }

  void MotorCluster::all_to_duty(float duty, bool load) {
    uint8_t motor_count = pwms.get_chan_count();
    for(uint8_t motor = 0; motor < motor_count; motor++) {
      this->duty(motor, duty, false);
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
    float new_duty = states[motor].set_speed_with_return(speed);
    apply_duty(motor, new_duty, load);
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
            apply_duty(motor, states[motor].get_duty(), false);
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

  float MotorCluster::speed_scale(uint8_t motor) const {
    assert(is_assigned(motor));
    return states[motor].get_speed_scale();
  }

  void MotorCluster::to_full_negative(uint8_t motor, bool load) {
    assert(is_assigned(motor));
    float new_duty = states[motor].to_full_negative_with_return();
    apply_duty(motor, new_duty, load);
  }

  void MotorCluster::to_full_negative(const uint8_t *motors, uint8_t length, bool load) {
    assert(motors != nullptr);
    for(uint8_t i = 0; i < length; i++) {
      to_full_negative(motors[i], false);
    }
    if(load)
      pwms.load_pwm();
  }

  void MotorCluster::to_full_negative(std::initializer_list<uint8_t> motors, bool load) {
    for(auto motor : motors) {
      to_full_negative(motor, false);
    }
    if(load)
      pwms.load_pwm();
  }

  void MotorCluster::all_to_full_negative(bool load) {
    uint8_t motor_count = pwms.get_chan_count();
    for(uint8_t motor = 0; motor < motor_count; motor++) {
      to_full_negative(motor, false);
    }
    if(load)
      pwms.load_pwm();
  }

  void MotorCluster::to_full_positive(uint8_t motor, bool load) {
    assert(is_assigned(motor));
    float new_duty = states[motor].to_full_positive_with_return();
    apply_duty(motor, new_duty, load);
  }

  void MotorCluster::to_full_positive(const uint8_t *motors, uint8_t length, bool load) {
    assert(motors != nullptr);
    for(uint8_t i = 0; i < length; i++) {
      to_full_positive(motors[i], false);
    }
    if(load)
      pwms.load_pwm();
  }

  void MotorCluster::to_full_positive(std::initializer_list<uint8_t> motors, bool load) {
    for(auto motor : motors) {
      to_full_positive(motor, false);
    }
    if(load)
      pwms.load_pwm();
  }

  void MotorCluster::all_to_full_positive(bool load) {
    uint8_t motor_count = pwms.get_chan_count();
    for(uint8_t motor = 0; motor < motor_count; motor++) {
      to_full_positive(motor, false);
    }
    if(load)
      pwms.load_pwm();
  }

  void MotorCluster::to_percent(uint8_t motor, float in, float in_min, float in_max, bool load) {
    assert(is_assigned(motor));
    float new_duty = states[motor].to_percent_with_return(in, in_min, in_max);
    apply_duty(motor, new_duty, load);
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
    float new_duty = states[motor].to_percent_with_return(in, in_min, in_max, speed_min, speed_max);
    apply_duty(motor, new_duty, load);
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

  void MotorCluster::load() {
    pwms.load_pwm();
  }

  void MotorCluster::apply_duty(uint8_t motor, float duty, bool load) {
    pwms.set_chan_level(motor, MotorState::duty_to_level(duty, pwm_period), load);
  }

  void MotorCluster::create_motor_states(float speed_scale, bool inverted, bool auto_phase) {
    uint8_t motor_count = pwms.get_chan_count();
    if(motor_count > 0) {
      states = new MotorState[motor_count];
      motor_phases = new float[motor_count];

      for(uint motor = 0; motor < motor_count; motor++) {
        states[motor] = MotorState(speed_scale, inverted);
        motor_phases[motor] = (auto_phase) ? (float)motor / (float)motor_count : 0.0f;
      }
    }
  }
};