#pragma once

#include "pico/stdlib.h"
#include "pwm_cluster.hpp"
#include "servo_state.hpp"

using namespace pimoroni;

namespace servo {

  class ServoCluster {
    //--------------------------------------------------
    // Variables
    //--------------------------------------------------
  public:
    static const uint MAX_SERVO_CHANNELS = 32;
  private:
    PWMCluster pwms;
    uint32_t pwm_period;
    float pwm_frequency;
    ServoState states[MAX_SERVO_CHANNELS];
    float servo_phases[MAX_SERVO_CHANNELS];


    //--------------------------------------------------
    // Constructors/Destructor
    //--------------------------------------------------
  public:
    ServoCluster(PIO pio, uint sm, uint pin_mask, CalibrationType default_type = ANGULAR, float freq = ServoState::DEFAULT_FREQUENCY, bool auto_phase = true);
    ServoCluster(PIO pio, uint sm, uint pin_base, uint pin_count, CalibrationType default_type = ANGULAR, float freq = ServoState::DEFAULT_FREQUENCY, bool auto_phase = true);
    ServoCluster(PIO pio, uint sm, const uint8_t *pins, uint32_t length, CalibrationType default_type = ANGULAR, float freq = ServoState::DEFAULT_FREQUENCY, bool auto_phase = true);
    ServoCluster(PIO pio, uint sm, std::initializer_list<uint8_t> pins, CalibrationType default_type = ANGULAR, float freq = ServoState::DEFAULT_FREQUENCY, bool auto_phase = true);

    ServoCluster(PIO pio, uint sm, uint pin_mask, const Calibration& calibration, float freq = ServoState::DEFAULT_FREQUENCY, bool auto_phase = true);
    ServoCluster(PIO pio, uint sm, uint pin_base, uint pin_count, const Calibration& calibration, float freq = ServoState::DEFAULT_FREQUENCY, bool auto_phase = true);
    ServoCluster(PIO pio, uint sm, const uint8_t *pins, uint32_t length, const Calibration& calibration, float freq = ServoState::DEFAULT_FREQUENCY, bool auto_phase = true);
    ServoCluster(PIO pio, uint sm, std::initializer_list<uint8_t> pins, const Calibration& calibration, float freq = ServoState::DEFAULT_FREQUENCY, bool auto_phase = true);
    ~ServoCluster();


    //--------------------------------------------------
    // Methods
    //--------------------------------------------------
  public:
    bool init();

    uint8_t count() const;
    uint8_t pin(uint8_t servo) const;

    void enable(uint8_t servo, bool load = true);
    void enable(const uint8_t *servos, uint8_t length, bool load = true);
    void enable(std::initializer_list<uint8_t> servos, bool load = true);
    void enable_all(bool load = true);

    void disable(uint8_t servo, bool load = true);
    void disable(const uint8_t *servos, uint8_t length, bool load = true);
    void disable(std::initializer_list<uint8_t> servos, bool load = true);
    void disable_all(bool load = true);

    bool is_enabled(uint8_t servo) const;

    float pulse(uint8_t servo) const;
    void pulse(uint8_t servo, float pulse, bool load = true);
    void pulse(const uint8_t *servos, uint8_t length, float pulse, bool load = true);
    void pulse(std::initializer_list<uint8_t> servos, float pulse, bool load = true);
    void all_to_pulse(float pulse, bool load = true);

    float value(uint8_t servo) const;
    void value(uint8_t servo, float value, bool load = true);
    void value(const uint8_t *servos, uint8_t length, float value, bool load = true);
    void value(std::initializer_list<uint8_t> servos, float value, bool load = true);
    void all_to_value(float value, bool load = true);

    float phase(uint8_t servo) const;
    void phase(uint8_t servo, float phase, bool load = true);
    void phase(const uint8_t *servos, uint8_t length, float phase, bool load = true);
    void phase(std::initializer_list<uint8_t> servos, float phase, bool load = true);
    void all_to_phase(float phase, bool load = true);

    float frequency() const;
    bool frequency(float freq);

    //--------------------------------------------------
    float min_value(uint8_t servo) const;
    float mid_value(uint8_t servo) const;
    float max_value(uint8_t servo) const;

    void to_min(uint8_t servo, bool load = true);
    void to_min(const uint8_t *servos, uint8_t length, bool load = true);
    void to_min(std::initializer_list<uint8_t> servos, bool load = true);
    void all_to_min(bool load = true);

    void to_mid(uint8_t servo, bool load = true);
    void to_mid(const uint8_t *servos, uint8_t length, bool load = true);
    void to_mid(std::initializer_list<uint8_t> servos, bool load = true);
    void all_to_mid(bool load = true);

    void to_max(uint8_t servo, bool load = true);
    void to_max(const uint8_t *servos, uint8_t length, bool load = true);
    void to_max(std::initializer_list<uint8_t> servos, bool load = true);
    void all_to_max(bool load = true);

    void to_percent(uint8_t servo, float in, float in_min = ServoState::ZERO_PERCENT, float in_max = ServoState::ONEHUNDRED_PERCENT, bool load = true);
    void to_percent(const uint8_t *servos, uint8_t length, float in, float in_min = ServoState::ZERO_PERCENT, float in_max = ServoState::ONEHUNDRED_PERCENT, bool load = true);
    void to_percent(std::initializer_list<uint8_t> servos, float in, float in_min = ServoState::ZERO_PERCENT, float in_max = ServoState::ONEHUNDRED_PERCENT, bool load = true);
    void all_to_percent(float in, float in_min = ServoState::ZERO_PERCENT, float in_max = ServoState::ONEHUNDRED_PERCENT, bool load = true);

    void to_percent(uint8_t servo, float in, float in_min, float in_max, float value_min, float value_max, bool load = true);
    void to_percent(const uint8_t *servos, uint8_t length, float in, float in_min, float in_max, float value_min, float value_max, bool load = true);
    void to_percent(std::initializer_list<uint8_t> servos, float in, float in_min, float in_max, float value_min, float value_max, bool load = true);
    void all_to_percent(float in, float in_min, float in_max, float value_min, float value_max, bool load = true);

    Calibration& calibration(uint8_t servo);
    const Calibration& calibration(uint8_t servo) const;

    void load();

    //--------------------------------------------------
  private:
    void apply_pulse(uint8_t servo, float pulse, bool load);
    void create_servo_states(CalibrationType default_type, bool auto_phase);
    void create_servo_states(const Calibration& calibration, bool auto_phase);
  };

}