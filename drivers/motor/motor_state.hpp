#pragma once

#include "pico/stdlib.h"

namespace motor {

  struct MotorPins {
    uint positive;
    uint negative;

    MotorPins() : positive(0), negative(0) {}
    MotorPins(uint pos_pin, uint neg_pin) : positive(pos_pin), negative(neg_pin) {}
  };

  struct EncoderPins {
    uint a;
    uint b;

    EncoderPins() : a(0), b(0) {}
    EncoderPins(uint a_pin, uint b_pin) : a(a_pin), b(b_pin) {}
  };

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
    static constexpr float DEFAULT_SPEED_SCALE = 1.0f;      // The standard motor update rate

    static constexpr float MIN_FREQUENCY = 10.0f;           // Lowest achievable with hardware PWM with good resolution
    static constexpr float MAX_FREQUENCY = 50000.0f;          // Highest nice speed
    static constexpr float ZERO_PERCENT = 0.0f;
    static constexpr float ONEHUNDRED_PERCENT = 1.0f;


    //--------------------------------------------------
    // Variables
    //--------------------------------------------------
  private:
    float motor_speed;
    float motor_scale;
    bool inverted;
    float last_enabled_duty;
    bool enabled;
    float deadzone_percent = 0.0f;


    //--------------------------------------------------
    // Constructors/Destructor
    //--------------------------------------------------
  public:
    MotorState();
    MotorState(float speed_scale, bool inverted);


    //--------------------------------------------------
    // Methods
    //--------------------------------------------------
  public:
    float enable_with_return();
    float disable_with_return();
    bool is_enabled() const;
  public:
    float get_duty() const;
    float set_duty_with_return(float duty);

    float get_speed() const;
    float set_speed_with_return(float speed);

    float get_speed_scale() const;
    void set_speed_scale(float speed_scale);

    void invert_direction(bool invert);
    bool is_inverted() const;

    //--------------------------------------------------

    float stop_with_return();
    float to_full_negative_with_return();
    float to_full_positive_with_return();
    float to_percent_with_return(float in, float in_min = ZERO_PERCENT, float in_max = ONEHUNDRED_PERCENT);
    float to_percent_with_return(float in, float in_min, float in_max, float speed_min, float speed_max);

    //--------------------------------------------------
    static int32_t duty_to_level(float duty, uint32_t resolution);

    static float map_float(float in, float in_min, float in_max, float out_min, float out_max);
  };

}