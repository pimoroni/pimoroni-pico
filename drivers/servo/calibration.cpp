#include "calibration.hpp"

namespace servo {
  Calibration::Point::Point()
    : pulse(0.0f), value(0.0f) {
  }

  Calibration::Point::Point(uint16_t pulse, float value)
    : pulse(pulse), value(value) {
  }

  Calibration::Calibration()
    : calibration(nullptr), calibration_size(0), limit_lower(true), limit_upper(true) {
    create_default_calibration(ANGULAR);
  }

  Calibration::Calibration(CalibrationType default_type)
    : calibration(nullptr), calibration_size(0), limit_lower(true), limit_upper(true) {
    create_default_calibration(default_type);
  }

  Calibration::~Calibration() {
    if(calibration != nullptr) {
      delete[] calibration;
      calibration = nullptr;
    }
  }

  void Calibration::create_blank_calibration(uint size) {
    if(calibration != nullptr) {
      delete[] calibration;
    }

    if(size > 0) {
      calibration = new Point[size];
      calibration_size = size;
    }
    else {
      calibration = nullptr;
      calibration_size = 0;
    }
  }

  void Calibration::create_two_point_calibration(float min_pulse, float max_pulse, float min_value, float max_value) {
    create_blank_calibration(2);
    calibration[0] = Point(min_pulse, min_value);
    calibration[1] = Point(max_pulse, max_value);
  }

  void Calibration::create_three_point_calibration(float min_pulse, float mid_pulse, float max_pulse, float min_value, float mid_value, float max_value) {
    create_blank_calibration(3);
    calibration[0] = Point(min_pulse, min_value);
    calibration[1] = Point(mid_pulse, mid_value);
    calibration[2] = Point(max_pulse, max_value);
  }

  void Calibration::create_uniform_calibration(uint size, float min_pulse, float max_pulse, float min_value, float max_value) {
    create_blank_calibration(size);
    if(size > 0) {
      float size_minus_one = (float)(size - 1);
      for(uint i = 0; i < size; i++) {
        float pulse = Calibration::map_float((float)i, 0.0f, size_minus_one, min_pulse, max_pulse);
        float value = Calibration::map_float((float)i, 0.0f, size_minus_one, min_value, max_value);
        calibration[i] = Point(pulse, value);
      }
    }
  }

  void Calibration::create_default_calibration(CalibrationType default_type) {
    switch(default_type) {
    default:
    case ANGULAR:
      create_three_point_calibration(DEFAULT_MIN_PULSE, DEFAULT_MID_PULSE, DEFAULT_MAX_PULSE,
                                     -90.0f,            0.0f,              +90.0f);
      break;
    case LINEAR:
      create_two_point_calibration(DEFAULT_MIN_PULSE, DEFAULT_MAX_PULSE,
                                   0.0f,              1.0f);
      break;
    case CONTINUOUS:
      create_three_point_calibration(DEFAULT_MIN_PULSE, DEFAULT_MID_PULSE, DEFAULT_MAX_PULSE,
                                     -1.0f,            0.0f,              +1.0f);
      break;
    case EMPTY:
      create_blank_calibration(0);
    }
  }

  uint Calibration::size() const {
    return calibration_size;
  }

  Calibration::Point* Calibration::point_at(uint8_t index) const {
    if(index < calibration_size) {
      return &calibration[index];
    }
    return nullptr;
  }

  Calibration::Point* Calibration::first_point() const {
    if(calibration_size > 0) {
      return &calibration[0];
    }
    return nullptr;
  }

  Calibration::Point* Calibration::last_point() const {
    if(calibration_size > 0) {
      return &calibration[calibration_size - 1];
    }
    return nullptr;
  }

  void Calibration::limit_to_calibration(bool lower, bool upper) {
    limit_lower = lower;
    limit_upper = upper;
  }


  float Calibration::value_to_pulse(float value) const {
    float pulse = 0.0f;
    if(calibration_size >= 2) {
      uint8_t last = calibration_size - 1;

      // Is the value below the bottom most calibration point?
      if(value < calibration[0].value) {
        // Should the value be limited to the calibration or projected below it?
        if(limit_lower)
          pulse = calibration[0].pulse;
        else
          pulse = map_float(value, calibration[0].value, calibration[1].value,
                                   calibration[0].pulse, calibration[1].pulse);
      }
      // Is the value above the top most calibration point?
      else if(value > calibration[last].value) {
        // Should the value be limited to the calibration or projected above it?
        if(limit_upper)
          pulse = calibration[last].pulse;
        else
          pulse = map_float(value, calibration[last - 1].value, calibration[last].value,
                                   calibration[last - 1].pulse, calibration[last].pulse);
      }
      else {
        // The value must between two calibration points, so iterate through them to find which ones
        for(uint8_t i = 0; i < last; i++) {
          if(value <= calibration[i + 1].value) {
            pulse = map_float(value, calibration[i].value, calibration[i + 1].value,
                                     calibration[i].pulse, calibration[i + 1].pulse);
            break; // No need to continue checking so break out of the loop
          }
        }
      }
    }

    return pulse;
  }

  float Calibration::value_from_pulse(float pulse) const {
    float value = 0.0f;
    if(calibration_size >= 2) {
      uint8_t last = calibration_size - 1;

      // Is the pulse below the bottom most calibration point?
      if(pulse < calibration[0].pulse) {
        // Should the pulse be limited to the calibration or projected below it?
        if(limit_lower)
          value = calibration[0].value;
        else
          value = map_float(pulse, calibration[0].pulse, calibration[1].pulse,
                                   calibration[0].value, calibration[1].value);
      }
      // Is the pulse above the top most calibration point?
      else if(pulse > calibration[last].pulse) {
        // Should the pulse be limited to the calibration or projected above it?
        if(limit_upper)
          value = calibration[last].value;
        else
          value = map_float(pulse, calibration[last - 1].pulse, calibration[last].pulse,
                                   calibration[last - 1].value, calibration[last].value);
      }
      else {
        // The pulse must between two calibration points, so iterate through them to find which ones
        for(uint8_t i = 0; i < last; i++) {
          if(pulse <= calibration[i + 1].pulse) {
            value = map_float(pulse, calibration[i].pulse, calibration[i + 1].pulse,
                                     calibration[i].value, calibration[i + 1].value);
            break; // No need to continue checking so break out of the loop
          }
        }
      }
    }

    return value;
  }

  float Calibration::map_float(float in, float in_min, float in_max, float out_min, float out_max) {
    return (((in - in_min) * (out_max - out_min)) / (in_max - in_min)) + out_min;
  }
};