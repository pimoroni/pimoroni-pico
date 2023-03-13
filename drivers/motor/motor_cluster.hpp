#pragma once

#include "pico/stdlib.h"
#include "pwm_cluster.hpp"
#include "motor_state.hpp"

using namespace pimoroni;

namespace motor {

  class MotorCluster {
    //--------------------------------------------------
    // Substructures
    //--------------------------------------------------
  private:
    struct motor_config {
      float phase;
      DecayMode mode;
    };


    //--------------------------------------------------
    // Variables
    //--------------------------------------------------
  public:
    static const uint MAX_MOTOR_CHANNELS = 16;

  private:
    PWMCluster pwms;
    uint32_t pwm_period;
    float pwm_frequency;
    MotorState states[MAX_MOTOR_CHANNELS];
    motor_config configs[MAX_MOTOR_CHANNELS];


    //--------------------------------------------------
    // Constructors/Destructor
    //--------------------------------------------------
  public:
    MotorCluster(PIO pio, uint sm, uint pin_base, uint pin_pair_count, Direction direction = NORMAL_DIR, float speed_scale = MotorState::DEFAULT_SPEED_SCALE,
                 float zeropoint = MotorState::DEFAULT_ZEROPOINT, float deadzone = MotorState::DEFAULT_DEADZONE, float freq = MotorState::DEFAULT_FREQUENCY, DecayMode mode = MotorState::DEFAULT_DECAY_MODE,
                 bool auto_phase = true);
    MotorCluster(PIO pio, uint sm, const pin_pair *pin_pairs, uint32_t length, Direction direction = NORMAL_DIR, float speed_scale = MotorState::DEFAULT_SPEED_SCALE,
                 float zeropoint = MotorState::DEFAULT_ZEROPOINT, float deadzone = MotorState::DEFAULT_DEADZONE, float freq = MotorState::DEFAULT_FREQUENCY, DecayMode mode = MotorState::DEFAULT_DECAY_MODE,
                 bool auto_phase = true);
    MotorCluster(PIO pio, uint sm, std::initializer_list<pin_pair> pin_pairs, Direction direction = NORMAL_DIR, float speed_scale = MotorState::DEFAULT_SPEED_SCALE,
                 float zeropoint = MotorState::DEFAULT_ZEROPOINT, float deadzone = MotorState::DEFAULT_DEADZONE, float freq = MotorState::DEFAULT_FREQUENCY, DecayMode mode = MotorState::DEFAULT_DECAY_MODE,
                 bool auto_phase = true);
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

    void brake(uint8_t motor, bool load = true);
    void brake(const uint8_t *motors, uint8_t length, bool load = true);
    void brake(std::initializer_list<uint8_t> motors, bool load = true);
    void brake_all(bool load = true);

    void full_negative(uint8_t motor, bool load = true);
    void full_negative(const uint8_t *motors, uint8_t length, bool load = true);
    void full_negative(std::initializer_list<uint8_t> motors, bool load = true);
    void all_full_negative(bool load = true);

    void full_positive(uint8_t motor, bool load = true);
    void full_positive(const uint8_t *motors, uint8_t length, bool load = true);
    void full_positive(std::initializer_list<uint8_t> motors, bool load = true);
    void all_full_positive(bool load = true);

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

    Direction direction(uint8_t motor) const;
    void direction(uint8_t motor, Direction direction);
    void direction(const uint8_t *motors, uint8_t length, Direction direction);
    void direction(std::initializer_list<uint8_t> motors, Direction direction);
    void all_directions(Direction direction);

    float speed_scale(uint8_t motor) const;
    void speed_scale(uint8_t motor, float speed_scale);
    void speed_scale(const uint8_t *motors, uint8_t length, float speed_scale);
    void speed_scale(std::initializer_list<uint8_t> motors, float speed_scale);
    void all_speed_scales(float speed_scale);

    float zeropoint(uint8_t motor) const;
    void zeropoint(uint8_t motor, float zeropoint, bool load = true);
    void zeropoint(const uint8_t *motors, uint8_t length, float zeropoint, bool load = true);
    void zeropoint(std::initializer_list<uint8_t> motors, float zeropoint, bool load = true);
    void all_zeropoints(float zeropoint, bool load = true);

    float deadzone(uint8_t motor) const;
    void deadzone(uint8_t motor, float deadzone, bool load = true);
    void deadzone(const uint8_t *motors, uint8_t length, float deadzone, bool load = true);
    void deadzone(std::initializer_list<uint8_t> motors, float deadzone, bool load = true);
    void all_deadzones(float deadzone, bool load = true);

    DecayMode decay_mode(uint8_t motor) const;
    void decay_mode(uint8_t motor, DecayMode mode, bool load = true);
    void decay_mode(const uint8_t *motors, uint8_t length, DecayMode mode, bool load = true);
    void decay_mode(std::initializer_list<uint8_t> motors, DecayMode mode, bool load = true);
    void all_decay_modes(DecayMode mode, bool load = true);

    //--------------------------------------------------
  private:
    void apply_duty(uint8_t motor, float duty, DecayMode mode, bool load);
    void create_motor_states(Direction direction, float speed_scale, float zeropoint,
                             float deadzone, DecayMode mode, bool auto_phase);
  };

}