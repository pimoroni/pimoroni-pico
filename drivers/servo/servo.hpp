#pragma once

#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include "servo_state.hpp"

namespace servo {

  class Servo {
    //--------------------------------------------------
    // Constants
    //--------------------------------------------------
  public:
    static const uint16_t DEFAULT_FREQUENCY = 50;       //The standard servo update rate

  private:
    static const uint32_t MAX_PWM_WRAP = UINT16_MAX;


    //--------------------------------------------------
    // Variables
    //--------------------------------------------------
  private:
    uint pin;
    pwm_config pwm_cfg;
    uint16_t pwm_period;
    float pwm_frequency = DEFAULT_FREQUENCY;
    ServoState state;


    //--------------------------------------------------
    // Constructors/Destructor
    //--------------------------------------------------
  public:
    Servo(uint pin, CalibrationType default_type = ANGULAR);
    ~Servo();

    //--------------------------------------------------
    // Methods
    //--------------------------------------------------
  public:
    bool init();

    // For print access in micropython
    uint get_pin() const;

    void enable();
    void disable();
    bool is_enabled() const;

    float get_value() const;
    void set_value(float value);

    float get_pulse() const;
    void set_pulse(float pulse);

    float get_frequency() const;
    bool set_frequency(float freq);

    float get_min_value() const;
    float get_mid_value() const;
    float get_max_value() const;

    void to_min();
    void to_mid();
    void to_max();
    void to_percent(float in, float in_min = ServoState::ZERO_PERCENT, float in_max = ServoState::ONEHUNDRED_PERCENT);
    void to_percent(float in, float in_min, float in_max, float value_min, float value_max);

    Calibration& calibration();
    const Calibration& calibration() const;
  private:
    static bool calculate_pwm_factors(float freq, uint16_t& top_out, uint16_t& div16_out);
  };

}