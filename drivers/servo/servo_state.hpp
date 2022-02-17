#pragma once

#include <stdint.h>
#include <math.h>

#include "pico/stdlib.h"
#include "hardware/clocks.h"
#include "common/pimoroni_common.hpp"
#include "calibration.hpp"

namespace servo {

  class ServoState {
    //--------------------------------------------------
    // Variables
    //--------------------------------------------------
  private:
    //uint pin;
    float servo_value = 0.0f;
    float last_enabled_pulse = 0.0f;
    bool enabled = false;

    Converter converter;


    //--------------------------------------------------
    // Constructors/Destructor
    //--------------------------------------------------
  public:
    ServoState(/*uint pin, */Type type = ANGULAR);

    //--------------------------------------------------
    // Methods
    //--------------------------------------------------
  public:
    bool init();

    float enable();
    float disable();
    bool is_enabled();

    float get_value();
    float set_value(float value);

    float get_pulse();
    float set_pulse(float pulse);

    float to_min();
    float to_mid();
    float to_max();
    float to_percent(float in, float in_min, float in_max);
    float to_percent(float in, float in_min, float in_max, float value_min, float value_max);

    float get_min_value();
    float get_mid_value();
    float get_max_value();

    Calibration& calibration();
  };

}