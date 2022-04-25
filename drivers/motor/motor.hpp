#pragma once

#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include "common/pimoroni_common.hpp"
#include "motor_state.hpp"

using namespace pimoroni;

namespace motor {

  class Motor {
    //--------------------------------------------------
    // Subclasses
    //--------------------------------------------------
  private:
    class Driver {
      //--------------------------------------------------
      // Variables
      //--------------------------------------------------
    protected:
      pin_pair motor_pins;
      uint16_t pwm_period;


      //--------------------------------------------------
      // Constructors/Destructor
      //--------------------------------------------------
    public:
      Driver(const pin_pair &pins);
      virtual ~Driver();


      //--------------------------------------------------
      // Methods
      //--------------------------------------------------
    public:
      const pin_pair& pins() const;
      virtual void init(pwm_config *pwm_cfg, uint16_t period) = 0;
      virtual void update_frequency(uint8_t div, uint8_t mod, uint16_t period, float duty, DecayMode mode) = 0;
      virtual void apply_duty(float duty, DecayMode mode) = 0;
    };

    class DualPWMDriver : public Driver {
      //--------------------------------------------------
      // Constructors/Destructor
      //--------------------------------------------------
    public:
      DualPWMDriver(pin_pair pins) : Driver(pins) {}
      virtual ~DualPWMDriver() {}


      //--------------------------------------------------
      // Methods
      //--------------------------------------------------
    public:
      virtual void init(pwm_config *pwm_cfg, uint16_t period);
      virtual void update_frequency(uint8_t div, uint8_t mod, uint16_t period, float duty, DecayMode mode);
      virtual void apply_duty(float duty, DecayMode mode);
    };

    class PhEnDriver : public Driver {
      //--------------------------------------------------
      // Methods
      //--------------------------------------------------
    public:
      PhEnDriver(pin_pair pins) : Driver(pins) {}
      virtual ~PhEnDriver() {}


      //--------------------------------------------------
      // Methods
      //--------------------------------------------------
    public:
      virtual void init(pwm_config *pwm_cfg, uint16_t period);
      virtual void update_frequency(uint8_t div, uint8_t mod, uint16_t period, float duty, DecayMode mode);
      virtual void apply_duty(float duty, DecayMode mode);
    };


    //--------------------------------------------------
    // Variables
    //--------------------------------------------------
  private:
    Driver *driver;
    MotorState state;
    pwm_config pwm_cfg;
    float pwm_frequency;
    DecayMode motor_mode;


    //--------------------------------------------------
    // Constructors/Destructor
    //--------------------------------------------------
  public:
    Motor(const pin_pair &pins, Direction direction = NORMAL_DIR, float speed_scale = MotorState::DEFAULT_SPEED_SCALE, float zeropoint = MotorState::DEFAULT_ZEROPOINT,
          float deadzone = MotorState::DEFAULT_DEADZONE, float freq = MotorState::DEFAULT_FREQUENCY, DecayMode mode = MotorState::DEFAULT_DECAY_MODE,
          bool ph_en_driver = false);
    ~Motor();


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

    //--------------------------------------------------

    void stop();
    void coast();
    void brake();
    void full_negative();
    void full_positive();
    void to_percent(float in, float in_min = MotorState::ZERO_PERCENT, float in_max = MotorState::ONEHUNDRED_PERCENT);
    void to_percent(float in, float in_min, float in_max, float speed_min, float speed_max);

    //--------------------------------------------------

    Direction direction() const;
    void direction(Direction direction);

    float speed_scale() const;
    void speed_scale(float speed_scale);

    float zeropoint() const;
    void zeropoint(float zeropoint);

    float deadzone() const;
    void deadzone(float deadzone);

    DecayMode decay_mode();
    void decay_mode(DecayMode mode);
  };

}