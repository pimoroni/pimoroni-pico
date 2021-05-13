#include <cstdlib>
#include <math.h>
#include <map>
#include <vector>

#include "msa301.hpp"

namespace pimoroni {

  void MSA301::init() {
    i2c_init(i2c, 400000);

    gpio_set_function(sda, GPIO_FUNC_I2C); gpio_pull_up(sda);
    gpio_set_function(scl, GPIO_FUNC_I2C); gpio_pull_up(scl);
    
    if(interrupt != PIN_UNUSED) {
      gpio_set_function(interrupt, GPIO_FUNC_SIO);
      gpio_set_dir(interrupt, GPIO_IN);
      gpio_pull_up(interrupt);
    }

    reset();

    set_power_mode(PowerMode::NORMAL);
    set_range_and_resolution(Range::G_2, Resolution::BITS_14);
  }

  void MSA301::i2c_reg_write_uint8(uint8_t reg, uint8_t value) {
    uint8_t buffer[2] = {reg, value};
    i2c_write_blocking(i2c, address, buffer, 2, false);
  }

  uint8_t MSA301::i2c_reg_read_uint8(uint8_t reg) {
    uint8_t value;
    i2c_write_blocking(i2c, address, &reg, 1, true);
    i2c_read_blocking(i2c, address, (uint8_t *)&value, 1, false);
    return value;
  }

  int16_t MSA301::i2c_reg_read_int16(uint8_t reg) {
    int16_t value;
    i2c_write_blocking(i2c, address, &reg, 1, true);
    i2c_read_blocking(i2c, address, (uint8_t *)&value, 2, false);
    return value;
  }

  void MSA301::reset() {
    i2c_reg_write_uint8(SOFT_RESET, 0b00100100);
    sleep_ms(1);
  }

  uint8_t MSA301::part_id() {
    return i2c_reg_read_uint8(PART_ID);
  }

  float MSA301::get_axis(Axis axis, uint8_t sample_count) {
    if(sample_count > 1) {
      int32_t total = 0;
      for(uint8_t i = 0; i < sample_count; i++) {
        total += i2c_reg_read_int16(int(axis));
      }
      total /= sample_count;
      return total / 16384.0f;
    }

    return i2c_reg_read_int16(axis) / 16384.0f;
  }

  MSA301::Orientation MSA301::get_orientation() {
    return (Orientation)((i2c_reg_read_uint8(ORIENTATION_STATUS) >> 4) & 0b11);
  }

  void MSA301::set_power_mode(MSA301::PowerMode power_mode) {
    i2c_reg_write_uint8(POWER_MODE_BANDWIDTH, power_mode); 
  }

  void MSA301::set_range_and_resolution(Range range, MSA301::Resolution resolution) { 
    i2c_reg_write_uint8(RESOLUTION_RANGE, range | resolution); 
  }

  void MSA301::set_axis_polarity(int polarity) {
    i2c_reg_write_uint8(SET_AXIS_POLARITY, polarity);
  }

  void MSA301::disable_all_interrupts() {
    enable_interrupts(MSA301::Interrupt::NONE);
  }
  
  void MSA301::enable_interrupts(int interrupts) { 
    i2c_reg_write_uint8(INTERRUPT_ENABLE_0, interrupts & 0xff); 
    i2c_reg_write_uint8(INTERRUPT_ENABLE_1, (interrupts & 0xff00) >> 8); 
  }

  bool MSA301::read_interrupt(Interrupt interrupt) {    
    if(interrupt == NEW_DATA) {
      return i2c_reg_read_uint8(DATA_INTERRUPT) & 0b1;  
    }

    // determine which bit indicates the status of this interrupt
    uint8_t bit = 0;
    if(interrupt == FREEFALL)     bit = 0;
    if(interrupt == ACTIVE)       bit = 2;
    if(interrupt == DOUBLE_TAP)   bit = 4;
    if(interrupt == SINGLE_TAP)   bit = 5;
    if(interrupt == ORIENTATION)  bit = 6;
    
    return i2c_reg_read_uint8(MOTION_INTERRUPT) & (1U << bit);
  }

  void MSA301::set_interrupt_latch(MSA301::InterruptLatchPeriod latch_period, bool reset_latched = false) {
    i2c_reg_write_uint8(INTERRUPT_LATCH_PERIOD, latch_period | (reset_latched ? 0b10000000: 0b0)); 
  }

}