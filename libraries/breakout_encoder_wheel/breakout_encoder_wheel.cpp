#include "breakout_encoder_wheel.hpp"
#include <algorithm>

namespace pimoroni {

  bool BreakoutEncoderWheel::init(bool skip_chip_id_check) {
    bool success = false;
    if(ioe.init(skip_chip_id_check)) {

      if(interrupt_pin != PIN_UNUSED) {
        ioe.enable_interrupt_out(true);
      }

      ioe.setup_rotary_encoder(ENC_CHANNEL, ENC_TERM_A, ENC_TERM_B);

      success = true;
    }

    return success;
  }

  i2c_inst_t* BreakoutEncoderWheel::get_i2c() const {
    return ioe.get_i2c();
  }

  int BreakoutEncoderWheel::get_ioe_address() const {
    return ioe.get_address();
  }

  int BreakoutEncoderWheel::get_led_address() const {
    return led_ring.get_address();
  }

  int BreakoutEncoderWheel::get_sda() const {
    return ioe.get_sda();
  }

  int BreakoutEncoderWheel::get_scl() const {
    return ioe.get_scl();
  }

  int BreakoutEncoderWheel::get_int() const {
    return ioe.get_int();
  }

  void BreakoutEncoderWheel::set_ioe_address(uint8_t address) {
    ioe.set_address(address);
  }
  
  bool BreakoutEncoderWheel::get_interrupt_flag() {
    return ioe.get_interrupt_flag();
  }

  void BreakoutEncoderWheel::clear_interrupt_flag() {
    ioe.clear_interrupt_flag();
  }

  BreakoutEncoderWheel::Direction BreakoutEncoderWheel::get_encoder_direction() {
    return direction;
  }

  void BreakoutEncoderWheel::set_encoder_direction(Direction direction) {
    this->direction = direction;
  }

  void BreakoutEncoderWheel::set_pixel(uint8_t index, uint8_t r, uint8_t g, uint8_t b) {
    RGBLookup rgb = lookup_table[index];
    led_ring.set(rgb.r, r);
    led_ring.set(rgb.g, g);
    led_ring.set(rgb.b, b);
  }

  bool BreakoutEncoderWheel::wheel_available() {
    return (ioe.get_interrupt_flag() > 0);
  }

  int16_t BreakoutEncoderWheel::read_wheel() {
    int16_t count = ioe.read_rotary_encoder(ENC_CHANNEL);
    if(direction != DIRECTION_CW)
      count = 0 - count;

    ioe.clear_interrupt_flag();
    return count;
  }

  void BreakoutEncoderWheel::clear_wheel() {
    ioe.clear_rotary_encoder(ENC_CHANNEL);
  }
}