#pragma once

#include "pico/stdlib.h"
#include "common/pimoroni_common.hpp"

using namespace pimoroni;

namespace brushless {

  // Per-motor state shared by BrushlessCluster and Brushless. A duty is the PWM duty of
  // one leg, 0.0 to 1.0 with 0.5 as the neutral point; commutation stays with the caller.
  class BrushlessState {
    //--------------------------------------------------
    // Constants
    //--------------------------------------------------
  public:
    static constexpr float DEFAULT_FREQUENCY = 25000.0f;
    static constexpr float MIN_FREQUENCY = 10.0f;
    static constexpr float MAX_FREQUENCY = 400000.0f;

    static constexpr float NEUTRAL_POINT = 0.5f;

    static const uint8_t NUM_LEGS = 3;


    //--------------------------------------------------
    // Variables
    //--------------------------------------------------
  private:
    float leg_duties[NUM_LEGS];
    Direction motor_direction;
    bool enabled;


    //--------------------------------------------------
    // Constructors/Destructor
    //--------------------------------------------------
  public:
    BrushlessState();
    BrushlessState(Direction direction);


    //--------------------------------------------------
    // Methods
    //--------------------------------------------------
  public:
    void enable();
    void disable();
    bool is_enabled() const;

    float get_duty(uint8_t leg) const;
    void set_duties(float u, float v, float w);

    // The duty a leg's output should carry now: zero when disabled, and with
    // the direction resolved, reversal being a swap of the V and W legs
    float applied_duty(uint8_t leg) const;

    //--------------------------------------------------

    Direction get_direction() const;
    void set_direction(Direction direction);

    //--------------------------------------------------
    static uint32_t duty_to_level(float duty, uint32_t resolution);
  };

}
