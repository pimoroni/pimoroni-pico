#pragma once

#include "pico/stdlib.h"

namespace motor {

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

  private:
    static constexpr float LOWER_HARD_LIMIT = 400.0f;   // The minimum microsecond duty to send
    static constexpr float UPPER_HARD_LIMIT = 2600.0f;  // The maximum microsecond duty to send


    //--------------------------------------------------
    // Substructures
    //--------------------------------------------------
  public:
    struct Pair {
        //--------------------------------------------------
        // Constructors/Destructor
        //--------------------------------------------------
        Pair();
        Pair(float duty, float speed);


        //--------------------------------------------------
        // Variables
        //--------------------------------------------------
        float duty;
        float speed;
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
    void apply_two_pairs(float min_duty, float max_duty, float min_speed, float max_speed);
    void apply_three_pairs(float min_duty, float mid_duty, float max_duty, float min_speed, float mid_speed, float max_speed);
    void apply_uniform_pairs(uint size, float min_duty, float max_duty, float min_speed, float max_speed);
    void apply_default_pairs(CalibrationType default_type);

    uint size() const;

    Pair &pair(uint8_t index); // Ensure the pairs are assigned in ascending speed order
    const Pair &pair(uint8_t index) const; // Ensure the pairs are assigned in ascending speed order
    float duty(uint8_t index) const;
    void duty(uint8_t index, float duty);
    float speed(uint8_t index) const;
    void speed(uint8_t index, float speed);

    Pair &first();
    const Pair &first() const;
    float first_duty() const;
    void first_duty(float duty);
    float first_speed() const;
    void first_speed(float speed);

    Pair &last();
    const Pair &last() const;
    float last_duty() const;
    void last_duty(float duty);
    float last_speed() const;
    void last_speed(float speed);

    bool has_lower_limit() const;
    bool has_upper_limit() const;
    void limit_to_calibration(bool lower, bool upper);

    bool speed_to_duty(float speed, float &duty_out, float &speed_out) const;
    bool duty_to_speed(float duty, float &speed_out, float &duty_out) const;

    //static float map_float(float in, float in_min, float in_max, float out_min, float out_max);


    //--------------------------------------------------
    // Variables
    //--------------------------------------------------
  private:
    Pair* calibration;
    uint calibration_size;
    bool limit_lower;
    bool limit_upper;
  };

}