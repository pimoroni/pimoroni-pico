#pragma once

#include "pico/stdlib.h"
#include "hardware/pwm.h"

namespace pimoroni {

  class Motor {
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
    static const uint16_t DEFAULT_PWM_FREQUENCY = 25000;      // Chose 25KHz because it is outside of hearing
                                                              // and divides nicely into the RP2040's 125MHz PWM frequency
    static const DecayMode DEFAULT_DECAY_MODE = SLOW_DECAY;


    //--------------------------------------------------
    // Variables
    //--------------------------------------------------
  private:
    uint pin_pos;
    uint pin_neg;
    pwm_config pwm_cfg;
    uint16_t pwm_period;
    float pwm_frequency = DEFAULT_PWM_FREQUENCY;

    DecayMode motor_decay_mode = DEFAULT_DECAY_MODE;
    float motor_speed = 0.0f;


    //--------------------------------------------------
    // Constructors/Destructor
    //--------------------------------------------------
  public:
    Motor(uint pin_pos, uint pin_neg, float freq = DEFAULT_PWM_FREQUENCY, DecayMode mode = DEFAULT_DECAY_MODE);
    ~Motor();


    //--------------------------------------------------
    // Methods
    //--------------------------------------------------
  public:
    bool init();

    float get_speed();
    void set_speed(float speed);

    float get_frequency();
    bool set_frequency(float freq);

    DecayMode get_decay_mode();
    void set_decay_mode(DecayMode mode);

    void stop();
    void disable();

    //--------------------------------------------------
  private:
    void update_pwm();
  };

}
