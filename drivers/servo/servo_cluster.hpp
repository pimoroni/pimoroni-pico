#pragma once

#include "pico/stdlib.h"
#include "pwm_cluster.hpp"
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


    //--------------------------------------------------
    // Variables
    //--------------------------------------------------
  private:
    pimoroni::PWMCluster pwms;
    ServoState servos[NUM_BANK0_GPIOS]; // TODO change this to array of pointers
                                        // so that only the servos actually assigned
                                        // to this cluster have states


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

    // For print access in micropython
    uint get_pin_mask() const;

    void enable(uint servo, bool load = true);
    void disable(uint servo, bool load = true);
    bool is_enabled(uint servo) const;

    float get_value(uint servo) const;
    void set_value(uint servo, float value, bool load = true);

    float get_pulse(uint servo) const;
    void set_pulse(uint servo, float pulse, bool load = true);

    float get_min_value(uint servo) const;
    float get_mid_value(uint servo) const;
    float get_max_value(uint servo) const;

    void to_min(uint servo, bool load = true);
    void to_mid(uint servo, bool load = true);
    void to_max(uint servo, bool load = true);
    void to_percent(uint servo, float in, float in_min = ServoState::ZERO_PERCENT, float in_max = ServoState::ONEHUNDRED_PERCENT, bool load = true);
    void to_percent(uint servo, float in, float in_min, float in_max, float value_min, float value_max, bool load = true);

    Calibration* calibration(uint servo);
    const Calibration* calibration(uint servo) const;
  };

}