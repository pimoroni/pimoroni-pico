#pragma once

#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include "common/pimoroni_common.hpp"
#include "motor_state.hpp"

using namespace pimoroni;

namespace motor {

  class Motor2 {
    //--------------------------------------------------
    // Variables
    //--------------------------------------------------
  private:
    pin_pair motor_pins;
    MotorState state;
    pwm_config pwm_cfg;
    uint16_t pwm_period;
    float pwm_frequency;
    DecayMode motor_decay_mode;

    //--------------------------------------------------
    // Constructors/Destructor
    //--------------------------------------------------
  public:
    Motor2(const pin_pair &pins, Direction direction = NORMAL, float speed_scale = MotorState::DEFAULT_SPEED_SCALE,
           float deadzone = MotorState::DEFAULT_DEADZONE, float freq = MotorState::DEFAULT_FREQUENCY, DecayMode mode = MotorState::DEFAULT_DECAY_MODE);
    ~Motor2();


    //--------------------------------------------------
    // Methods
    //--------------------------------------------------
  public:
    bool init();

    // For print access in micropython
    pin_pair pins() const;

    void enable();
    void disable();
    bool is_enabled() const;

    float duty() const;
    void duty(float duty);

    float speed() const;
    void speed(float speed);

    float frequency() const;
    bool frequency(float freq);

    DecayMode decay_mode();
    void decay_mode(DecayMode mode);

    //--------------------------------------------------

    void stop();
    void coast();
    void brake(); //TODO
    void full_negative();
    void full_positive();
    void to_percent(float in, float in_min = MotorState::ZERO_PERCENT, float in_max = MotorState::ONEHUNDRED_PERCENT);
    void to_percent(float in, float in_min, float in_max, float speed_min, float speed_max);

    //--------------------------------------------------

    Direction direction() const;
    void direction(Direction direction);

    float speed_scale() const;
    void speed_scale(float speed_scale);

    float deadzone() const;
    void deadzone(float deadzone);

    //--------------------------------------------------
  private:
    void apply_duty(float duty, DecayMode mode);
  };

}