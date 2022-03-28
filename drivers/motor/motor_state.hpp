#pragma once

#include "pico/stdlib.h"
#include "calibration.hpp"

namespace motor {

  class MotorState {
    //--------------------------------------------------
    // Enums
    //--------------------------------------------------
  public:
    enum DecayMode {
      FAST_DECAY  = 0, //aka 'Coasting'
      SLOW_DECAY  = 1, //aka 'Braking'
    };


    //--------------------------------------------------
    // Constants
    //--------------------------------------------------
  public:
    static constexpr float DEFAULT_FREQUENCY = 25000.0f;      // The standard motor update rate
    static const DecayMode DEFAULT_DECAY_MODE = SLOW_DECAY;
    static constexpr float MIN_FREQUENCY = 10.0f;           // Lowest achievable with hardware PWM with good resolution
    static constexpr float MAX_FREQUENCY = 50000.0f;          // Highest nice speed
    static constexpr float ZERO_PERCENT = 0.0f;
    static constexpr float ONEHUNDRED_PERCENT = 1.0f;

  private:
    static constexpr float MIN_VALID_DUTY = 1.0f;


    //--------------------------------------------------
    // Variables
    //--------------------------------------------------
  private:
    float motor_speed = 0.0f;
    float last_enabled_duty = 0.0f;
    bool enabled = false;
    //Calibration calib;


    //--------------------------------------------------
    // Constructors/Destructor
    //--------------------------------------------------
  public:
    MotorState();
    //MotorState(CalibrationType default_type);
    //MotorState(const Calibration& calibration);


    //--------------------------------------------------
    // Methods
    //--------------------------------------------------
  public:
    float enable();
    float disable();
    bool is_enabled() const;
  private:
    float _enable(); // Internal version of enable without convenient initialisation to the middle
  public:
    float get_duty() const;
    float set_duty_with_return(float duty);

    float get_speed() const;
    float set_speed_with_return(float speed);

  public:
    float get_min_speed() const;
    //float get_mid_speed() const;
    float get_max_speed() const;

    float to_min_with_return();
    //float to_mid_with_return();
    float to_max_with_return();
    float to_percent_with_return(float in, float in_min = ZERO_PERCENT, float in_max = ONEHUNDRED_PERCENT);
    float to_percent_with_return(float in, float in_min, float in_max, float speed_min, float speed_max);

    //Calibration& calibration();
    //const Calibration& calibration() const;

    //--------------------------------------------------
    static int32_t duty_to_level(float duty, uint32_t resolution);

  private:
    static float map_float(float in, float in_min, float in_max, float out_min, float out_max);
  };

}