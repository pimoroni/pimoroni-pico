#pragma once

#include "pico/stdlib.h"
#include "pwm_cluster.hpp"
#include "servo_state.hpp"

using namespace pimoroni;

namespace servo {

  class ServoCluster {
    //--------------------------------------------------
    // Variables
    //--------------------------------------------------
  private:
    PWMCluster pwms;
    uint32_t pwm_period;
    float pwm_frequency;
    ServoState* states;
    float* servo_phases;


    //--------------------------------------------------
    // Constructors/Destructor
    //--------------------------------------------------
  public:
    ServoCluster(PIO pio, uint sm, uint pin_mask, CalibrationType default_type = ANGULAR, float freq = ServoState::DEFAULT_FREQUENCY, bool auto_phase = true, PWMCluster::Sequence *seq_buffer = nullptr, PWMCluster::TransitionData *dat_buffer = nullptr);
    ServoCluster(PIO pio, uint sm, uint pin_base, uint pin_count, CalibrationType default_type = ANGULAR, float freq = ServoState::DEFAULT_FREQUENCY, bool auto_phase = true, PWMCluster::Sequence *seq_buffer = nullptr, PWMCluster::TransitionData *dat_buffer = nullptr);
    ServoCluster(PIO pio, uint sm, const uint8_t *pins, uint32_t length, CalibrationType default_type = ANGULAR, float freq = ServoState::DEFAULT_FREQUENCY, bool auto_phase = true, PWMCluster::Sequence *seq_buffer = nullptr, PWMCluster::TransitionData *dat_buffer = nullptr);
    ServoCluster(PIO pio, uint sm, std::initializer_list<uint8_t> pins, CalibrationType default_type = ANGULAR, float freq = ServoState::DEFAULT_FREQUENCY, bool auto_phase = true, PWMCluster::Sequence *seq_buffer = nullptr, PWMCluster::TransitionData *dat_buffer = nullptr);
    ~ServoCluster();


    //--------------------------------------------------
    // Methods
    //--------------------------------------------------
  public:
    bool init();

    uint8_t get_count() const;
    uint8_t get_pin(uint8_t servo) const;

    void enable(uint servo, bool load = true);
    void enable(const uint8_t *servos, uint8_t length, bool load = true);
    void enable(std::initializer_list<uint8_t> servos, bool load = true);
    void enable_all(bool load = true);

    void disable(uint servo, bool load = true);
    void disable(const uint8_t *servos, uint8_t length, bool load = true);
    void disable(std::initializer_list<uint8_t> servos, bool load = true);
    void disable_all(bool load = true);

    bool is_enabled(uint servo) const;

    float get_pulse(uint servo) const;
    void set_pulse(uint servo, float pulse, bool load = true);
    void set_pulse(const uint8_t *servos, uint8_t length, float pulse, bool load = true);
    void set_pulse(std::initializer_list<uint8_t> servos, float pulse, bool load = true);
    void set_all_pulses(float pulse, bool load = true);

    float get_value(uint servo) const;
    void set_value(uint servo, float value, bool load = true);
    void set_value(const uint8_t *servos, uint8_t length, float value, bool load = true);
    void set_value(std::initializer_list<uint8_t> servos, float value, bool load = true);
    void set_all_values(float value, bool load = true);

    float get_phase(uint servo) const;
    void set_phase(uint servo, float phase, bool load = true);
    void set_phase(const uint8_t *servos, uint8_t length, float phase, bool load = true);
    void set_phase(std::initializer_list<uint8_t> servos, float phase, bool load = true);
    void set_all_phases(float phase, bool load = true);

    float get_frequency() const;
    bool set_frequency(float freq);

    //--------------------------------------------------
    float get_min_value(uint servo) const;
    float get_mid_value(uint servo) const;
    float get_max_value(uint servo) const;

    void to_min(uint servo, bool load = true);
    void to_min(const uint8_t *servos, uint8_t length, bool load = true);
    void to_min(std::initializer_list<uint8_t> servos, bool load = true);
    void all_to_min(bool load = true);

    void to_mid(uint servo, bool load = true);
    void to_mid(const uint8_t *servos, uint8_t length, bool load = true);
    void to_mid(std::initializer_list<uint8_t> servos, bool load = true);
    void all_to_mid(bool load = true);

    void to_max(uint servo, bool load = true);
    void to_max(const uint8_t *servos, uint8_t length, bool load = true);
    void to_max(std::initializer_list<uint8_t> servos, bool load = true);
    void all_to_max(bool load = true);

    void to_percent(uint servo, float in, float in_min = ServoState::ZERO_PERCENT, float in_max = ServoState::ONEHUNDRED_PERCENT, bool load = true);
    void to_percent(const uint8_t *servos, uint8_t length, float in, float in_min = ServoState::ZERO_PERCENT, float in_max = ServoState::ONEHUNDRED_PERCENT, bool load = true);
    void to_percent(std::initializer_list<uint8_t> servos, float in, float in_min = ServoState::ZERO_PERCENT, float in_max = ServoState::ONEHUNDRED_PERCENT, bool load = true);
    void all_to_percent(float in, float in_min = ServoState::ZERO_PERCENT, float in_max = ServoState::ONEHUNDRED_PERCENT, bool load = true);

    void to_percent(uint servo, float in, float in_min, float in_max, float value_min, float value_max, bool load = true);
    void to_percent(const uint8_t *servos, uint8_t length, float in, float in_min, float in_max, float value_min, float value_max, bool load = true);
    void to_percent(std::initializer_list<uint8_t> servos, float in, float in_min, float in_max, float value_min, float value_max, bool load = true);
    void all_to_percent(float in, float in_min, float in_max, float value_min, float value_max, bool load = true);

    Calibration& calibration(uint servo);
    const Calibration& calibration(uint servo) const;

    void load();

    //--------------------------------------------------
  private:
    void apply_pulse(uint servo, float pulse, bool load);
    void create_servo_states(CalibrationType default_type, bool auto_phase);
  };

}