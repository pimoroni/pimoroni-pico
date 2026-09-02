#include "brushless_cluster.hpp"
#include <new>

// A motor's channels sit consecutively: three signals, or three {signal, inverse} pairs
#define LEG_CHANNEL(motor, leg)  ((motor) * channels_per_motor + (leg) * (channels_per_motor / 3))
#define INV_CHANNEL(motor, leg)  (LEG_CHANNEL(motor, leg) + 1)

namespace brushless {

  // Flattens pin structures for the PWMCluster constructor, which copies them before returning.
  // Filled by a constructor argument expression and consumed inside the delegated constructor,
  // so no lifetime extends beyond construction
  static uint8_t flattened_pins[pimoroni::PWMCluster::MAX_PWM_CHANNELS];

  static uint32_t flatten_trios(const pin_trio *pin_trios, uint32_t length) {
    uint32_t pin_count = 0;
    for(uint32_t i = 0; (i < length) && (pin_count + 3u <= pimoroni::PWMCluster::MAX_PWM_CHANNELS); i++) {
      flattened_pins[pin_count++] = pin_trios[i].u;
      flattened_pins[pin_count++] = pin_trios[i].v;
      flattened_pins[pin_count++] = pin_trios[i].w;
    }
    return pin_count;
  }

  static uint32_t flatten_bridges(const bridge_trio *bridge_trios, uint32_t length) {
    uint32_t pin_count = 0;
    for(uint32_t i = 0; (i < length) && (pin_count + 6u <= pimoroni::PWMCluster::MAX_PWM_CHANNELS); i++) {
      const bridge_trio &trio = bridge_trios[i];
      const pin_pair legs[] = {trio.u, trio.v, trio.w};
      for(auto leg : legs) {
        flattened_pins[pin_count++] = leg.first;
        flattened_pins[pin_count++] = leg.second;
      }
    }
    return pin_count;
  }

  BrushlessCluster::BrushlessCluster(PIO pio, uint sm, const uint8_t *pins, uint32_t pin_count, uint8_t channels_per_motor,
                                     Direction direction, float freq, bool auto_phase)
    : pwms(pio, sm, pins, pin_count, false), pwm_frequency(freq), channels_per_motor(channels_per_motor) {
    create_motor_states(direction, auto_phase);
  }

  BrushlessCluster::BrushlessCluster(PIO pio, uint sm, const pin_trio *pin_trios, uint32_t length,
                                     Direction direction, float freq, bool auto_phase)
    : BrushlessCluster(pio, sm, flattened_pins, flatten_trios(pin_trios, length), 3, direction, freq, auto_phase) {
  }

  BrushlessCluster::BrushlessCluster(PIO pio, uint sm, std::initializer_list<pin_trio> pin_trios,
                                     Direction direction, float freq, bool auto_phase)
    : BrushlessCluster(pio, sm, pin_trios.begin(), pin_trios.size(), direction, freq, auto_phase) {
  }

  BrushlessCluster::BrushlessCluster(PIO pio, uint sm, uint pin_base, uint pin_trio_count,
                                     Direction direction, float freq, bool auto_phase)
    : pwms(pio, sm, pin_base, pin_trio_count * 3, false), pwm_frequency(freq), channels_per_motor(3) {
    create_motor_states(direction, auto_phase);
  }

  BrushlessCluster::BrushlessCluster(PIO pio, uint sm, const bridge_trio *bridge_trios, uint32_t length,
                                     Direction direction, float freq, bool auto_phase)
    : BrushlessCluster(pio, sm, flattened_pins, flatten_bridges(bridge_trios, length), 6, direction, freq, auto_phase) {
  }

  BrushlessCluster::BrushlessCluster(PIO pio, uint sm, std::initializer_list<bridge_trio> bridge_trios,
                                     Direction direction, float freq, bool auto_phase)
    : BrushlessCluster(pio, sm, bridge_trios.begin(), bridge_trios.size(), direction, freq, auto_phase) {
  }

  BrushlessCluster::~BrushlessCluster() {
    // BrushlessState is trivially destructible, so no destructor calls
    pwm_deallocate(states);
  }

  bool BrushlessCluster::init() {
    bool success = false;

    if(states == nullptr && pwms.get_chan_count() > 0) {
      return false;
    }

    if(pwms.init()) {
      // Calculate a suitable pwm wrap period for this frequency
      uint32_t period; uint32_t div256;
      if(pimoroni::PWMCluster::calculate_pwm_factors(pwm_frequency, period, div256)) {
        pwm_period = period;

        uint8_t motor_count = count();
        for(uint8_t motor = 0; motor < motor_count; motor++) {
          for(uint8_t leg = 0; leg < 3; leg++) {
            uint8_t channel = LEG_CHANNEL(motor, leg);
            pwms.set_chan_level(channel, 0, false);
            pwms.set_chan_alignment(channel, CENTRE_ALIGN, false);
            if(channels_per_motor == 6) {
              pwms.set_chan_level(INV_CHANNEL(motor, leg), 0, false);
              pwms.set_chan_alignment(INV_CHANNEL(motor, leg), CENTRE_ALIGN, false);
              pwms.set_chan_polarity(INV_CHANNEL(motor, leg), true, false);
            }
          }
          apply_phase(motor, false);
        }

        // Set the new wrap (should be 1 less than the period to get full 0 to 100%)
        pwms.set_wrap(pwm_period, true);

        // Apply the new divider
        // This is done after loading new PWM levels to avoid a lockup condition
        uint16_t div = div256 >> 8;
        uint8_t mod = div256 % 256;
        pwms.set_clkdiv_int_frac(div, mod);

        success = true;
      }
    }

    return success;
  }

  uint8_t BrushlessCluster::count() const {
    return pwms.get_chan_count() / channels_per_motor;
  }

  pin_trio BrushlessCluster::pins(uint8_t motor) const {
    assert(motor < count());
    return pin_trio(pwms.get_chan_pin(LEG_CHANNEL(motor, 0)),
                    pwms.get_chan_pin(LEG_CHANNEL(motor, 1)),
                    pwms.get_chan_pin(LEG_CHANNEL(motor, 2)));
  }

  bool BrushlessCluster::has_inverses() const {
    return channels_per_motor == 6;
  }

  void BrushlessCluster::enable(uint8_t motor, bool load) {
    assert(motor < count());
    states[motor].enable();
    apply_duties(motor, load);
  }

  void BrushlessCluster::enable(const uint8_t *motors, uint8_t length, bool load) {
    assert(motors != nullptr);
    for(uint8_t i = 0; i < length; i++) {
      enable(motors[i], false);
    }
    if(load)
      pwms.load_pwm();
  }

  void BrushlessCluster::enable(std::initializer_list<uint8_t> motors, bool load) {
    for(auto motor : motors) {
      enable(motor, false);
    }
    if(load)
      pwms.load_pwm();
  }

  void BrushlessCluster::enable_all(bool load) {
    uint8_t motor_count = count();
    for(uint8_t motor = 0; motor < motor_count; motor++) {
      enable(motor, false);
    }
    if(load)
      pwms.load_pwm();
  }

  void BrushlessCluster::disable(uint8_t motor, bool load) {
    assert(motor < count());
    states[motor].disable();
    apply_duties(motor, load);
  }

  void BrushlessCluster::disable(const uint8_t *motors, uint8_t length, bool load) {
    assert(motors != nullptr);
    for(uint8_t i = 0; i < length; i++) {
      disable(motors[i], false);
    }
    if(load)
      pwms.load_pwm();
  }

  void BrushlessCluster::disable(std::initializer_list<uint8_t> motors, bool load) {
    for(auto motor : motors) {
      disable(motor, false);
    }
    if(load)
      pwms.load_pwm();
  }

  void BrushlessCluster::disable_all(bool load) {
    uint8_t motor_count = count();
    for(uint8_t motor = 0; motor < motor_count; motor++) {
      disable(motor, false);
    }
    if(load)
      pwms.load_pwm();
  }

  bool BrushlessCluster::is_enabled(uint8_t motor) const {
    assert(motor < count());
    return states[motor].is_enabled();
  }

  float BrushlessCluster::u_duty(uint8_t motor) const {
    assert(motor < count());
    return states[motor].get_duty(0);
  }

  float BrushlessCluster::v_duty(uint8_t motor) const {
    assert(motor < count());
    return states[motor].get_duty(1);
  }

  float BrushlessCluster::w_duty(uint8_t motor) const {
    assert(motor < count());
    return states[motor].get_duty(2);
  }

  void BrushlessCluster::duties(uint8_t motor, float u, float v, float w, bool load) {
    assert(motor < count());
    states[motor].set_duties(u, v, w);
    apply_duties(motor, load);
  }

  void BrushlessCluster::duties(const uint8_t *motors, uint8_t length, float u, float v, float w, bool load) {
    assert(motors != nullptr);
    for(uint8_t i = 0; i < length; i++) {
      this->duties(motors[i], u, v, w, false);
    }
    if(load)
      pwms.load_pwm();
  }

  void BrushlessCluster::duties(std::initializer_list<uint8_t> motors, float u, float v, float w, bool load) {
    for(auto motor : motors) {
      this->duties(motor, u, v, w, false);
    }
    if(load)
      pwms.load_pwm();
  }

  void BrushlessCluster::all_to_duties(float u, float v, float w, bool load) {
    uint8_t motor_count = count();
    for(uint8_t motor = 0; motor < motor_count; motor++) {
      this->duties(motor, u, v, w, false);
    }
    if(load)
      pwms.load_pwm();
  }

  float BrushlessCluster::phase(uint8_t motor) const {
    assert(motor < count());
    return motor_phases[motor];
  }

  void BrushlessCluster::phase(uint8_t motor, float phase, bool load) {
    assert(motor < count());
    motor_phases[motor] = MIN(MAX(phase, 0.0f), 1.0f);
    apply_phase(motor, load);
  }

  void BrushlessCluster::phase(const uint8_t *motors, uint8_t length, float phase, bool load) {
    assert(motors != nullptr);
    for(uint8_t i = 0; i < length; i++) {
      this->phase(motors[i], phase, false);
    }
    if(load)
      pwms.load_pwm();
  }

  void BrushlessCluster::phase(std::initializer_list<uint8_t> motors, float phase, bool load) {
    for(auto motor : motors) {
      this->phase(motor, phase, false);
    }
    if(load)
      pwms.load_pwm();
  }

  void BrushlessCluster::all_to_phase(float phase, bool load) {
    uint8_t motor_count = count();
    for(uint8_t motor = 0; motor < motor_count; motor++) {
      this->phase(motor, phase, false);
    }
    if(load)
      pwms.load_pwm();
  }

  float BrushlessCluster::frequency() const {
    return pwm_frequency;
  }

  bool BrushlessCluster::frequency(float freq) {
    bool success = false;

    if((freq >= BrushlessState::MIN_FREQUENCY) && (freq <= MAX_FREQUENCY)) {
      // Calculate a suitable pwm wrap period for this frequency
      uint32_t period; uint32_t div256;
      if(pimoroni::PWMCluster::calculate_pwm_factors(freq, period, div256)) {
        pwm_period = period;
        pwm_frequency = freq;

        // Update the pwm before setting the new wrap
        uint8_t motor_count = count();
        for(uint8_t motor = 0; motor < motor_count; motor++) {
          apply_duties(motor, false);
          apply_phase(motor, false);
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

  Direction BrushlessCluster::direction(uint8_t motor) const {
    assert(motor < count());
    return states[motor].get_direction();
  }

  void BrushlessCluster::direction(uint8_t motor, Direction direction) {
    assert(motor < count());
    states[motor].set_direction(direction);
    apply_duties(motor, true);
  }

  void BrushlessCluster::load() {
    pwms.load_pwm();
  }

  void BrushlessCluster::apply_duties(uint8_t motor, bool load) {
    for(uint8_t leg = 0; leg < 3; leg++) {
      uint32_t level = BrushlessState::duty_to_level(states[motor].applied_duty(leg), pwm_period);
      pwms.set_chan_level(LEG_CHANNEL(motor, leg), level, false);
      if(channels_per_motor == 6) {
        pwms.set_chan_level(INV_CHANNEL(motor, leg), level, false);
      }
    }
    if(load)
      pwms.load_pwm();
  }

  void BrushlessCluster::apply_phase(uint8_t motor, bool load) {
    // The three legs share one offset, keeping their centred pulses concentric
    uint32_t offset = (uint32_t)(motor_phases[motor] * (float)pwm_period);
    for(uint8_t leg = 0; leg < 3; leg++) {
      pwms.set_chan_offset(LEG_CHANNEL(motor, leg), offset, false);
      if(channels_per_motor == 6) {
        pwms.set_chan_offset(INV_CHANNEL(motor, leg), offset, false);
      }
    }
    if(load)
      pwms.load_pwm();
  }

  void BrushlessCluster::create_motor_states(Direction direction, bool auto_phase) {
    uint8_t motor_count = count();
    if(motor_count > 0) {
      states = (BrushlessState*)pwm_allocate(((size_t)sizeof(BrushlessState) + sizeof(float)) * motor_count);
      if(states == nullptr) {
        return;  // init() reports the failure
      }
      motor_phases = (float*)(states + motor_count);
      for(uint8_t motor = 0; motor < motor_count; motor++) {
        new(&states[motor]) BrushlessState(direction);
        motor_phases[motor] = (auto_phase) ? (float)motor / (float)motor_count : 0.0f;
      }
    }
  }
}
