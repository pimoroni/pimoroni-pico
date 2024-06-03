#include <cstdlib>
#include <math.h>
#include <map>
#include <vector>
#include <cstring>

#include "as7343.hpp"

namespace pimoroni {

  bool AS7343::init() {
    if(interrupt != PIN_UNUSED) {
      gpio_set_function(interrupt, GPIO_FUNC_SIO);
      gpio_set_dir(interrupt, GPIO_IN);
      gpio_pull_up(interrupt);
    }

    uint8_t aux_id, revision_id,  hardware_id;
    get_version(aux_id, revision_id, hardware_id);

    if(hardware_id != HARDWARE_ID) {
      return false;
    }

    reset();

    bank_select(0);

    // Enable all 7 channels in output FIFO
    i2c->set_bits(address, reg::FIFO_MAP, 0, FIFO_MAP_CH5 | FIFO_MAP_CH4 | FIFO_MAP_CH3 | FIFO_MAP_CH2 | FIFO_MAP_CH1 | FIFO_MAP_CH0 | FIFO_MAP_ASTATUS);

    // Set the PON bit
    i2c->reg_write_uint8(address, reg::ENABLE, ENABLE_WEN | ENABLE_SMUXEN | ENABLE_SP_EN | ENABLE_PON);

    return true;
  }

  void AS7343::reset() {
    i2c->reg_write_uint8(address, reg::CONTROL, CONTROL_SW_RESET);
    sleep_ms(1000);
  }

  i2c_inst_t* AS7343::get_i2c() const {
    return i2c->get_i2c();
  }

  int AS7343::get_sda() const {
    return i2c->get_sda();
  }

  int AS7343::get_scl() const {
    return i2c->get_scl();
  }

  int AS7343::get_int() const {
    return interrupt;
  }

  void AS7343::bank_select(uint8_t bank) {
    if(bank == 1) {
        i2c->set_bits(address, reg::CFG0, 0, CFG0_BANK);
    } else {
        i2c->clear_bits(address, reg::CFG0, 0, CFG0_BANK);
    }
  }

  void AS7343::get_version(uint8_t &auxid, uint8_t &revid, uint8_t &hwid) {
    bank_select(1);
    auxid = i2c->reg_read_uint8(address, reg::AUXID) & 0b00001111;
    revid = i2c->reg_read_uint8(address, reg::REVID) & 0b00000111;
    hwid = i2c->reg_read_uint8(address, reg::ID);
    bank_select(0);
  }

  void AS7343::set_gain(float gain) {
    if(gain == 0.5f) {
      i2c->reg_write_uint8(address, reg::CFG1, 0);
    } else {
        uint16_t ugain = (uint16_t)gain;
        uint16_t bitlength = 0;
        while(ugain > 0) {
            bitlength++;
            ugain >>= 1;
        }
        i2c->reg_write_uint8(address, reg::CFG1, bitlength & 0x1f);
    }
  }

  void AS7343::set_channels(channel_count channel_count) {
    this->read_cycles = uint8_t(channel_count) / 6;
    this->ch_count = (uint8_t)channel_count;
    uint8_t temp = i2c->reg_read_uint8(address, reg::CFG20) & ~CFG20_18_CH;
    switch(channel_count) {
        case channel_count::SIX_CHANNEL:
            temp |= CFG20_6_CH;
            break;
        case channel_count::TWELVE_CHANNEL:
            temp |= CFG20_12_CH;
            break;
        case channel_count::EIGHTEEN_CHANNEL:
            temp |= CFG20_18_CH;
            break;
    }
    i2c->reg_write_uint8(address, reg::CFG20, temp);
  }

  void AS7343::set_illumination_current(float current) {
    current -= 4;
    current /= 2.0f;
    uint8_t temp = i2c->reg_read_uint8(address, reg::LED) & 0b10000000;
    temp |= (uint8_t)current;
    i2c->reg_write_uint8(address, reg::LED, temp);
  }

  void AS7343::set_illumination_led(bool state) {
    uint8_t temp = i2c->reg_read_uint8(address, reg::LED) & 0b01111111;
    temp |= state ? 0x80 : 0x00;
    i2c->reg_write_uint8(address, reg::LED, temp);
  }

  void AS7343::set_measurement_time(float measurement_time_ms) {
    constexpr float resolution = 2.78f;
    i2c->reg_write_uint8(address, reg::WTIME, (uint8_t)((measurement_time_ms - resolution) / resolution));
  }

  void AS7343::set_integration_time(float integration_time_us, uint8_t repeat) {
    constexpr float resolution = 2.78f;
    constexpr float max_astep = (65534 + 1) * resolution;

    if (integration_time_us <= max_astep) {
      i2c->reg_write_uint8(address, reg::ATIME, repeat - 1);
      i2c->reg_write_uint16(address, reg::ASTEP, (uint16_t)((integration_time_us - resolution) / resolution) & 0xfffe);
    } else {
      // Time out of range...
    }
  }

  void AS7343::read_fifo(uint16_t *buf) {
    uint16_t expected_results = read_cycles * 7;
    uint16_t result_slot = 0;

    while (i2c->reg_read_uint8(address, reg::FIFO_LVL) < expected_results) {
        sleep_ms(1);
    }

    while (i2c->reg_read_uint8(address, reg::FIFO_LVL) > 0 && expected_results > 0) {
        buf[result_slot] = i2c->reg_read_uint16(address, reg::FDATA);
        expected_results--;
        result_slot++;
    }
  }

  AS7343::reading AS7343::read() {
    reading result;
    read_fifo((uint16_t *)&result);
    return result;
  }


}