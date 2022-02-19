#pragma once

#include "pico/stdlib.h"
#include "calibration.hpp"

namespace servo {

  class ServoState {
    //--------------------------------------------------
    // Constants
    //--------------------------------------------------
  public:
    static constexpr float ZERO_PERCENT = 0.0f;
    static constexpr float ONEHUNDRED_PERCENT = 1.0f;

  private:
    static constexpr float MIN_VALID_PULSE = 1.0f;


    //--------------------------------------------------
    // Variables
    //--------------------------------------------------
  private:
    float servo_value = 0.0f;
    float last_enabled_pulse = 0.0f;
    bool enabled = false;
    Calibration table;


    //--------------------------------------------------
    // Constructors/Destructor
    //--------------------------------------------------
  public:
    ServoState(CalibrationType default_type = ANGULAR);


    //--------------------------------------------------
    // Methods
    //--------------------------------------------------
  public:
    bool init();

    float enable();
    float disable();
    bool is_enabled() const;
  private:
    float _enable(); // Internal version of enable without convenient initialisation to mid point
  public:
    float get_value() const;
    float set_value(float value);

    float get_pulse() const;
    float set_pulse(float pulse);

  public:
    float get_min_value() const;
    float get_mid_value() const;
    float get_max_value() const;

    float to_min();
    float to_mid();
    float to_max();
    float to_percent(float in, float in_min = ZERO_PERCENT, float in_max = ONEHUNDRED_PERCENT);
    float to_percent(float in, float in_min, float in_max, float value_min, float value_max);

    Calibration& calibration();
    const Calibration& calibration() const;

    //--------------------------------------------------
    static uint32_t pulse_to_level(float pulse, uint32_t resolution, float freq);
  };

}