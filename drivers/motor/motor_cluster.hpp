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
    MotorCluster(PIO pio, uint sm, uint pin_mask, CalibrationType default_type = ANGULAR, float freq = MotorState::DEFAULT_FREQUENCY, bool auto_phase = true, PWMCluster::Sequence *seq_buffer = nullptr, PWMCluster::TransitionData *dat_buffer = nullptr);
    MotorCluster(PIO pio, uint sm, uint pin_base, uint pin_count, CalibrationType default_type = ANGULAR, float freq = MotorState::DEFAULT_FREQUENCY, bool auto_phase = true, PWMCluster::Sequence *seq_buffer = nullptr, PWMCluster::TransitionData *dat_buffer = nullptr);
    MotorCluster(PIO pio, uint sm, const uint8_t *pins, uint32_t length, CalibrationType default_type = ANGULAR, float freq = MotorState::DEFAULT_FREQUENCY, bool auto_phase = true, PWMCluster::Sequence *seq_buffer = nullptr, PWMCluster::TransitionData *dat_buffer = nullptr);
    MotorCluster(PIO pio, uint sm, std::initializer_list<uint8_t> pins, CalibrationType default_type = ANGULAR, float freq = MotorState::DEFAULT_FREQUENCY, bool auto_phase = true, PWMCluster::Sequence *seq_buffer = nullptr, PWMCluster::TransitionData *dat_buffer = nullptr);

    MotorCluster(PIO pio, uint sm, uint pin_mask, const Calibration& calibration, float freq = MotorState::DEFAULT_FREQUENCY, bool auto_phase = true, PWMCluster::Sequence *seq_buffer = nullptr, PWMCluster::TransitionData *dat_buffer = nullptr);
    MotorCluster(PIO pio, uint sm, uint pin_base, uint pin_count, const Calibration& calibration, float freq = MotorState::DEFAULT_FREQUENCY, bool auto_phase = true, PWMCluster::Sequence *seq_buffer = nullptr, PWMCluster::TransitionData *dat_buffer = nullptr);
    MotorCluster(PIO pio, uint sm, const uint8_t *pins, uint32_t length, const Calibration& calibration, float freq = MotorState::DEFAULT_FREQUENCY, bool auto_phase = true, PWMCluster::Sequence *seq_buffer = nullptr, PWMCluster::TransitionData *dat_buffer = nullptr);
    MotorCluster(PIO pio, uint sm, std::initializer_list<uint8_t> pins, const Calibration& calibration, float freq = MotorState::DEFAULT_FREQUENCY, bool auto_phase = true, PWMCluster::Sequence *seq_buffer = nullptr, PWMCluster::TransitionData *dat_buffer = nullptr);
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
    float min_speed(uint8_t motor) const;
    float mid_speed(uint8_t motor) const;
    float max_speed(uint8_t motor) const;

    void to_min(uint8_t motor, bool load = true);
    void to_min(const uint8_t *motors, uint8_t length, bool load = true);
    void to_min(std::initializer_list<uint8_t> motors, bool load = true);
    void all_to_min(bool load = true);

    void to_mid(uint8_t motor, bool load = true);
    void to_mid(const uint8_t *motors, uint8_t length, bool load = true);
    void to_mid(std::initializer_list<uint8_t> motors, bool load = true);
    void all_to_mid(bool load = true);

    void to_max(uint8_t motor, bool load = true);
    void to_max(const uint8_t *motors, uint8_t length, bool load = true);
    void to_max(std::initializer_list<uint8_t> motors, bool load = true);
    void all_to_max(bool load = true);

    void to_percent(uint8_t motor, float in, float in_min = MotorState::ZERO_PERCENT, float in_max = MotorState::ONEHUNDRED_PERCENT, bool load = true);
    void to_percent(const uint8_t *motors, uint8_t length, float in, float in_min = MotorState::ZERO_PERCENT, float in_max = MotorState::ONEHUNDRED_PERCENT, bool load = true);
    void to_percent(std::initializer_list<uint8_t> motors, float in, float in_min = MotorState::ZERO_PERCENT, float in_max = MotorState::ONEHUNDRED_PERCENT, bool load = true);
    void all_to_percent(float in, float in_min = MotorState::ZERO_PERCENT, float in_max = MotorState::ONEHUNDRED_PERCENT, bool load = true);

    void to_percent(uint8_t motor, float in, float in_min, float in_max, float speed_min, float speed_max, bool load = true);
    void to_percent(const uint8_t *motors, uint8_t length, float in, float in_min, float in_max, float speed_min, float speed_max, bool load = true);
    void to_percent(std::initializer_list<uint8_t> motors, float in, float in_min, float in_max, float speed_min, float speed_max, bool load = true);
    void all_to_percent(float in, float in_min, float in_max, float speed_min, float speed_max, bool load = true);

    Calibration& calibration(uint8_t motor);
    const Calibration& calibration(uint8_t motor) const;

    void load();

    //--------------------------------------------------
  private:
    void apply_duty(uint8_t motor, float duty, bool load);
    void create_motor_states(CalibrationType default_type, bool auto_phase);
    void create_motor_states(const Calibration& calibration, bool auto_phase);
  };

}