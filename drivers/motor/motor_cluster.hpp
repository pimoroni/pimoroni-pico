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
    MotorCluster(PIO pio, uint sm, uint pin_mask, float speed_scale = MotorState::DEFAULT_SPEED_SCALE, bool inverted = false, float freq = MotorState::DEFAULT_FREQUENCY, bool auto_phase = true, PWMCluster::Sequence *seq_buffer = nullptr, PWMCluster::TransitionData *dat_buffer = nullptr);
    MotorCluster(PIO pio, uint sm, uint pin_base, uint pin_count, float speed_scale = MotorState::DEFAULT_SPEED_SCALE, bool inverted = false, float freq = MotorState::DEFAULT_FREQUENCY, bool auto_phase = true, PWMCluster::Sequence *seq_buffer = nullptr, PWMCluster::TransitionData *dat_buffer = nullptr);
    MotorCluster(PIO pio, uint sm, const uint8_t *pins, uint32_t length, float speed_scale = MotorState::DEFAULT_SPEED_SCALE, bool inverted = false, float freq = MotorState::DEFAULT_FREQUENCY, bool auto_phase = true, PWMCluster::Sequence *seq_buffer = nullptr, PWMCluster::TransitionData *dat_buffer = nullptr);
    MotorCluster(PIO pio, uint sm, std::initializer_list<uint8_t> pins, float speed_scale = MotorState::DEFAULT_SPEED_SCALE, bool inverted = false, float freq = MotorState::DEFAULT_FREQUENCY, bool auto_phase = true, PWMCluster::Sequence *seq_buffer = nullptr, PWMCluster::TransitionData *dat_buffer = nullptr);
    ~MotorCluster();


    //--------------------------------------------------
    // Methods
    //--------------------------------------------------
  public:
    bool init();

    uint8_t count() const;
    uint8_t pin(uint8_t motor) const;

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
    float speed_scale(uint8_t motor) const;

    void to_full_negative(uint8_t motor, bool load = true);
    void to_full_negative(const uint8_t *motors, uint8_t length, bool load = true);
    void to_full_negative(std::initializer_list<uint8_t> motors, bool load = true);
    void all_to_full_negative(bool load = true);

    void to_full_positive(uint8_t motor, bool load = true);
    void to_full_positive(const uint8_t *motors, uint8_t length, bool load = true);
    void to_full_positive(std::initializer_list<uint8_t> motors, bool load = true);
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
  private:
    void apply_duty(uint8_t motor, float duty, bool load);
    void create_motor_states(float speed_scale, bool inverted, bool auto_phase);
  };

}