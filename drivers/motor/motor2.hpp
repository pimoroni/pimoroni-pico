#pragma once

#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include "motor_state.hpp"

namespace motor {

  class Motor2 {
    //--------------------------------------------------
    // Variables
    //--------------------------------------------------
  private:
    uint motor_pin_pos;
    uint motor_pin_neg;
    MotorState state;
    pwm_config pwm_cfg;
    uint16_t pwm_period;
    float pwm_frequency;
    MotorState::DecayMode motor_decay_mode;


    //--------------------------------------------------
    // Constructors/Destructor
    //--------------------------------------------------
  public:
    Motor2(uint pin_pos, uint pin_neg, float freq = MotorState::DEFAULT_FREQUENCY, MotorState::DecayMode mode = MotorState::DEFAULT_DECAY_MODE);
    //Motor2(uint pin, /*const Calibration& calibration,*/ float freq = MotorState::DEFAULT_FREQUENCY);
    ~Motor2();


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

    //float duty() const;
    //void duty(float duty);

    float speed() const;
    void speed(float speed);

    float frequency() const;
    bool frequency(float freq);

    MotorState::DecayMode decay_mode();
    void decay_mode(MotorState::DecayMode mode);

    void stop();
    void coast(); // An alias for disable

    //--------------------------------------------------
    float min_speed() const;
    //float mid_speed() const;
    float max_speed() const;

    void to_min();
    //void to_mid();
    void to_max();
    void to_percent(float in, float in_min = MotorState::ZERO_PERCENT, float in_max = MotorState::ONEHUNDRED_PERCENT);
    void to_percent(float in, float in_min, float in_max, float speed_min, float speed_max);

    //Calibration& calibration();
    //const Calibration& calibration() const;

    //--------------------------------------------------
  private:
    void apply_duty(float duty);
  };

}