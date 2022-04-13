#pragma once

#include "pico/stdlib.h"

namespace pimoroni {

  class Capture {
    //--------------------------------------------------
    // Variables
    //--------------------------------------------------
  private:
    const int32_t captured_count        = 0;
    const int32_t capture_count_change  = 0;
    const float average_frequency       = 0.0f;
    const float counts_per_revolution   = 1;


    //--------------------------------------------------
    // Constructors
    //--------------------------------------------------
  public:
    Capture() {}
    Capture(int32_t captured_count, int32_t count_change,
            float average_frequency, float counts_per_revolution);


    //--------------------------------------------------
    // Methods
    //--------------------------------------------------
  public:
    int32_t count() const;
    float revolutions() const;
    float angle_degrees() const;
    float angle_radians() const;

    int32_t count_change() const;

    float frequency() const;
    float revolutions_per_second() const;
    float revolutions_per_minute() const;
    float degrees_per_second() const;
    float radians_per_second() const;
  };

}