#include "calibration.hpp"

namespace motor {
  Calibration::Pair::Pair()
  : duty(0.0f), speed(0.0f) {
  }

  Calibration::Pair::Pair(float duty, float speed)
  : duty(duty), speed(speed) {
  }

  Calibration::Calibration()
    : calibration(nullptr), calibration_size(0), limit_lower(true), limit_upper(true) {
  }

  Calibration::Calibration(CalibrationType default_type)
    : Calibration() {
    apply_default_pairs(default_type);
  }

  Calibration::Calibration(const Calibration &other)
    : calibration(nullptr), calibration_size(0), limit_lower(other.limit_lower), limit_upper(other.limit_upper) {
    uint size = other.size();
    apply_blank_pairs(size);
    for(uint i = 0; i < size; i++) {
      calibration[i] = other.calibration[i];
    }
  }

  Calibration::~Calibration() {
    if(calibration != nullptr) {
      delete[] calibration;
      calibration = nullptr;
    }
  }

  Calibration &Calibration::operator=(const Calibration &other) {
    uint size = other.size();
    apply_blank_pairs(size);
    for(uint i = 0; i < size; i++) {
      calibration[i] = other.calibration[i];
    }
    limit_lower = other.limit_lower;
    limit_upper = other.limit_upper;

    return *this;
  }

  Calibration::Pair &Calibration::operator[](uint8_t index) {
    assert(index < calibration_size);
    return calibration[index];
  }

  const Calibration::Pair &Calibration::operator[](uint8_t index) const {
    assert(index < calibration_size);
    return calibration[index];
  }

  void Calibration::apply_blank_pairs(uint size) {
    if(calibration != nullptr) {
      delete[] calibration;
    }

    if(size > 0) {
      calibration = new Pair[size];
      calibration_size = size;
    }
    else {
      calibration = nullptr;
      calibration_size = 0;
    }
  }

  void Calibration::apply_two_pairs(float min_duty, float max_duty, float min_speed, float max_speed) {
    apply_blank_pairs(2);
    calibration[0] = Pair(min_duty, min_speed);
    calibration[1] = Pair(max_duty, max_speed);
  }

  void Calibration::apply_three_pairs(float min_duty, float mid_duty, float max_duty, float min_speed, float mid_speed, float max_speed) {
    apply_blank_pairs(3);
    calibration[0] = Pair(min_duty, min_speed);
    calibration[1] = Pair(mid_duty, mid_speed);
    calibration[2] = Pair(max_duty, max_speed);
  }

  void Calibration::apply_uniform_pairs(uint size, float min_duty, float max_duty, float min_speed, float max_speed) {
    apply_blank_pairs(size);
    if(size > 0) {
      float size_minus_one = (float)(size - 1);
      for(uint i = 0; i < size; i++) {
        float duty = Calibration::map_float((float)i, 0.0f, size_minus_one, min_duty, max_duty);
        float speed = Calibration::map_float((float)i, 0.0f, size_minus_one, min_speed, max_speed);
        calibration[i] = Pair(duty, speed);
      }
    }
  }

  void Calibration::apply_default_pairs(CalibrationType default_type) {
    switch(default_type) {
    default:
    case ANGULAR:
      apply_three_pairs(DEFAULT_MIN_PULSE, DEFAULT_MID_PULSE, DEFAULT_MAX_PULSE,
                       -90.0f,            0.0f,              +90.0f);
      break;
    case LINEAR:
      apply_two_pairs(DEFAULT_MIN_PULSE, DEFAULT_MAX_PULSE,
                     0.0f,              1.0f);
      break;
    case CONTINUOUS:
      apply_three_pairs(DEFAULT_MIN_PULSE, DEFAULT_MID_PULSE, DEFAULT_MAX_PULSE,
                       -1.0f,            0.0f,              +1.0f);
      break;
    }
  }

  uint Calibration::size() const {
    return calibration_size;
  }

  Calibration::Pair &Calibration::pair(uint8_t index) {
    assert(index < calibration_size);
    return calibration[index];
  }

  const Calibration::Pair &Calibration::pair(uint8_t index) const {
    assert(index < calibration_size);
    return calibration[index];
  }

  float Calibration::duty(uint8_t index) const {
    return pair(index).duty;
  }

  void Calibration::duty(uint8_t index, float duty) {
    pair(index).duty = duty;
  }

  float Calibration::speed(uint8_t index) const {
    return pair(index).speed;
  }

  void Calibration::speed(uint8_t index, float speed) {
    pair(index).speed = speed;
  }

  Calibration::Pair &Calibration::first() {
    assert(calibration_size > 0);
    return calibration[0];
  }

  const Calibration::Pair &Calibration::first() const {
    assert(calibration_size > 0);
    return calibration[0];
  }

  float Calibration::first_duty() const {
    return first().duty;
  }

  void Calibration::first_duty(float duty) {
    first().duty = duty;
  }

  float Calibration::first_speed() const {
    return first().speed;
  }

  void Calibration::first_speed(float speed) {
    first().speed = speed;
  }

  Calibration::Pair &Calibration::last() {
    assert(calibration_size > 0);
    return calibration[calibration_size - 1];
  }

  const Calibration::Pair &Calibration::last() const {
    assert(calibration_size > 0);
    return calibration[calibration_size - 1];
  }

  float Calibration::last_duty() const {
    return last().duty;
  }

  void Calibration::last_duty(float duty) {
    last().duty = duty;
  }

  float Calibration::last_speed() const {
    return last().speed;
  }

  void Calibration::last_speed(float speed) {
    last().speed = speed;
  }

  bool Calibration::has_lower_limit() const {
    return limit_lower;
  }

  bool Calibration::has_upper_limit() const {
    return limit_upper;
  }

  void Calibration::limit_to_calibration(bool lower, bool upper) {
    limit_lower = lower;
    limit_upper = upper;
  }

  bool Calibration::speed_to_duty(float speed, float &duty_out, float &speed_out) const {
    bool success = false;
    if(calibration_size >= 2) {
      uint8_t last = calibration_size - 1;

      speed_out = speed;

      // Is the speed below the bottom most calibration pair?
      if(speed < calibration[0].speed) {
        // Should the speed be limited to the calibration or projected below it?
        if(limit_lower) {
          duty_out = calibration[0].duty;
          speed_out = calibration[0].speed;
        }
        else {
          duty_out = map_float(speed, calibration[0].speed, calibration[1].speed,
                                       calibration[0].duty, calibration[1].duty);
        }
      }
      // Is the speed above the top most calibration pair?
      else if(speed > calibration[last].speed) {
        // Should the speed be limited to the calibration or projected above it?
        if(limit_upper) {
          duty_out = calibration[last].duty;
          speed_out = calibration[last].speed;
        }
        else {
          duty_out = map_float(speed, calibration[last - 1].speed, calibration[last].speed,
                                       calibration[last - 1].duty, calibration[last].duty);
        }
      }
      else {
        // The speed must between two calibration pairs, so iterate through them to find which ones
        for(uint8_t i = 0; i < last; i++) {
          if(speed <= calibration[i + 1].speed) {
            duty_out = map_float(speed, calibration[i].speed, calibration[i + 1].speed,
                                         calibration[i].duty, calibration[i + 1].duty);
            break; // No need to continue checking so break out of the loop
          }
        }
      }

      // Clamp the duty between the hard limits
      if(duty_out < LOWER_HARD_LIMIT || duty_out > UPPER_HARD_LIMIT) {
        duty_out = MIN(MAX(duty_out, LOWER_HARD_LIMIT), UPPER_HARD_LIMIT);

        // Is the duty below the bottom most calibration pair?
        if(duty_out < calibration[0].duty) {
          speed_out = map_float(duty_out, calibration[0].duty, calibration[1].duty,
                                           calibration[0].speed, calibration[1].speed);
        }
        // Is the duty above the top most calibration pair?
        else if(duty_out > calibration[last].duty) {
          speed_out = map_float(duty_out, calibration[last - 1].duty, calibration[last].duty,
                                           calibration[last - 1].speed, calibration[last].speed);
        }
        else {
          // The duty must between two calibration pairs, so iterate through them to find which ones
          for(uint8_t i = 0; i < last; i++) {
            if(duty_out <= calibration[i + 1].duty) {
              speed_out = map_float(duty_out, calibration[i].duty, calibration[i + 1].duty,
                                               calibration[i].speed, calibration[i + 1].speed);
              break; // No need to continue checking so break out of the loop
            }
          }
        }
      }

      success = true;
    }

    return success;
  }

  bool Calibration::duty_to_speed(float duty, float &speed_out, float &duty_out) const {
    bool success = false;
    if(calibration_size >= 2) {
      uint8_t last = calibration_size - 1;

      // Clamp the duty between the hard limits
      duty_out = MIN(MAX(duty, LOWER_HARD_LIMIT), UPPER_HARD_LIMIT);

      // Is the duty below the bottom most calibration pair?
      if(duty_out < calibration[0].duty) {
        // Should the duty be limited to the calibration or projected below it?
        if(limit_lower) {
          speed_out = calibration[0].speed;
          duty_out = calibration[0].duty;
        }
        else {
          speed_out = map_float(duty, calibration[0].duty, calibration[1].duty,
                                       calibration[0].speed, calibration[1].speed);
        }
      }
      // Is the duty above the top most calibration pair?
      else if(duty > calibration[last].duty) {
        // Should the duty be limited to the calibration or projected above it?
        if(limit_upper) {
          speed_out = calibration[last].speed;
          duty_out = calibration[last].duty;
        }
        else {
          speed_out = map_float(duty, calibration[last - 1].duty, calibration[last].duty,
                                       calibration[last - 1].speed, calibration[last].speed);
        }
      }
      else {
        // The duty must between two calibration pairs, so iterate through them to find which ones
        for(uint8_t i = 0; i < last; i++) {
          if(duty <= calibration[i + 1].duty) {
            speed_out = map_float(duty, calibration[i].duty, calibration[i + 1].duty,
                                         calibration[i].speed, calibration[i + 1].speed);
            break; // No need to continue checking so break out of the loop
          }
        }
      }

      success = true;
    }

    return success;
  }

  float Calibration::map_float(float in, float in_min, float in_max, float out_min, float out_max) {
    return (((in - in_min) * (out_max - out_min)) / (in_max - in_min)) + out_min;
  }
};