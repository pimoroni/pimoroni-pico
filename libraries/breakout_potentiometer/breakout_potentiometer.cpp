#include "breakout_potentiometer.hpp"
#include <algorithm>

namespace pimoroni {

  bool BreakoutPotentiometer::init(bool skip_chip_id_check) {
    bool success = false;
    if(ioe.init(skip_chip_id_check)) {

      ioe.set_mode(POT_TERM_A, IOExpander::PIN_OUT);
      ioe.set_mode(POT_TERM_B, IOExpander::PIN_OUT);
      ioe.set_mode(POT_INPUT, IOExpander::PIN_ADC);

      if(direction == DIRECTION_CW) {
        // Clockwise increasing
        ioe.output(POT_TERM_A, IOExpander::LOW);
        ioe.output(POT_TERM_B, IOExpander::HIGH);
      }
      else {
        // Counter clockwise increasing
        ioe.output(POT_TERM_A, IOExpander::HIGH);
        ioe.output(POT_TERM_B, IOExpander::LOW);
      }

      // Calculate a period large enough to get 0-255 steps at the desired brightness
      uint16_t period = (uint16_t)(255.0f / brightness);

      ioe.set_pwm_period(period);
      ioe.set_pwm_control(2);     // PWM as fast as we can to avoid LED flicker

      ioe.set_mode(LED_R, IOExpander::PIN_PWM, false, INVERT_OUTPUT);
      ioe.set_mode(LED_G, IOExpander::PIN_PWM, false, INVERT_OUTPUT);
      ioe.set_mode(LED_B, IOExpander::PIN_PWM, false, INVERT_OUTPUT);

      success = true;
    }

    return success;
  }

  i2c_inst_t* BreakoutPotentiometer::get_i2c() const {
    return ioe.get_i2c();
  }

  int BreakoutPotentiometer::get_address() const {
    return ioe.get_address();
  }

  int BreakoutPotentiometer::get_sda() const {
    return ioe.get_sda();
  }

  int BreakoutPotentiometer::get_scl() const {
    return ioe.get_scl();
  }

  int BreakoutPotentiometer::get_int() const {
    return ioe.get_int();
  }

  void BreakoutPotentiometer::set_address(uint8_t address) {
    ioe.set_address(address);
  }

  float BreakoutPotentiometer::get_adc_vref() {
    return ioe.get_adc_vref();
  }

  void BreakoutPotentiometer::set_adc_vref(float vref) {
    ioe.set_adc_vref(vref);
  }

  BreakoutPotentiometer::Direction BreakoutPotentiometer::get_direction() {
    return direction;
  }

  void BreakoutPotentiometer::set_direction(Direction direction) {
    if(direction == DIRECTION_CW) {
      // Clockwise increasing
      ioe.output(POT_TERM_A, IOExpander::LOW);
      ioe.output(POT_TERM_B, IOExpander::HIGH);
    }
    else {
      // Counter clockwise increasing
      ioe.output(POT_TERM_A, IOExpander::HIGH);
      ioe.output(POT_TERM_B, IOExpander::LOW);
    }
    this->direction = direction;
  }

  void BreakoutPotentiometer::set_brightness(float brightness) {
    this->brightness = std::min(std::max(brightness, 0.01f), 1.0f);
    
    // Calculate a period large enough to get 0-255 steps at the desired brightness
    uint16_t period = (uint16_t)(255.0f / this->brightness);
    
    ioe.set_pwm_period(period);
  }

  void BreakoutPotentiometer::set_led(uint8_t r, uint8_t g, uint8_t b) {
    ioe.output(LED_R, r, false);    // Hold off pwm load until the last
    ioe.output(LED_G, g, false);    // Hold off pwm load until the last
    ioe.output(LED_B, b);           // Loads all 3 pwms
  }

float BreakoutPotentiometer::read(uint32_t adc_timeout) {
    return (ioe.input_as_voltage(POT_INPUT, adc_timeout) / ioe.get_adc_vref());
  }

  int16_t BreakoutPotentiometer::read_raw(uint32_t adc_timeout) {
    return ioe.input(POT_INPUT, adc_timeout);
  }
}