#pragma once

#include <stdint.h>
#include <math.h>

#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include "common/pimoroni_common.hpp"

namespace pimoroni {

  class Motor {
    //--------------------------------------------------
    // Enums
    //--------------------------------------------------
  public:
    enum DecayMode {
      FAST_DECAY  = 0, //aka 'Coasting'
      COASTING    = 0,
      SLOW_DECAY  = 1, //aka 'Braking'
      BRAKING     = 1
    };

    //--------------------------------------------------
    // Constants
    //--------------------------------------------------
  public:
    static const uint16_t DEFAULT_PWM_FREQUENCY = 25000;      // Chose 25KHz because it is outside of hearing
                                                              // and divides nicely into the RP2040's 125MHz PWM frequency
    static const DecayMode DEFAULT_DECAY_MODE = SLOW_DECAY;

  private:
    static const uint32_t MAX_PWM_PERIOD = UINT16_MAX;
    static constexpr uint8_t MAX_PWM_DIVIDER = (1 << 7);


    //--------------------------------------------------
    // Variables
    //--------------------------------------------------
  private:
    uint pin_plus;
    uint pin_minus;
    pwm_config pwm_cfg;
    uint16_t pwm_period;
    float pwm_frequency = DEFAULT_PWM_FREQUENCY;

    DecayMode motor_mode = DEFAULT_DECAY_MODE;
    float motor_speed = 0.0f;


    //--------------------------------------------------
    // Constructors/Destructor
    //--------------------------------------------------
  public:
    Motor(uint pin_plus, uint pin_minus, float freq = DEFAULT_PWM_FREQUENCY, DecayMode mode = DEFAULT_DECAY_MODE);
    ~Motor();


    //--------------------------------------------------
    // Methods
    //--------------------------------------------------
  public:
    bool init();

    float get_speed();
    void set_speed(float speed);

    float get_frequency();
    void set_frequency(float freq);

    DecayMode get_decay_mode();
    void set_mode(DecayMode mode);

    void stop();
    void disable();

  private:
    static bool calculate_pwm_period(float freq, uint16_t& period_out, uint8_t& divider_out);
    void update_pwm();
  };

}
