#pragma once

#include "pico/stdlib.h"
#include "pwm_cluster.hpp"
#include "servo_state.hpp"

namespace servo {

  class ServoCluster {
    //--------------------------------------------------
    // Variables
    //--------------------------------------------------
  private:
    pimoroni::PWMCluster pwms;
    uint32_t pwm_period;
    float pwm_frequency;
    ServoState* servos;
    float* servo_phases;


    //--------------------------------------------------
    // Constructors/Destructor
    //--------------------------------------------------
  public:
    ServoCluster(PIO pio, uint sm, uint pin_mask, CalibrationType default_type = ANGULAR, float freq = ServoState::DEFAULT_FREQUENCY, bool auto_phase = true);
    ServoCluster(PIO pio, uint sm, uint pin_base, uint pin_count, CalibrationType default_type = ANGULAR, float freq = ServoState::DEFAULT_FREQUENCY, bool auto_phase = true);
    ServoCluster(PIO pio, uint sm, const uint8_t *pins, uint32_t length, CalibrationType default_type = ANGULAR, float freq = ServoState::DEFAULT_FREQUENCY, bool auto_phase = true);
    ServoCluster(PIO pio, uint sm, std::initializer_list<uint8_t> pins, CalibrationType default_type = ANGULAR, float freq = ServoState::DEFAULT_FREQUENCY, bool auto_phase = true);
    ~ServoCluster();


    //--------------------------------------------------
    // Methods
    //--------------------------------------------------
  public:
    bool init();

    uint8_t get_count() const;
    uint8_t get_pin(uint8_t servo) const;

    void enable(uint servo, bool load = true);
    void disable(uint servo, bool load = true);
    bool is_enabled(uint servo) const;

    float get_pulse(uint servo) const;
    void set_pulse(uint servo, float pulse, bool load = true);

    float get_value(uint servo) const;
    void set_value(uint servo, float value, bool load = true);

    float get_phase(uint servo) const;
    void set_phase(uint servo, float phase, bool load = true);

    float get_frequency() const;
    bool set_frequency(float freq);

    //--------------------------------------------------
    float get_min_value(uint servo) const;
    float get_mid_value(uint servo) const;
    float get_max_value(uint servo) const;

    void to_min(uint servo, bool load = true);
    void to_mid(uint servo, bool load = true);
    void to_max(uint servo, bool load = true);
    void to_percent(uint servo, float in, float in_min = ServoState::ZERO_PERCENT, float in_max = ServoState::ONEHUNDRED_PERCENT, bool load = true);
    void to_percent(uint servo, float in, float in_min, float in_max, float value_min, float value_max, bool load = true);

    Calibration* calibration(uint servo);
    const Calibration* calibration(uint servo) const;

    //--------------------------------------------------
  private:
    void apply_pulse(uint servo, float pulse, bool load);
    void create_servo_states(CalibrationType default_type, bool auto_phase);
  };

}