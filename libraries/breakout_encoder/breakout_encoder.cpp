#include "breakout_encoder.hpp"
#include <algorithm>

namespace pimoroni {

  bool BreakoutEncoder::init(bool skip_chip_id_check) {
    bool success = false;
    if(ioe.init(skip_chip_id_check)) {

      if(interrupt_pin != PIN_UNUSED)
        ioe.enable_interrupt_out(true);

      ioe.setup_rotary_encoder(ENC_CHANNEL, ENC_TERM_A, ENC_TERM_B, ENC_TERM_C);

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

  i2c_inst_t* BreakoutEncoder::get_i2c() const {
    return ioe.get_i2c();
  }

  int BreakoutEncoder::get_address() const {
    return ioe.get_address();
  }

  int BreakoutEncoder::get_sda() const {
    return ioe.get_sda();
  }

  int BreakoutEncoder::get_scl() const {
    return ioe.get_scl();
  }

  int BreakoutEncoder::get_int() const {
    return ioe.get_int();
  }

  void BreakoutEncoder::set_address(uint8_t address) {
    ioe.set_address(address);
  }
  
  bool BreakoutEncoder::get_interrupt_flag() {
    return ioe.get_interrupt_flag();
  }

  void BreakoutEncoder::clear_interrupt_flag() {
    ioe.clear_interrupt_flag();
  }

  BreakoutEncoder::Direction BreakoutEncoder::get_direction() {
    return direction;
  }

  void BreakoutEncoder::set_direction(Direction direction) {
    this->direction = direction;
  }

  void BreakoutEncoder::set_brightness(float brightness) {
    this->brightness = std::min(std::max(brightness, 0.01f), 1.0f);
    
    // Calculate a period large enough to get 0-255 steps at the desired brightness
    uint16_t period = (uint16_t)(255.0f / this->brightness);
    
    ioe.set_pwm_period(period);
  }

  void BreakoutEncoder::set_led(uint8_t r, uint8_t g, uint8_t b) {
    ioe.output(LED_R, r, false);    // Hold off pwm load until the last
    ioe.output(LED_G, g, false);    // Hold off pwm load until the last
    ioe.output(LED_B, b);           // Loads all 3 pwms
  }

  bool BreakoutEncoder::available() {
    return (ioe.get_interrupt_flag() > 0);
  }

  void BreakoutEncoder::clear() {
    ioe.clear_rotary_encoder(ENC_CHANNEL);
  }

  int16_t BreakoutEncoder::read() {
    int16_t count = ioe.read_rotary_encoder(ENC_CHANNEL);
    if(direction != DIRECTION_CW)
      count = 0 - count;

    ioe.clear_interrupt_flag();
    return count;
  }
}