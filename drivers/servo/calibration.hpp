#pragma once

#include "pico/stdlib.h"

namespace servo {

  enum CalibrationType {
    ANGULAR = 0,
    LINEAR,
    CONTINUOUS
  };

  class Calibration {
    //--------------------------------------------------
    // Constants
    //--------------------------------------------------
  public:
    static constexpr float DEFAULT_MIN_PULSE = 500.0f;   // in microseconds
    static constexpr float DEFAULT_MID_PULSE = 1500.0f;  // in microseconds
    static constexpr float DEFAULT_MAX_PULSE = 2500.0f;  // in microseconds

    static const uint MAX_CALIBRATION_PAIRS = 32;

  private:
    static constexpr float LOWER_HARD_LIMIT = 400.0f;   // The minimum microsecond pulse to send
    static constexpr float UPPER_HARD_LIMIT = 2600.0f;  // The maximum microsecond pulse to send


    //--------------------------------------------------
    // Substructures
    //--------------------------------------------------
  public:
    struct Pair {
        //--------------------------------------------------
        // Constructors/Destructor
        //--------------------------------------------------
        Pair();
        Pair(float pulse, float value);


        //--------------------------------------------------
        // Variables
        //--------------------------------------------------
        float pulse;
        float value;
    };


    //--------------------------------------------------
    // Constructors/Destructor
    //--------------------------------------------------
  public:
    Calibration();
    Calibration(CalibrationType default_type);
    Calibration(const Calibration &other);
    virtual ~Calibration();


    //--------------------------------------------------
    // Operators
    //--------------------------------------------------
  public:
    Calibration &operator=(const Calibration &other);
    Pair &operator[](uint8_t index);
    const Pair &operator[](uint8_t index) const;


    //--------------------------------------------------
    // Methods
    //--------------------------------------------------
  public:
    void apply_blank_pairs(uint size);
    void apply_two_pairs(float min_pulse, float max_pulse, float min_value, float max_value);
    void apply_three_pairs(float min_pulse, float mid_pulse, float max_pulse, float min_value, float mid_value, float max_value);
    void apply_uniform_pairs(uint size, float min_pulse, float max_pulse, float min_value, float max_value);
    void apply_default_pairs(CalibrationType default_type);

    uint size() const;

    Pair &pair(uint8_t index); // Ensure the pairs are assigned in ascending value order
    const Pair &pair(uint8_t index) const; // Ensure the pairs are assigned in ascending value order
    float pulse(uint8_t index) const;
    void pulse(uint8_t index, float pulse);
    float value(uint8_t index) const;
    void value(uint8_t index, float value);

    Pair &first();
    const Pair &first() const;
    float first_pulse() const;
    void first_pulse(float pulse);
    float first_value() const;
    void first_value(float value);

    Pair &last();
    const Pair &last() const;
    float last_pulse() const;
    void last_pulse(float pulse);
    float last_value() const;
    void last_value(float value);

    bool has_lower_limit() const;
    bool has_upper_limit() const;
    void limit_to_calibration(bool lower, bool upper);

    bool value_to_pulse(float value, float &pulse_out, float &value_out) const;
    bool pulse_to_value(float pulse, float &value_out, float &pulse_out) const;

    static float map_float(float in, float in_min, float in_max, float out_min, float out_max);


    //--------------------------------------------------
    // Variables
    //--------------------------------------------------
  private:
    Pair calibration[MAX_CALIBRATION_PAIRS];
    uint calibration_size;
    bool limit_lower;
    bool limit_upper;
  };

}