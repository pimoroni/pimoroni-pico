#include "servo_cluster.hpp"
#include "pwm.hpp"
#include <cstdio>

namespace servo {
  ServoCluster::ServoCluster(PIO pio, uint sm, uint pin_mask, CalibrationType default_type, float freq, bool auto_phase)
  : pwms(pio, sm, pin_mask), pwm_frequency(freq) {
    create_servo_states(default_type, auto_phase);
  }

  ServoCluster::ServoCluster(PIO pio, uint sm, uint pin_base, uint pin_count, CalibrationType default_type, float freq, bool auto_phase)
  : pwms(pio, sm, pin_base, pin_count), pwm_frequency(freq) {
    create_servo_states(default_type, auto_phase);
  }

  ServoCluster::ServoCluster(PIO pio, uint sm, std::initializer_list<uint8_t> pins, CalibrationType default_type, float freq, bool auto_phase)
  : pwms(pio, sm, pins), pwm_frequency(freq) {
    create_servo_states(default_type, auto_phase);
  }

  ServoCluster::~ServoCluster() {
    delete[] servos;
    delete[] servo_phases;
  }

  bool ServoCluster::init() {
    bool success = false;

    if(pwms.init()) {
      // Calculate a suitable pwm wrap period for this frequency
      uint32_t period; uint16_t div16;
      if(pimoroni::PWMCluster::calculate_pwm_factors(pwm_frequency, period, div16)) {
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
        uint8_t div = div16 >> 4;
        uint8_t mod = div16 % 16;
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
    float new_pulse = servos[servo].enable();
    apply_pulse(servo, new_pulse, load);
  }

  void ServoCluster::disable(uint servo, bool load) {
    assert(servo < pwms.get_chan_count());
    float new_pulse = servos[servo].disable();
    apply_pulse(servo, new_pulse, load);
  }

  bool ServoCluster::is_enabled(uint servo) const {
    assert(servo < pwms.get_chan_count());
    return servos[servo].is_enabled();
  }

  float ServoCluster::get_value(uint servo) const {
    assert(servo < pwms.get_chan_count());
    return servos[servo].get_value();
  }

  void ServoCluster::set_value(uint servo, float value, bool load) {
    assert(servo < pwms.get_chan_count());
    float new_pulse = servos[servo].set_value(value);
    apply_pulse(servo, new_pulse, load);
  }

  float ServoCluster::get_pulse(uint servo) const {
    assert(servo < pwms.get_chan_count());
    return servos[servo].get_pulse();
  }

  void ServoCluster::set_pulse(uint servo, float pulse, bool load) {
    assert(servo < pwms.get_chan_count());
    float new_pulse = servos[servo].set_pulse(pulse);
    apply_pulse(servo, new_pulse, load);
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
        uint8_t servo_count = pwms.get_chan_count();
        for(uint servo = 0; servo < servo_count; servo++) {
          float current_pulse = servos[servo].get_pulse();
          apply_pulse(servo, current_pulse, false);
          pwms.set_chan_offset(servo, (uint32_t)(servo_phases[servo] * (float)pwm_period), false);
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
    assert(is_assigned(servo));
    return servos[servo].get_min_value();
  }

  float ServoCluster::get_mid_value(uint servo) const {
    assert(is_assigned(servo));
    return servos[servo].get_mid_value();
  }

  float ServoCluster::get_max_value(uint servo) const {
    assert(is_assigned(servo));
    return servos[servo].get_max_value();
  }

  void ServoCluster::to_min(uint servo, bool load) {
    assert(is_assigned(servo));
    float new_pulse = servos[servo].to_min();
    apply_pulse(servo, new_pulse, load);
  }

  void ServoCluster::to_mid(uint servo, bool load) {
    assert(is_assigned(servo));
    float new_pulse = servos[servo].to_mid();
    apply_pulse(servo, new_pulse, load);
  }

  void ServoCluster::to_max(uint servo, bool load) {
    assert(is_assigned(servo));
    float new_pulse = servos[servo].to_max();
    apply_pulse(servo, new_pulse, load);
  }

  void ServoCluster::to_percent(uint servo, float in, float in_min, float in_max, bool load) {
    assert(is_assigned(servo));
    float new_pulse = servos[servo].to_percent(in, in_min, in_max);
    apply_pulse(servo, new_pulse, load);
  }

  void ServoCluster::to_percent(uint servo, float in, float in_min, float in_max, float value_min, float value_max, bool load) {
    assert(is_assigned(servo));
    float new_pulse = servos[servo].to_percent(in, in_min, in_max, value_min, value_max);
    apply_pulse(servo, new_pulse, load);
  }

  Calibration* ServoCluster::calibration(uint servo) {
    assert(is_assigned(servo));
    return &servos[servo].calibration();
  }

  const Calibration* ServoCluster::calibration(uint servo) const {
    assert(is_assigned(servo));
    return &servos[servo].calibration();
  }

  void ServoCluster::apply_pulse(uint servo, float pulse, bool load) {
    pwms.set_chan_level(servo, ServoState::pulse_to_level(pulse, pwm_period, pwm_frequency), load);
  }

  void ServoCluster::create_servo_states(CalibrationType default_type, bool auto_phase) {
    uint8_t servo_count = pwms.get_chan_count();
    if(servo_count > 0) {
      servos = new ServoState[servo_count];
      servo_phases = new float[servo_count];

      for(uint servo = 0; servo < servo_count; servo++) {
        servos[servo] = ServoState(default_type);
        servo_phases[servo] = (auto_phase) ? (float)servo / (float)servo_count : 0.0f;
      }
    }
  }
};