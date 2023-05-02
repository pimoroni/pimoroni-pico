#include "breakout_encoder_wheel.hpp"
#include <algorithm>
#include <cmath>

namespace pimoroni {

  bool BreakoutEncoderWheel::init(bool skip_chip_id_check) {
    bool success = false;
    if(ioe.init(skip_chip_id_check)) {

      if(interrupt_pin != PIN_UNUSED) {
        ioe.enable_interrupt_out(true);
      }

      ioe.setup_rotary_encoder(ENC_CHANNEL, ENC_TERM_A, ENC_TERM_B);

      if(led_ring.init()) {
        led_ring.enable({
          0b00000000, 0b10111111,
          0b00111110, 0b00111110,
          0b00111111, 0b10111110,
          0b00000111, 0b10000110,
          0b00110000, 0b00110000,
          0b00111111, 0b10111110,
          0b00111111, 0b10111110,
          0b01111111, 0b11111110,
          0b01111111, 0b00000000
        }, 0);

        success = true;
      }
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

  bool BreakoutEncoderWheel::pressed(uint button) {
    return 0; // TODO
  }

  int BreakoutEncoderWheel::count() {
    return 0; // TODO
  }

  int BreakoutEncoderWheel::delta() {
    return 0; // TODO
  }

  int BreakoutEncoderWheel::step() {
    return 0; // TODO
  }

  int BreakoutEncoderWheel::turn() {
    return 0; // TODO
  }

  void BreakoutEncoderWheel::zero() {

  }

  float BreakoutEncoderWheel::revolutions() {
    return 0; // TODO
  }

  float BreakoutEncoderWheel::degrees() {
    return 0; // TODO
  }

  float BreakoutEncoderWheel::radians() {
    return 0; // TODO
  }

  Direction BreakoutEncoderWheel::direction() {
    return enc_direction;
  }

  void BreakoutEncoderWheel::direction(Direction direction) {
    enc_direction = direction;
  }

  void BreakoutEncoderWheel::set_rgb(int index, int r, int g, int b) {
    RGBLookup rgb = lookup_table[index];
    led_ring.set(rgb.r, r);
    led_ring.set(rgb.g, g);
    led_ring.set(rgb.b, b);
  }

  void BreakoutEncoderWheel::set_hsv(int index, float h, float s, float v) {
    int r, g, b;
    if(h < 0.0f) {
        h = 1.0f + fmod(h, 1.0f);
    }

    int i = int(h * 6);
    float f = h * 6 - i;

    v = v * 255.0f;

    float sv = s * v;
    float fsv = f * sv;

    auto p = uint8_t(-sv + v);
    auto q = uint8_t(-fsv + v);
    auto t = uint8_t(fsv - sv + v);

    uint8_t bv = uint8_t(v);

    switch (i % 6) {
        default:
        case 0: r = bv; g = t; b = p; break;
        case 1: r = q; g = bv; b = p; break;
        case 2: r = p; g = bv; b = t; break;
        case 3: r = p; g = q; b = bv; break;
        case 4: r = t; g = p; b = bv; break;
        case 5: r = bv; g = p; b = q; break;
    }

    set_rgb(index, r, g, b);
  }

  void BreakoutEncoderWheel::clear() {
    led_ring.clear();
  }

  void BreakoutEncoderWheel::show() {
    led_ring.update();
  }

  int BreakoutEncoderWheel::gpio_pin_mode(int gpio) {
    return 0; // TODO
  }

  void BreakoutEncoderWheel::gpio_pin_mode(int gpio, int mode) {

  }

  int BreakoutEncoderWheel::gpio_pin_value(int gpio) {
    return 0; // TODO
  }

  float BreakoutEncoderWheel::gpio_pin_value_as_voltage(int gpio) {
    return 0; // TODO
  }

  void BreakoutEncoderWheel::gpio_pin_value(int gpio, int value, bool load, bool wait_for_load) {

  }

  void BreakoutEncoderWheel::gpio_pwm_load(bool wait_for_load) {

  }

  int BreakoutEncoderWheel::gpio_pwm_frequency(float frequency, bool load, bool wait_for_load) {
    return 0; // TODO
  }

  /*
  bool BreakoutEncoderWheel::wheel_available() {
    return (ioe.get_interrupt_flag() > 0);
  }

  int16_t BreakoutEncoderWheel::read_wheel() {
    //int16_t count = ioe.read_rotary_encoder(ENC_CHANNEL);
    //if(direction != DIRECTION_CW)
    //  count = 0 - count;

    ioe.clear_interrupt_flag();
    //return count;
    return 0;
  }
  */
}