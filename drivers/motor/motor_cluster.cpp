#include "motor_cluster.hpp"
#include "pwm.hpp"
#include <cstdio>
#include "math.h"

#define POS_MOTOR(motor)  (PWMCluster::channel_from_pair(motor))
#define NEG_MOTOR(motor)  (PWMCluster::channel_from_pair(motor) + 1)

namespace motor {
  MotorCluster::MotorCluster(PIO pio, uint sm, uint pin_base, uint pin_pair_count, Direction direction,
                             float speed_scale, float zeropoint, float deadzone, float freq, DecayMode mode,
                             bool auto_phase)
    : pwms(pio, sm, pin_base, (pin_pair_count * 2), false), pwm_frequency(freq) {
    create_motor_states(direction, speed_scale, zeropoint, deadzone, mode, auto_phase);
  }

  MotorCluster::MotorCluster(PIO pio, uint sm, const pin_pair *pin_pairs, uint32_t length, Direction direction,
                             float speed_scale, float zeropoint, float deadzone, float freq, DecayMode mode,
                             bool auto_phase)
    : pwms(pio, sm, pin_pairs, length, false), pwm_frequency(freq) {
    create_motor_states(direction, speed_scale, zeropoint, deadzone, mode, auto_phase);
  }

  MotorCluster::MotorCluster(PIO pio, uint sm, std::initializer_list<pin_pair> pin_pairs, Direction direction,
                             float speed_scale, float zeropoint, float deadzone, float freq, DecayMode mode,
                             bool auto_phase)
    : pwms(pio, sm, pin_pairs, false), pwm_frequency(freq) {
    create_motor_states(direction, speed_scale, zeropoint, deadzone, mode, auto_phase);
  }

  MotorCluster::~MotorCluster() {
  }

  bool MotorCluster::init() {
    bool success = false;

    if(pwms.init()) {
      // Calculate a suitable pwm wrap period for this frequency
      uint32_t period; uint32_t div256;
      if(pimoroni::PWMCluster::calculate_pwm_factors(pwm_frequency, period, div256)) {
        pwm_period = period;

        // Update the pwm before setting the new wrap
        uint8_t motor_count = pwms.get_chan_pair_count();
        for(uint8_t motor = 0; motor < motor_count; motor++) {
          pwms.set_chan_level(POS_MOTOR(motor), 0, false);
          pwms.set_chan_level(NEG_MOTOR(motor), 0, false);
          pwms.set_chan_offset(POS_MOTOR(motor), (uint32_t)(configs[motor].phase * (float)pwm_period), false);
          pwms.set_chan_offset(NEG_MOTOR(motor), (uint32_t)(configs[motor].phase * (float)pwm_period), false);
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
    return pwms.get_chan_pair_count();
  }

  pin_pair MotorCluster::pins(uint8_t motor) const {
    assert(motor < pwms.get_chan_pair_count());
    return pwms.get_chan_pin_pair(motor);
  }

  void MotorCluster::enable(uint8_t motor, bool load) {
    assert(motor < pwms.get_chan_pair_count());
    float new_duty = states[motor].enable_with_return();
    apply_duty(motor, new_duty, configs[motor].mode, load);
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
    uint8_t motor_count = pwms.get_chan_pair_count();
    for(uint8_t motor = 0; motor < motor_count; motor++) {
      enable(motor, false);
    }
    if(load)
      pwms.load_pwm();
  }

  void MotorCluster::disable(uint8_t motor, bool load) {
    assert(motor < pwms.get_chan_pair_count());
    float new_duty = states[motor].disable_with_return();
    apply_duty(motor, new_duty, configs[motor].mode, load);
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
    uint8_t motor_count = pwms.get_chan_pair_count();
    for(uint8_t motor = 0; motor < motor_count; motor++) {
      disable(motor, false);
    }
    if(load)
      pwms.load_pwm();
  }

  bool MotorCluster::is_enabled(uint8_t motor) const {
    assert(motor < pwms.get_chan_pair_count());
    return states[motor].is_enabled();
  }

  float MotorCluster::duty(uint8_t motor) const {
    assert(motor < pwms.get_chan_pair_count());
    return states[motor].get_duty();
  }

  void MotorCluster::duty(uint8_t motor, float duty, bool load) {
    assert(motor < pwms.get_chan_pair_count());
    float new_duty = states[motor].set_duty_with_return(duty);
    apply_duty(motor, new_duty, configs[motor].mode, load);
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
    uint8_t motor_count = pwms.get_chan_pair_count();
    for(uint8_t motor = 0; motor < motor_count; motor++) {
      this->duty(motor, duty, false);
    }
    if(load)
      pwms.load_pwm();
  }

  float MotorCluster::speed(uint8_t motor) const {
    assert(motor < pwms.get_chan_pair_count());
    return states[motor].get_speed();
  }

  void MotorCluster::speed(uint8_t motor, float speed, bool load) {
    assert(motor < pwms.get_chan_pair_count());
    float new_duty = states[motor].set_speed_with_return(speed);
    apply_duty(motor, new_duty, configs[motor].mode, load);
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
    uint8_t motor_count = pwms.get_chan_pair_count();
    for(uint8_t motor = 0; motor < motor_count; motor++) {
      this->speed(motor, speed, false);
    }
    if(load)
      pwms.load_pwm();
  }

  float MotorCluster::phase(uint8_t motor) const {
    assert(motor < pwms.get_chan_pair_count());
    return configs[motor].phase;
  }

  void MotorCluster::phase(uint8_t motor, float phase, bool load) {
    assert(motor < pwms.get_chan_pair_count());
    configs[motor].phase = MIN(MAX(phase, 0.0f), 1.0f);
    pwms.set_chan_offset(motor, (uint32_t)(configs[motor].phase * (float)pwms.get_wrap()), load);
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
    uint8_t motor_count = pwms.get_chan_pair_count();
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
        uint8_t motor_count = pwms.get_chan_pair_count();
        for(uint motor = 0; motor < motor_count; motor++) {
          apply_duty(motor, states[motor].get_deadzoned_duty(), configs[motor].mode, false);
          pwms.set_chan_offset(POS_MOTOR(motor), (uint32_t)(configs[motor].phase * (float)pwm_period), false);
          pwms.set_chan_offset(NEG_MOTOR(motor), (uint32_t)(configs[motor].phase * (float)pwm_period), false);
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

  void MotorCluster::stop(uint8_t motor, bool load) {
    assert(motor < pwms.get_chan_pair_count());
    float new_duty = states[motor].stop_with_return();
    apply_duty(motor, new_duty, configs[motor].mode, load);
  }

  void MotorCluster::stop(const uint8_t *motors, uint8_t length, bool load) {
    assert(motors != nullptr);
    for(uint8_t i = 0; i < length; i++) {
      this->stop(motors[i], false);
    }
    if(load)
      pwms.load_pwm();
  }

  void MotorCluster::stop(std::initializer_list<uint8_t> motors, bool load) {
    for(auto motor : motors) {
      this->stop(motor, false);
    }
    if(load)
      pwms.load_pwm();
  }

  void MotorCluster::stop_all(bool load) {
    uint8_t motor_count = pwms.get_chan_pair_count();
    for(uint8_t motor = 0; motor < motor_count; motor++) {
      this->stop(motor, false);
    }
    if(load)
      pwms.load_pwm();
  }

  void MotorCluster::coast(uint8_t motor, bool load) {
    assert(motor < pwms.get_chan_pair_count());
    float new_duty = states[motor].stop_with_return();
    apply_duty(motor, new_duty, FAST_DECAY, load);
  }

  void MotorCluster::coast(const uint8_t *motors, uint8_t length, bool load) {
    assert(motors != nullptr);
    for(uint8_t i = 0; i < length; i++) {
      this->coast(motors[i], false);
    }
    if(load)
      pwms.load_pwm();
  }

  void MotorCluster::coast(std::initializer_list<uint8_t> motors, bool load) {
    for(auto motor : motors) {
      this->coast(motor, false);
    }
    if(load)
      pwms.load_pwm();
  }

  void MotorCluster::coast_all(bool load) {
    uint8_t motor_count = pwms.get_chan_pair_count();
    for(uint8_t motor = 0; motor < motor_count; motor++) {
      this->coast(motor, false);
    }
    if(load)
      pwms.load_pwm();
  }

  void MotorCluster::brake(uint8_t motor, bool load) {
    assert(motor < pwms.get_chan_pair_count());
    float new_duty = states[motor].stop_with_return();
    apply_duty(motor, new_duty, SLOW_DECAY, load);
  }

  void MotorCluster::brake(const uint8_t *motors, uint8_t length, bool load) {
    assert(motors != nullptr);
    for(uint8_t i = 0; i < length; i++) {
      this->brake(motors[i], false);
    }
    if(load)
      pwms.load_pwm();
  }

  void MotorCluster::brake(std::initializer_list<uint8_t> motors, bool load) {
    for(auto motor : motors) {
      this->brake(motor, false);
    }
    if(load)
      pwms.load_pwm();
  }

  void MotorCluster::brake_all(bool load) {
    uint8_t motor_count = pwms.get_chan_pair_count();
    for(uint8_t motor = 0; motor < motor_count; motor++) {
      this->brake(motor, false);
    }
    if(load)
      pwms.load_pwm();
  }

  void MotorCluster::full_negative(uint8_t motor, bool load) {
    assert(motor < pwms.get_chan_pair_count());
    float new_duty = states[motor].full_negative_with_return();
    apply_duty(motor, new_duty, configs[motor].mode, load);
  }

  void MotorCluster::full_negative(const uint8_t *motors, uint8_t length, bool load) {
    assert(motors != nullptr);
    for(uint8_t i = 0; i < length; i++) {
      this->full_negative(motors[i], false);
    }
    if(load)
      pwms.load_pwm();
  }

  void MotorCluster::full_negative(std::initializer_list<uint8_t> motors, bool load) {
    for(auto motor : motors) {
      this->full_negative(motor, false);
    }
    if(load)
      pwms.load_pwm();
  }

  void MotorCluster::all_full_negative(bool load) {
    uint8_t motor_count = pwms.get_chan_pair_count();
    for(uint8_t motor = 0; motor < motor_count; motor++) {
      this->full_negative(motor, false);
    }
    if(load)
      pwms.load_pwm();
  }

  void MotorCluster::full_positive(uint8_t motor, bool load) {
    assert(motor < pwms.get_chan_pair_count());
    float new_duty = states[motor].full_positive_with_return();
    apply_duty(motor, new_duty, configs[motor].mode, load);
  }

  void MotorCluster::full_positive(const uint8_t *motors, uint8_t length, bool load) {
    assert(motors != nullptr);
    for(uint8_t i = 0; i < length; i++) {
      this->full_positive(motors[i], false);
    }
    if(load)
      pwms.load_pwm();
  }

  void MotorCluster::full_positive(std::initializer_list<uint8_t> motors, bool load) {
    for(auto motor : motors) {
      this->full_positive(motor, false);
    }
    if(load)
      pwms.load_pwm();
  }

  void MotorCluster::all_full_positive(bool load) {
    uint8_t motor_count = pwms.get_chan_pair_count();
    for(uint8_t motor = 0; motor < motor_count; motor++) {
      this->full_positive(motor, false);
    }
    if(load)
      pwms.load_pwm();
  }

  void MotorCluster::to_percent(uint8_t motor, float in, float in_min, float in_max, bool load) {
    assert(motor < pwms.get_chan_pair_count());
    float new_duty = states[motor].to_percent_with_return(in, in_min, in_max);
    apply_duty(motor, new_duty, configs[motor].mode, load);
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
    uint8_t motor_count = pwms.get_chan_pair_count();
    for(uint8_t motor = 0; motor < motor_count; motor++) {
      to_percent(motor, in, in_min, in_max, false);
    }
    if(load)
      pwms.load_pwm();
  }

  void MotorCluster::to_percent(uint8_t motor, float in, float in_min, float in_max, float speed_min, float speed_max, bool load) {
    assert(motor < pwms.get_chan_pair_count());
    float new_duty = states[motor].to_percent_with_return(in, in_min, in_max, speed_min, speed_max);
    apply_duty(motor, new_duty, configs[motor].mode, load);
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
    uint8_t motor_count = pwms.get_chan_pair_count();
    for(uint8_t motor = 0; motor < motor_count; motor++) {
      to_percent(motor, in, in_min, in_max, speed_min, speed_max, false);
    }
    if(load)
      pwms.load_pwm();
  }

  void MotorCluster::load() {
    pwms.load_pwm();
  }

  Direction MotorCluster::direction(uint8_t motor) const {
    assert(motor < pwms.get_chan_pair_count());
    return states[motor].get_direction();
  }

  void MotorCluster::direction(uint8_t motor, Direction direction) {
    assert(motor < pwms.get_chan_pair_count());
    states[motor].set_direction(direction);
  }

  void MotorCluster::direction(const uint8_t *motors, uint8_t length, Direction direction) {
    assert(motors != nullptr);
    for(uint8_t i = 0; i < length; i++) {
      this->direction(motors[i], direction);
    }
  }

  void MotorCluster::direction(std::initializer_list<uint8_t> motors, Direction direction) {
    for(auto motor : motors) {
      this->direction(motor, direction);
    }
  }

  void MotorCluster::all_directions(Direction direction) {
    uint8_t motor_count = pwms.get_chan_pair_count();
    for(uint8_t motor = 0; motor < motor_count; motor++) {
      this->direction(motor, direction);
    }
  }

  float MotorCluster::speed_scale(uint8_t motor) const {
    assert(motor < pwms.get_chan_pair_count());
    return states[motor].get_speed_scale();
  }

  void MotorCluster::speed_scale(uint8_t motor, float speed_scale) {
    assert(motor < pwms.get_chan_pair_count());
    states[motor].set_speed_scale(speed_scale);
  }

  void MotorCluster::speed_scale(const uint8_t *motors, uint8_t length, float speed_scale) {
    assert(motors != nullptr);
    for(uint8_t i = 0; i < length; i++) {
      this->speed_scale(motors[i], speed_scale);
    }
  }

  void MotorCluster::speed_scale(std::initializer_list<uint8_t> motors, float speed_scale) {
    for(auto motor : motors) {
      this->speed_scale(motor, speed_scale);
    }
  }

  void MotorCluster::all_speed_scales(float speed_scale) {
    uint8_t motor_count = pwms.get_chan_pair_count();
    for(uint8_t motor = 0; motor < motor_count; motor++) {
      this->speed_scale(motor, speed_scale);
    }
  }

  float MotorCluster::zeropoint(uint8_t motor) const {
    assert(motor < pwms.get_chan_pair_count());
    return states[motor].get_zeropoint();
  }

  void MotorCluster::zeropoint(uint8_t motor, float zeropoint, bool load) {
    assert(motor < pwms.get_chan_pair_count());
    states[motor].set_zeropoint(zeropoint);
  }

  void MotorCluster::zeropoint(const uint8_t *motors, uint8_t length, float zeropoint, bool load) {
    assert(motors != nullptr);
    for(uint8_t i = 0; i < length; i++) {
      this->zeropoint(motors[i], zeropoint);
    }
  }

  void MotorCluster::zeropoint(std::initializer_list<uint8_t> motors, float zeropoint, bool load) {
    for(auto motor : motors) {
      this->zeropoint(motor, zeropoint);
    }
  }

  void MotorCluster::all_zeropoints(float zeropoint, bool load) {
    uint8_t motor_count = pwms.get_chan_pair_count();
    for(uint8_t motor = 0; motor < motor_count; motor++) {
      this->zeropoint(motor, zeropoint);
    }
  }

  float MotorCluster::deadzone(uint8_t motor) const {
    assert(motor < pwms.get_chan_pair_count());
    return states[motor].get_deadzone();
  }

  void MotorCluster::deadzone(uint8_t motor, float deadzone, bool load) {
    assert(motor < pwms.get_chan_pair_count());
    float new_duty = states[motor].set_deadzone_with_return(deadzone);
    apply_duty(motor, new_duty, configs[motor].mode, load);
  }

  void MotorCluster::deadzone(const uint8_t *motors, uint8_t length, float deadzone, bool load) {
    assert(motors != nullptr);
    for(uint8_t i = 0; i < length; i++) {
      this->deadzone(motors[i], deadzone);
    }
  }

  void MotorCluster::deadzone(std::initializer_list<uint8_t> motors, float deadzone, bool load) {
    for(auto motor : motors) {
      this->deadzone(motor, deadzone);
    }
  }

  void MotorCluster::all_deadzones(float deadzone, bool load) {
    uint8_t motor_count = pwms.get_chan_pair_count();
    for(uint8_t motor = 0; motor < motor_count; motor++) {
      this->deadzone(motor, deadzone);
    }
  }

  DecayMode MotorCluster::decay_mode(uint8_t motor) const {
    assert(motor < pwms.get_chan_pair_count());
    return configs[motor].mode;
  }

  void MotorCluster::decay_mode(uint8_t motor, DecayMode mode, bool load) {
    assert(motor < pwms.get_chan_pair_count());
    configs[motor].mode = mode;
    apply_duty(motor, states[motor].get_deadzoned_duty(), configs[motor].mode, false);
  }

  void MotorCluster::decay_mode(const uint8_t *motors, uint8_t length, DecayMode mode, bool load) {
    assert(motors != nullptr);
    for(uint8_t i = 0; i < length; i++) {
      this->decay_mode(motors[i], mode);
    }
  }

  void MotorCluster::decay_mode(std::initializer_list<uint8_t> motors, DecayMode mode, bool load) {
    for(auto motor : motors) {
      this->decay_mode(motor, mode);
    }
  }

  void MotorCluster::all_decay_modes(DecayMode mode, bool load) {
    uint8_t motor_count = pwms.get_chan_pair_count();
    for(uint8_t motor = 0; motor < motor_count; motor++) {
      this->decay_mode(motor, mode);
    }
  }


  void MotorCluster::apply_duty(uint8_t motor, float duty, DecayMode mode, bool load) {
    if(isfinite(duty)) {
      int32_t signed_level = MotorState::duty_to_level(duty, pwm_period);

      switch(mode) {
      case SLOW_DECAY: //aka 'Braking'
        if(signed_level >= 0) {
          pwms.set_chan_level(POS_MOTOR(motor), pwm_period, false);
          pwms.set_chan_level(NEG_MOTOR(motor), pwm_period - signed_level, load);
        }
        else {
          pwms.set_chan_level(POS_MOTOR(motor), pwm_period + signed_level, false);
          pwms.set_chan_level(NEG_MOTOR(motor), pwm_period, load);
        }
        break;

      case FAST_DECAY: //aka 'Coasting'
      default:
        if(signed_level >= 0) {
          pwms.set_chan_level(POS_MOTOR(motor), signed_level, false);
          pwms.set_chan_level(NEG_MOTOR(motor), 0, load);
        }
        else {
          pwms.set_chan_level(POS_MOTOR(motor), 0, false);
          pwms.set_chan_level(NEG_MOTOR(motor), 0 - signed_level, load);
        }
        break;
      }
    }
    else {
      pwms.set_chan_level(POS_MOTOR(motor), 0, false);
      pwms.set_chan_level(NEG_MOTOR(motor), 0, load);
    }
  }

  void MotorCluster::create_motor_states(Direction direction, float speed_scale, float zeropoint,
                                         float deadzone, DecayMode mode, bool auto_phase) {
    uint8_t motor_count = pwms.get_chan_pair_count();
    if(motor_count > 0) {
      for(uint motor = 0; motor < motor_count; motor++) {
        configs[motor] = motor_config();
        states[motor] = MotorState(direction, speed_scale, zeropoint, deadzone);
        configs[motor].phase = (auto_phase) ? (float)motor / (float)motor_count : 0.0f;
        configs[motor].mode = mode;
      }
    }
  }
}
