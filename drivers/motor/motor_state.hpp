#pragma once

#include "pico/stdlib.h"
#include "common/pimoroni_common.hpp"

using namespace pimoroni;

namespace motor {

  enum DecayMode {
    FAST_DECAY  = 0, //aka 'Coasting'
    SLOW_DECAY  = 1, //aka 'Braking'
  };

  class MotorState {
    //--------------------------------------------------
    // Constants
    //--------------------------------------------------
  public:
    static constexpr float DEFAULT_SPEED_SCALE = 1.0f;        // The standard motor speed scale
    static constexpr float DEFAULT_ZEROPOINT = 0.0f;          // The standard motor zeropoint
    static constexpr float DEFAULT_DEADZONE = 0.05f;          // The standard motor deadzone

    static const DecayMode DEFAULT_DECAY_MODE = SLOW_DECAY;   // The standard motor decay behaviour
    static constexpr float DEFAULT_FREQUENCY = 25000.0f;      // The standard motor update rate
    static constexpr float MIN_FREQUENCY = 10.0f;
    static constexpr float MAX_FREQUENCY = 400000.0f;

    static constexpr float ZERO_PERCENT = 0.0f;
    static constexpr float ONEHUNDRED_PERCENT = 1.0f;


    //--------------------------------------------------
    // Variables
    //--------------------------------------------------
  private:
    float motor_speed;
    float last_enabled_duty;
    bool enabled;

    // Customisation variables
    Direction motor_direction;
    float motor_scale;
    float motor_zeropoint;
    float motor_deadzone;


    //--------------------------------------------------
    // Constructors/Destructor
    //--------------------------------------------------
  public:
    MotorState();
    MotorState(Direction direction, float speed_scale, float zeropoint, float deadzone);


    //--------------------------------------------------
    // Methods
    //--------------------------------------------------
  public:
    float enable_with_return();
    float disable_with_return();
    bool is_enabled() const;

    float get_duty() const;
    float get_deadzoned_duty() const;
    float set_duty_with_return(float duty);

    float get_speed() const;
    float set_speed_with_return(float speed);

    //--------------------------------------------------

    float stop_with_return();
    float full_negative_with_return();
    float full_positive_with_return();
    float to_percent_with_return(float in, float in_min = ZERO_PERCENT, float in_max = ONEHUNDRED_PERCENT);
    float to_percent_with_return(float in, float in_min, float in_max, float speed_min, float speed_max);

    //--------------------------------------------------

    Direction get_direction() const;
    void set_direction(Direction direction);

    float get_speed_scale() const;
    void set_speed_scale(float speed_scale);

    float get_zeropoint() const;
    void set_zeropoint(float zeropoint);

    float get_deadzone() const;
    float set_deadzone_with_return(float deadzone);

    //--------------------------------------------------
    static int32_t duty_to_level(float duty, uint32_t resolution);

    static float map_float(float in, float in_min, float in_max, float out_min, float out_max);
  private:
    static float duty_to_speed(float duty, float zeropoint, float scale);
    static float speed_to_duty(float speed, float zeropoint, float scale);
  };

}