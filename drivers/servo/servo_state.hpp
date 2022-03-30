#pragma once

#include "pico/stdlib.h"
#include "calibration.hpp"

namespace servo {

  class ServoState {
    //--------------------------------------------------
    // Constants
    //--------------------------------------------------
  public:
    static constexpr float DEFAULT_FREQUENCY = 50.0f;       // The standard servo update rate
    static constexpr float MIN_FREQUENCY = 10.0f;           // Lowest achievable with hardware PWM with good resolution
    static constexpr float MAX_FREQUENCY = 350.0f;          // Highest nice value that still allows the full uS pulse range
                                                            // Some servos are rated for 333Hz for instance
    static constexpr float ZERO_PERCENT = 0.0f;
    static constexpr float ONEHUNDRED_PERCENT = 1.0f;

  private:
    static constexpr float MIN_VALID_PULSE = 1.0f;


    //--------------------------------------------------
    // Variables
    //--------------------------------------------------
  private:
    float servo_value;
    float last_enabled_pulse;
    bool enabled;
    Calibration calib;


    //--------------------------------------------------
    // Constructors/Destructor
    //--------------------------------------------------
  public:
    ServoState();
    ServoState(CalibrationType default_type);
    ServoState(const Calibration& calibration);


    //--------------------------------------------------
    // Methods
    //--------------------------------------------------
  public:
    float enable_with_return();
    float disable_with_return();
    bool is_enabled() const;
  private:
    float _enable_with_return(); // Internal version of enable without convenient initialisation to the middle
  public:
    float get_pulse() const;
    float set_pulse_with_return(float pulse);

    float get_value() const;
    float set_value_with_return(float value);

    //--------------------------------------------------
    float get_min_value() const;
    float get_mid_value() const;
    float get_max_value() const;

    float to_min_with_return();
    float to_mid_with_return();
    float to_max_with_return();
    float to_percent_with_return(float in, float in_min = ZERO_PERCENT, float in_max = ONEHUNDRED_PERCENT);
    float to_percent_with_return(float in, float in_min, float in_max, float value_min, float value_max);

    Calibration& calibration();
    const Calibration& calibration() const;

    //--------------------------------------------------
    static uint32_t pulse_to_level(float pulse, uint32_t resolution, float freq);
  };

}