#pragma once

#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include "servo_state.hpp"

namespace servo {

  class Servo {
    //--------------------------------------------------
    // Variables
    //--------------------------------------------------
  private:
    uint servo_pin;
    ServoState state;
    pwm_config pwm_cfg;
    uint16_t pwm_period;
    float pwm_frequency;


    //--------------------------------------------------
    // Constructors/Destructor
    //--------------------------------------------------
  public:
    Servo(uint pin, CalibrationType default_type = ANGULAR, float freq = ServoState::DEFAULT_FREQUENCY);
    Servo(uint pin, const Calibration& calibration, float freq = ServoState::DEFAULT_FREQUENCY);
    ~Servo();

    //--------------------------------------------------
    // Methods
    //--------------------------------------------------
  public:
    bool init();

    // For print access in micropython
    uint pin() const;

    void enable();
    void disable();
    bool is_enabled() const;

    float pulse() const;
    void pulse(float pulse);

    float value() const;
    void value(float value);

    float frequency() const;
    bool frequency(float freq);

    //--------------------------------------------------
    float min_value() const;
    float mid_value() const;
    float max_value() const;

    void to_min();
    void to_mid();
    void to_max();
    void to_percent(float in, float in_min = ServoState::ZERO_PERCENT, float in_max = ServoState::ONEHUNDRED_PERCENT);
    void to_percent(float in, float in_min, float in_max, float value_min, float value_max);

    Calibration& calibration();
    const Calibration& calibration() const;

    //--------------------------------------------------
  private:
    void apply_pulse(float pulse);
  };

}