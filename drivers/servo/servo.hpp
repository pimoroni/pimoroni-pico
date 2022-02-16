#pragma once

#include <stdint.h>
#include <math.h>

#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include "hardware/clocks.h"
#include "common/pimoroni_common.hpp"

namespace servo {

  class Calibration {
    //--------------------------------------------------
    // Constants
    //--------------------------------------------------
  public:
    static constexpr float DEFAULT_MIN_PULSE = 500.0f;   // in microseconds
    static constexpr float DEFAULT_MID_PULSE = 1500.0f;  // in microseconds
    static constexpr float DEFAULT_MAX_PULSE = 2500.0f;  // in microseconds
    static constexpr float DEFAULT_VALUE_EXTENT = 90.0f; // a range of -90 to +90


    //--------------------------------------------------
    // Substructures
    //--------------------------------------------------
  public:
    struct CalibrationPoint {
        //--------------------------------------------------
        // Constructors/Destructor
        //--------------------------------------------------
        CalibrationPoint();
        CalibrationPoint(uint16_t pulse, float value);


        //--------------------------------------------------
        // Variables
        //--------------------------------------------------
        float pulse;
        float value;
    };

    
    //--------------------------------------------------
    // Constructors/Destructor
    //--------------------------------------------------
  protected:
    Calibration();
    virtual ~Calibration();


    //--------------------------------------------------
    // Methods
    //--------------------------------------------------
  public:
    void create_default_calibration();
    bool create_blank_calibration(uint num_points); // Must have at least two points
    void create_three_point_calibration(float minus_pulse, float zero_pulse, float plus_pulse, float value_extent = DEFAULT_VALUE_EXTENT);
    bool create_uniform_calibration(uint num_points, float min_pulse, float min_value, float max_pulse, float max_value); // Must have at least two points

    uint points();
    bool get_point(uint8_t index, CalibrationPoint& point_out);
    void set_point(uint8_t index, const CalibrationPoint& point); // Ensure the points are entered in ascending value order

    void limit_to_calibration(bool lower, bool upper);


    //--------------------------------------------------
    // Variables
    //--------------------------------------------------
  protected:
    CalibrationPoint* calibration;
    uint calibration_points;
    bool limit_lower;
    bool limit_upper;
  };

  class Converter : public Calibration {
    //--------------------------------------------------
    // Constants
    //--------------------------------------------------
  private:
    static constexpr float LOWER_HARD_LIMIT = 500.0f;   // The minimum microsecond pulse to send
    static constexpr float UPPER_HARD_LIMIT = 2500.0f;  // The maximum microsecond pulse to send
    static constexpr float SERVO_PERIOD = 1000000 / 50;    // This is hardcoded as all servos *should* run at this frequency

    //Integer equivalents
    static const uint16_t LOWER_HARD_LIMIT_I = (uint16_t)LOWER_HARD_LIMIT;
    static const uint16_t UPPER_HARD_LIMIT_I = (uint16_t)UPPER_HARD_LIMIT;
    static const uint64_t SERVO_PERIOD_I = (uint64_t)SERVO_PERIOD;


    //--------------------------------------------------
    // Constructors/Destructor
    //--------------------------------------------------
  public:
    Converter() : Calibration() {}
    virtual ~Converter() {}


    //--------------------------------------------------
    // Methods
    //--------------------------------------------------
  public:
    static uint32_t pulse_to_level(float pulse, uint32_t resolution);
    static uint32_t pulse_to_level(uint16_t pulse, uint32_t resolution);
    float value_to_pulse(float value);
  private:
    static float map_pulse(float value, float min_value, float max_value, float min_pulse, float max_pulse);
  };

  class Servo {
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
    uint pin;
    pwm_config pwm_cfg;
    uint16_t pwm_period;
    float pwm_frequency = DEFAULT_PWM_FREQUENCY;

    float servo_angle = 0.0f;

    Converter converter;


    //--------------------------------------------------
    // Constructors/Destructor
    //--------------------------------------------------
  public:
    Servo(uint pin);
    ~Servo();

    //--------------------------------------------------
    // Methods
    //--------------------------------------------------
  public:
    bool init();

    void enable_servo();
    void disable_servo();

    void set_pulse();
    void set_value(float value);

    Calibration& calibration() {
        return converter;
    }

  private:
    //void update_pwm();
  };

}