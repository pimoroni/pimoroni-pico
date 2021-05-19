#include <cstdlib>
#include <math.h>
#include <map>
#include <vector>

#include "msa301.hpp"

namespace pimoroni {

  bool MSA301::init() {
    if(interrupt != PIN_UNUSED) {
      gpio_set_function(interrupt, GPIO_FUNC_SIO);
      gpio_set_dir(interrupt, GPIO_IN);
      gpio_pull_up(interrupt);
    }

    reset();

    set_power_mode(PowerMode::NORMAL);
    set_range_and_resolution(Range::G_2, Resolution::BITS_14);

    return true;
  }

  void MSA301::reset() {
    i2c->reg_write_uint8(address, SOFT_RESET, 0b00100100);
    sleep_ms(1);
  }

  i2c_inst_t* MSA301::get_i2c() const {
    return i2c->get_i2c();
  }

  int MSA301::get_sda() const {
    return i2c->get_sda();
  }

  int MSA301::get_scl() const {
    return i2c->get_scl();
  }

  int MSA301::get_int() const {
    return interrupt;
  }

  uint8_t MSA301::part_id() {
    return i2c->reg_read_uint8(address, PART_ID);
  }

  float MSA301::get_axis(Axis axis, uint8_t sample_count) {
    if(sample_count > 1) {
      int32_t total = 0;
      for(uint8_t i = 0; i < sample_count; i++) {
        total += i2c->reg_read_int16(address, (int)axis);
      }
      total /= sample_count;
      return total / 16384.0f;
    }

    return i2c->reg_read_int16(address, axis) / 16384.0f;
  }

  float MSA301::get_x_axis(uint8_t sample_count) {
    return get_axis(MSA301::X, sample_count);
  }

  float MSA301::get_y_axis(uint8_t sample_count) {
    return get_axis(MSA301::Y, sample_count);
  }

  float MSA301::get_z_axis(uint8_t sample_count) {
    return get_axis(MSA301::Z, sample_count);
  }

  MSA301::Orientation MSA301::get_orientation() {
    return (Orientation)((i2c->reg_read_uint8(address, ORIENTATION_STATUS) >> 4) & 0b11);
  }

  void MSA301::set_power_mode(MSA301::PowerMode power_mode) {
    i2c->reg_write_uint8(address, POWER_MODE_BANDWIDTH, power_mode);
  }

  void MSA301::set_range_and_resolution(Range range, MSA301::Resolution resolution) { 
    i2c->reg_write_uint8(address, RESOLUTION_RANGE, range | resolution); 
  }

  void MSA301::set_axis_polarity(uint8_t polarity) {
    i2c->reg_write_uint8(address, SET_AXIS_POLARITY, polarity);
  }

  void MSA301::disable_all_interrupts() {
    enable_interrupts(MSA301::Interrupt::NONE);
  }

  void MSA301::enable_interrupts(uint16_t interrupts) { 
    i2c->reg_write_uint8(address, INTERRUPT_ENABLE_0, interrupts & 0xff); 
    i2c->reg_write_uint8(address, INTERRUPT_ENABLE_1, (interrupts & 0xff00) >> 8); 
  }

  void MSA301::set_interrupt_latch(MSA301::InterruptLatchPeriod latch_period, bool reset_latched = false) {
    i2c->reg_write_uint8(address, INTERRUPT_LATCH_PERIOD, latch_period | (reset_latched ? 0b10000000: 0b0));
  }

  bool MSA301::read_interrupt(Interrupt interrupt) {
    if(interrupt == NEW_DATA) {
      return i2c->reg_read_uint8(address, DATA_INTERRUPT) & 0b1;
    }

    // determine which bit indicates the status of this interrupt
    uint8_t bit = 0;
    if(interrupt == FREEFALL)     bit = 0;
    if(interrupt == ACTIVE)       bit = 2;
    if(interrupt == DOUBLE_TAP)   bit = 4;
    if(interrupt == SINGLE_TAP)   bit = 5;
    if(interrupt == ORIENTATION)  bit = 6;

    return i2c->reg_read_uint8(address, MOTION_INTERRUPT) & (1U << bit);
  }

}