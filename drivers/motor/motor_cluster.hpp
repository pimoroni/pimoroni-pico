#pragma once

#include "pico/stdlib.h"
#include "pwm_cluster.hpp"
#include "motor_state.hpp"

using namespace pimoroni;

namespace motor {

  class MotorCluster {
    //--------------------------------------------------
    // Variables
    //--------------------------------------------------
  private:
    PWMCluster pwms;
    uint32_t pwm_period;
    float pwm_frequency;
    MotorState* states;
    float* motor_phases;


    //--------------------------------------------------
    // Constructors/Destructor
    //--------------------------------------------------
  public:
    MotorCluster(PIO pio, uint sm, uint pin_base, uint pin_pair_count, MotorState::Direction direction = MotorState::DEFAULT_DIRECTION, float speed_scale = MotorState::DEFAULT_SPEED_SCALE,
                 float deadzone_percent = MotorState::DEFAULT_DEADZONE, float freq = MotorState::DEFAULT_FREQUENCY, MotorState::DecayMode mode = MotorState::DEFAULT_DECAY_MODE,
                 bool auto_phase = true, PWMCluster::Sequence *seq_buffer = nullptr, PWMCluster::TransitionData *dat_buffer = nullptr);
    MotorCluster(PIO pio, uint sm, const pin_pair *pin_pairs, uint32_t length, MotorState::Direction direction = MotorState::DEFAULT_DIRECTION, float speed_scale = MotorState::DEFAULT_SPEED_SCALE,
                 float deadzone_percent = MotorState::DEFAULT_DEADZONE, float freq = MotorState::DEFAULT_FREQUENCY, MotorState::DecayMode mode = MotorState::DEFAULT_DECAY_MODE,
                 bool auto_phase = true, PWMCluster::Sequence *seq_buffer = nullptr, PWMCluster::TransitionData *dat_buffer = nullptr);
    MotorCluster(PIO pio, uint sm, std::initializer_list<pin_pair> pin_pairs, MotorState::Direction direction = MotorState::DEFAULT_DIRECTION, float speed_scale = MotorState::DEFAULT_SPEED_SCALE,
                 float deadzone_percent = MotorState::DEFAULT_DEADZONE, float freq = MotorState::DEFAULT_FREQUENCY, MotorState::DecayMode mode = MotorState::DEFAULT_DECAY_MODE,
                 bool auto_phase = true, PWMCluster::Sequence *seq_buffer = nullptr, PWMCluster::TransitionData *dat_buffer = nullptr);
    ~MotorCluster();


    //--------------------------------------------------
    // Methods
    //--------------------------------------------------
  public:
    bool init();

    uint8_t count() const;
    pin_pair pins(uint8_t motor) const;

    void enable(uint8_t motor, bool load = true);
    void enable(const uint8_t *motors, uint8_t length, bool load = true);
    void enable(std::initializer_list<uint8_t> motors, bool load = true);
    void enable_all(bool load = true);

    void disable(uint8_t motor, bool load = true);
    void disable(const uint8_t *motors, uint8_t length, bool load = true);
    void disable(std::initializer_list<uint8_t> motors, bool load = true);
    void disable_all(bool load = true);

    bool is_enabled(uint8_t motor) const;

    float duty(uint8_t motor) const;
    void duty(uint8_t motor, float duty, bool load = true);
    void duty(const uint8_t *motors, uint8_t length, float duty, bool load = true);
    void duty(std::initializer_list<uint8_t> motors, float duty, bool load = true);
    void all_to_duty(float duty, bool load = true);

    float speed(uint8_t motor) const;
    void speed(uint8_t motor, float speed, bool load = true);
    void speed(const uint8_t *motors, uint8_t length, float speed, bool load = true);
    void speed(std::initializer_list<uint8_t> motors, float speed, bool load = true);
    void all_to_speed(float speed, bool load = true);

    float phase(uint8_t motor) const;
    void phase(uint8_t motor, float phase, bool load = true);
    void phase(const uint8_t *motors, uint8_t length, float phase, bool load = true);
    void phase(std::initializer_list<uint8_t> motors, float phase, bool load = true);
    void all_to_phase(float phase, bool load = true);

    float frequency() const;
    bool frequency(float freq);

    //--------------------------------------------------
    void stop(uint8_t motor, bool load = true);
    void stop(const uint8_t *motors, uint8_t length, bool load = true);
    void stop(std::initializer_list<uint8_t> motors, bool load = true);
    void stop_all(bool load = true);

    void coast(uint8_t motor, bool load = true);
    void coast(const uint8_t *motors, uint8_t length, bool load = true);
    void coast(std::initializer_list<uint8_t> motors, bool load = true);
    void coast_all(bool load = true);

    void full_negative(uint8_t motor, bool load = true);
    void full_negative(const uint8_t *motors, uint8_t length, bool load = true);
    void full_negative(std::initializer_list<uint8_t> motors, bool load = true);
    void all_to_full_negative(bool load = true);

    void full_positive(uint8_t motor, bool load = true);
    void full_positive(const uint8_t *motors, uint8_t length, bool load = true);
    void full_positive(std::initializer_list<uint8_t> motors, bool load = true);
    void all_to_full_positive(bool load = true);

    void to_percent(uint8_t motor, float in, float in_min = MotorState::ZERO_PERCENT, float in_max = MotorState::ONEHUNDRED_PERCENT, bool load = true);
    void to_percent(const uint8_t *motors, uint8_t length, float in, float in_min = MotorState::ZERO_PERCENT, float in_max = MotorState::ONEHUNDRED_PERCENT, bool load = true);
    void to_percent(std::initializer_list<uint8_t> motors, float in, float in_min = MotorState::ZERO_PERCENT, float in_max = MotorState::ONEHUNDRED_PERCENT, bool load = true);
    void all_to_percent(float in, float in_min = MotorState::ZERO_PERCENT, float in_max = MotorState::ONEHUNDRED_PERCENT, bool load = true);

    void to_percent(uint8_t motor, float in, float in_min, float in_max, float speed_min, float speed_max, bool load = true);
    void to_percent(const uint8_t *motors, uint8_t length, float in, float in_min, float in_max, float speed_min, float speed_max, bool load = true);
    void to_percent(std::initializer_list<uint8_t> motors, float in, float in_min, float in_max, float speed_min, float speed_max, bool load = true);
    void all_to_percent(float in, float in_min, float in_max, float speed_min, float speed_max, bool load = true);

    void load();

    //--------------------------------------------------

    MotorState::Direction direction(uint8_t motor) const;
    void direction(uint8_t motor, MotorState::Direction direction);
    void direction(const uint8_t *motors, uint8_t length, MotorState::Direction direction);
    void direction(std::initializer_list<uint8_t> motors, MotorState::Direction direction);
    void all_directions(MotorState::Direction direction);

    float speed_scale(uint8_t motor) const;
    void speed_scale(uint8_t motor, float speed_scale);
    void speed_scale(const uint8_t *motors, uint8_t length, float speed_scale);
    void speed_scale(std::initializer_list<uint8_t> motors, float speed_scale);
    void all_speed_scales(float speed_scale);

    float deadzone_percent(uint8_t motor) const;
    void deadzone_percent(uint8_t motor, float deadzone_percent);
    void deadzone_percent(const uint8_t *motors, uint8_t length, float deadzone_percent);
    void deadzone_percent(std::initializer_list<uint8_t> motors, float deadzone_percent);
    void all_deadzone_percents(float deadzone_percent);

    MotorState::DecayMode decay_mode(uint8_t motor) const;
    void decay_mode(uint8_t motor, MotorState::DecayMode mode);
    void decay_mode(const uint8_t *motors, uint8_t length, MotorState::DecayMode mode);
    void decay_mode(std::initializer_list<uint8_t> motors, MotorState::DecayMode mode);
    void all_decay_modes(MotorState::DecayMode mode);

    //--------------------------------------------------
  private:
    void apply_duty(uint8_t motor, float duty, bool load);
    void create_motor_states(MotorState::Direction direction, float speed_scale,
                             float deadzone_percent, MotorState::DecayMode mode, bool auto_phase);

    static uint8_t motor_positive(uint8_t motor) {
      return PWMCluster::channel_from_pair(motor);
    }
    static uint8_t motor_negative(uint8_t motor) {
      return PWMCluster::channel_from_pair(motor) + 1;
    }
  };

}