#pragma once

#include <stdint.h>
#include <math.h>

#include "pico/stdlib.h"
#include "hardware/clocks.h"
#include "common/pimoroni_common.hpp"
#include "calibration.hpp"
#include "multi_pwm.hpp"
#include "servo_state.hpp"

namespace servo {

  class ServoCluster {
    //--------------------------------------------------
    // Constants
    //--------------------------------------------------
  public:
    static const uint16_t DEFAULT_PWM_FREQUENCY = 50;       //The standard servo update rate

  private:
    static const uint32_t MAX_PWM_WRAP = UINT16_MAX;
    static constexpr uint16_t MAX_PWM_DIVIDER = (1 << 7);

    static constexpr float MIN_VALID_PULSE = 1.0f;


    //--------------------------------------------------
    // Variables
    //--------------------------------------------------
  private:
    MultiPWM multi_pwm;
    ServoState servos[NUM_BANK0_GPIOS];


    //--------------------------------------------------
    // Constructors/Destructor
    //--------------------------------------------------
  public:
    ServoCluster(PIO pio, uint sm, uint channel_mask);
    ~ServoCluster();

    //--------------------------------------------------
    // Methods
    //--------------------------------------------------
  public:
    bool init();

    void enable(uint servo, bool load = true);
    void disable(uint servo, bool load = true);
    bool is_enabled(uint servo);

    float get_value(uint servo);
    void set_value(uint servo, float value, bool load = true);

    float get_pulse(uint servo);
    void set_pulse(uint servo, float pulse, bool load = true);

    void to_min(uint servo, bool load = true);
    void to_mid(uint servo, bool load = true);
    void to_max(uint servo, bool load = true);
    void to_percent(uint servo, float in, float in_min = 0.0f, float in_max = 1.0f, bool load = true);
    void to_percent(uint servo, float in, float in_min, float in_max, float value_min, float value_max, bool load = true);

    Calibration* calibration(uint servo);
  };

}