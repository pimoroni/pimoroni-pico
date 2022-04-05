#pragma once

#include "pico/stdlib.h"

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

    enum Direction {
      NORMAL    = 0,
      REVERSED  = 1,
    };


    //--------------------------------------------------
    // Constants
    //--------------------------------------------------
  public:
    static const Direction DEFAULT_DIRECTION = NORMAL;        // The standard motor direction
    static constexpr float DEFAULT_SPEED_SCALE = 1.0f;        // The standard motor speed scale
    static constexpr float DEFAULT_DEADZONE = 0.1f;           // The standard motor deadzone
    
    static const DecayMode DEFAULT_DECAY_MODE = SLOW_DECAY;   // The standard motor decay behaviour
    static constexpr float DEFAULT_FREQUENCY = 25000.0f;      // The standard motor update rate
    static constexpr float MIN_FREQUENCY = 10.0f;
    static constexpr float MAX_FREQUENCY = 50000.0f;

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
    float motor_deadzone;


    //--------------------------------------------------
    // Constructors/Destructor
    //--------------------------------------------------
  public:
    MotorState();
    MotorState(Direction direction, float speed_scale, float deadzone_percent);


    //--------------------------------------------------
    // Methods
    //--------------------------------------------------
  public:
    float enable_with_return();
    float disable_with_return();
    bool is_enabled() const;

    float get_duty() const;
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

    float get_deadzone_percent() const;
    float set_deadzone_percent_with_return(float deadzone_percent);

    //--------------------------------------------------
    static int32_t duty_to_level(float duty, uint32_t resolution);

    static float map_float(float in, float in_min, float in_max, float out_min, float out_max);
  };

}