#include "breakout_encoder_wheel.hpp"
#include <algorithm>
#include <cmath>

namespace pimoroni {
namespace encoderwheel {

  bool BreakoutEncoderWheel::init(bool skip_chip_id_check) {
    bool success = false;
    if(ioe.init(skip_chip_id_check, true) && led_ring.init()) {

      ioe.setup_rotary_encoder(ENC_CHANNEL, ENC_TERM_A, ENC_TERM_B, 0, true); // count microsteps

      ioe.set_mode(SW_UP, IOExpander::PIN_IN_PU);
      ioe.set_mode(SW_DOWN, IOExpander::PIN_IN_PU);
      ioe.set_mode(SW_LEFT, IOExpander::PIN_IN_PU);
      ioe.set_mode(SW_RIGHT, IOExpander::PIN_IN_PU);
      ioe.set_mode(SW_CENTRE, IOExpander::PIN_IN_PU);

      ioe.set_pin_interrupt(SW_UP, true);
      ioe.set_pin_interrupt(SW_DOWN, true);
      ioe.set_pin_interrupt(SW_LEFT, true);
      ioe.set_pin_interrupt(SW_RIGHT, true);
      ioe.set_pin_interrupt(SW_CENTRE, true);

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
    switch(button) {
      case 0:
        return ioe.input(SW_UP) == 0;
      case 1:
        return ioe.input(SW_DOWN) == 0;
      case 2:
        return ioe.input(SW_LEFT) == 0;
      case 3:
        return ioe.input(SW_RIGHT) == 0;
      case 4:
        return ioe.input(SW_CENTRE) == 0;
      default:
        return false;
    }
  }

  int16_t BreakoutEncoderWheel::count() {
    take_encoder_reading();
    return enc_count;
  }

  int16_t BreakoutEncoderWheel::delta() {
    take_encoder_reading();

    // Determine the change in counts since the last time this function was performed
    int16_t change = enc_count - last_delta_count;
    last_delta_count = enc_count;

    return change;
  }

  void BreakoutEncoderWheel::zero() {
    ioe.clear_rotary_encoder(ENC_CHANNEL);
    enc_count = 0;
    enc_step = 0;
    enc_turn = 0;
    last_raw_count = 0;
    last_delta_count = 0;
  }

  int16_t BreakoutEncoderWheel::step() {
    take_encoder_reading();
    return enc_step;
  }

  int16_t BreakoutEncoderWheel::turn() {
    take_encoder_reading();
    return enc_turn;
  }

  float BreakoutEncoderWheel::revolutions() {
    return (float)count() / (float)ENC_COUNTS_PER_REV;
  }

  float BreakoutEncoderWheel::degrees() {
    return revolutions() * 360.0f;
  }

  float BreakoutEncoderWheel::radians() {
    return revolutions() * M_PI * 2.0f;
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
        h = 1.0f + fmodf(h, 1.0f);
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

  uint8_t BreakoutEncoderWheel::gpio_pin_mode(uint8_t gpio) {
    assert(gpio < GP7 || gpio > GP9);
    return ioe.get_mode(gpio);
  }

  void BreakoutEncoderWheel::gpio_pin_mode(uint8_t gpio, uint8_t mode) {
    assert(gpio < GP7 || gpio > GP9);
    ioe.set_mode(gpio, mode);
  }

  int16_t BreakoutEncoderWheel::gpio_pin_value(uint8_t gpio) {
    assert(gpio < GP7 || gpio > GP9);
    return ioe.input(gpio);
  }

  float BreakoutEncoderWheel::gpio_pin_value_as_voltage(uint8_t gpio) {
    assert(gpio < GP7 || gpio > GP9);
    return ioe.input_as_voltage(gpio);
  }

  void BreakoutEncoderWheel::gpio_pin_value(uint8_t gpio, uint16_t value, bool load, bool wait_for_load) {
    assert(gpio < GP7 || gpio > GP9);
    ioe.output(gpio, value, load, wait_for_load);
  }

  void BreakoutEncoderWheel::gpio_pwm_load(bool wait_for_load) {
    ioe.pwm_load(wait_for_load);
  }

  uint16_t BreakoutEncoderWheel::gpio_pwm_frequency(float frequency, bool load, bool wait_for_load) {
    return ioe.set_pwm_frequency(frequency, load, wait_for_load);
  }

  void BreakoutEncoderWheel::take_encoder_reading() {
    // Read the current count
    int16_t raw_count = ioe.read_rotary_encoder(ENC_CHANNEL) / ENC_COUNT_DIVIDER;
    int16_t raw_change = raw_count - last_raw_count;
    last_raw_count = raw_count;

    // Invert the change
    if(enc_direction == REVERSED_DIR) {
      raw_change = 0 - raw_change;
    }

    enc_count += raw_change;

    enc_step += raw_change;
    if(raw_change > 0) {
      while(enc_step >= ENC_COUNTS_PER_REV) {
        enc_step -= ENC_COUNTS_PER_REV;
        enc_turn += 1;
      }
    }
    else if(raw_change < 0) {
      while(enc_step < 0) {
        enc_step += ENC_COUNTS_PER_REV;
        enc_turn -= 1;
      }
    }
  }
}
}