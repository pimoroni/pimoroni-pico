#pragma once

#include "pico/stdlib.h"

namespace pimoroni {

  class Capture {
    //--------------------------------------------------
    // Variables
    //--------------------------------------------------
  private:
    const int32_t captured_count        = 0;
    const int32_t count_change          = 0;
    const float average_frequency       = 0.0f;
    const float counts_per_revolution   = 1;


    //--------------------------------------------------
    // Constructors
    //--------------------------------------------------
  public:
    Capture() {}
    Capture(int32_t captured_count, int32_t count_change, float average_frequency, float counts_per_revolution);


    //--------------------------------------------------
    // Methods
    //--------------------------------------------------
  public:
    int32_t get_count() const;
    float get_revolutions() const;
    float get_angle_degrees() const;
    float get_angle_radians() const;

    int32_t get_count_change() const;

    float get_frequency() const;
    float get_revolutions_per_second() const;
    float get_revolutions_per_minute() const;
    float get_degrees_per_second() const;
    float get_radians_per_second() const;
  };

}