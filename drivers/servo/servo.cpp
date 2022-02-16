#include "servo.hpp"
#include <cstdio>

namespace servo {
  Calibration::CalibrationPoint::CalibrationPoint()
    : pulse(0.0f), value(0.0f) {
  }

  Calibration::CalibrationPoint::CalibrationPoint(uint16_t pulse, float value)
    : pulse(pulse), value(value) {
  }

  Calibration::Calibration()
    : calibration(nullptr), calibration_points(0), limit_lower(true), limit_upper(true) {
    create_default_calibration();
  }

  Calibration::~Calibration() {
    if(calibration != nullptr) {
      delete[] calibration;
      calibration = nullptr;
    }
  }

  void Calibration::create_default_calibration() {
    create_three_point_calibration(DEFAULT_MIN_PULSE, DEFAULT_MID_PULSE, DEFAULT_MAX_PULSE);
  }

  bool Calibration::create_blank_calibration(uint num_points) {
    bool success = false;
    if(num_points >= 2) {
      if(calibration != nullptr)
        delete[] calibration;
      
      calibration = new CalibrationPoint[num_points];
      calibration_points = num_points;
      success = true;
    }
    return success;
  }

  void Calibration::create_three_point_calibration(float minus_pulse, float zero_pulse, float plus_pulse, float value_extent) {
    create_blank_calibration(3);
    calibration[0] = CalibrationPoint(minus_pulse, -value_extent);
    calibration[1] = CalibrationPoint(zero_pulse,           0.0f);
    calibration[2] = CalibrationPoint(plus_pulse,  +value_extent);
  }

  bool Calibration::create_uniform_calibration(uint num_points, float min_pulse, float min_value, float max_pulse, float max_value) {
    bool success = false;
    if(create_blank_calibration(num_points)) {
      float points_minus_one = (float)(num_points - 1);
      for(uint i = 0; i < num_points; i++) {
        float pulse = ((max_pulse - min_pulse) * (float)i) / points_minus_one;
        float value = ((max_value - min_value) * (float)i) / points_minus_one;
        calibration[i] = CalibrationPoint(pulse, value);
      }
      success = true;
    }
    return success;
  }

  uint Calibration::points() {
    return calibration_points;
  }

  bool Calibration::get_point(uint8_t index, CalibrationPoint& point_out) {
    bool success = false;
    if(index < calibration_points) {
      point_out = CalibrationPoint(calibration[index]);
      success = true;
    }

    return success;
  }

  void Calibration::set_point(uint8_t index, const CalibrationPoint& point) {
    if(index < calibration_points) {
      calibration[index] = CalibrationPoint(point);
    }
  }

  void Calibration::limit_to_calibration(bool lower, bool upper) {
    limit_lower = lower;
    limit_upper = upper;
  }


  uint32_t Converter::pulse_to_level(float pulse, uint32_t resolution) {
    if(pulse != 0) {
      // Constrain the level to hardcoded limits to protect the servo
      pulse = MIN(MAX(pulse, LOWER_HARD_LIMIT), UPPER_HARD_LIMIT);
    }
    return (uint32_t)((pulse * (float)resolution) / SERVO_PERIOD);
  }

  uint32_t Converter::pulse_to_level(uint16_t pulse, uint32_t resolution) {
    if(pulse != 0) {
      // Constrain the level to hardcoded limits to protect the servo
      pulse = MIN(MAX(pulse, LOWER_HARD_LIMIT_I), UPPER_HARD_LIMIT_I);
    }
    return (uint32_t)(((uint64_t)pulse * (uint64_t)resolution) / SERVO_PERIOD);
  }

  float Converter::value_to_pulse(float value) {
    float pulse = 0;
    if(calibration_points >= 2) {
      uint8_t last = calibration_points - 1;

      // Is the value below the bottom most calibration point?
      if(value < calibration[0].value) {
        // Should the value be limited to the calibration or projected below it?
        if(limit_lower)
          pulse = calibration[0].pulse;
        else
          pulse = map_pulse(value, calibration[0].value, calibration[1].value,
                                   calibration[0].pulse, calibration[1].pulse);
      }
      // Is the value above the top most calibration point?
      else if(value > calibration[last].value) {
        // Should the value be limited to the calibration or projected above it?
        if(limit_upper)
          pulse = calibration[last].pulse;
        else
          pulse = map_pulse(value, calibration[last - 1].value, calibration[last].value,
                                   calibration[last - 1].pulse, calibration[last].pulse);
      }
      else {
        // The value must between two calibration points, so iterate through them to find which ones
        for(uint8_t i = 0; i < last; i++) {
          if(value <= calibration[i + 1].value) {
            pulse = map_pulse(value, calibration[i].value, calibration[i + 1].value,
                                     calibration[i].pulse, calibration[i + 1].pulse);
            break; // No need to continue checking so break out of the loop
          }
        }
      }
    }

    return pulse;
  }

  float Converter::map_pulse(float value, float min_value, float max_value, float min_pulse, float max_pulse) {
    return (((value - min_value) * (max_pulse - min_pulse)) / (max_value - min_value)) + min_pulse;
  }

  Servo::Servo(uint pin)
    : pin(pin) {
  };

  Servo::~Servo() {
    gpio_set_function(pin, GPIO_FUNC_NULL);
  }

  bool Servo::init() {
    pwm_cfg = pwm_get_default_config();
    pwm_config_set_wrap(&pwm_cfg, 20000 - 1);

    float div = clock_get_hz(clk_sys) / 1000000;
    pwm_config_set_clkdiv(&pwm_cfg, div);

    pwm_init(pwm_gpio_to_slice_num(pin), &pwm_cfg, true);
    gpio_set_function(pin, GPIO_FUNC_PWM);

    return true;
  }

  void Servo::set_value(float value) {
    float pulse = converter.value_to_pulse(value);
    uint16_t level = (uint16_t)converter.pulse_to_level(pulse, 20000);
    pwm_set_gpio_level(pin, level);
  }

  // void RGBLED::set_brightness(uint8_t brightness) {
  //   led_brightness = brightness;
  //   update_pwm();
  // }

  // void RGBLED::set_hsv(float h, float s, float v) {
  //   float i = floor(h * 6.0f);
  //   float f = h * 6.0f - i;
  //   v *= 255.0f;
  //   uint8_t p = v * (1.0f - s);
  //   uint8_t q = v * (1.0f - f * s);
  //   uint8_t t = v * (1.0f - (1.0f - f) * s);

  //   switch (int(i) % 6) {
  //     case 0: led_r = v; led_g = t; led_b = p; break;
  //     case 1: led_r = q; led_g = v; led_b = p; break;
  //     case 2: led_r = p; led_g = v; led_b = t; break;
  //     case 3: led_r = p; led_g = q; led_b = v; break;
  //     case 4: led_r = t; led_g = p; led_b = v; break;
  //     case 5: led_r = v; led_g = p; led_b = q; break;
  //   }

  //   update_pwm();
  // }

  // void RGBLED::update_pwm() {
  //   uint16_t r16 = GAMMA[led_r];
  //   uint16_t g16 = GAMMA[led_g];
  //   uint16_t b16 = GAMMA[led_b];
  //   r16 *= led_brightness;
  //   g16 *= led_brightness;
  //   b16 *= led_brightness;
  //   if(polarity == Polarity::ACTIVE_LOW) {
  //     r16 = UINT16_MAX - r16;
  //     g16 = UINT16_MAX - g16;
  //     b16 = UINT16_MAX - b16;
  //   }
  //   pwm_set_gpio_level(pin_r, r16);
  //   pwm_set_gpio_level(pin_g, g16);
  //   pwm_set_gpio_level(pin_b, b16);
  // }
};