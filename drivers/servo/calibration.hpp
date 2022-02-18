#pragma once

#include "pico/stdlib.h"

namespace servo {

  enum CalibrationType {
    ANGULAR = 0,
    LINEAR,
    CONTINUOUS,
    EMPTY
  };

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
    struct Point {
        //--------------------------------------------------
        // Constructors/Destructor
        //--------------------------------------------------
        Point();
        Point(uint16_t pulse, float value);


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
    virtual ~Calibration();


    //--------------------------------------------------
    // Methods
    //--------------------------------------------------
  public:
    void create_blank_calibration(uint size);
    void create_two_point_calibration(float min_pulse, float max_pulse, float min_value, float max_value);
    void create_three_point_calibration(float min_pulse, float mid_pulse, float max_pulse, float min_value, float mid_value, float max_value);
    void create_uniform_calibration(uint size, float min_pulse, float max_pulse, float min_value, float max_value);
    void create_default_calibration(CalibrationType default_type);

    uint size() const;
    Point* point_at(uint8_t index) const; // Ensure the points are assigned in ascending value order
    Point* first_point() const;
    Point* last_point() const;

    void limit_to_calibration(bool lower, bool upper);

    float value_to_pulse(float value) const;
    float value_from_pulse(float pulse) const;

    static float map_float(float in, float in_min, float in_max, float out_min, float out_max);


    //--------------------------------------------------
    // Variables
    //--------------------------------------------------
  protected:
    Point* calibration;
    uint calibration_size;
    bool limit_lower;
    bool limit_upper;
  };

}