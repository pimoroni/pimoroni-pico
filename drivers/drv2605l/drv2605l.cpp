#include <cstdlib>
#include <math.h>
#include <map>
#include <vector>

#include "drv2605l.hpp"

namespace pimoroni {

  enum reg {
    STATUS                  = 0x00,
    MODE                    = 0x01,
    REALTIME_PLAYBACK       = 0x02,
    LIBRARY_SELECTION       = 0x03,
    WAVEFORM_SEQUENCER      = 0x04,
    GO                      = 0x0c,
    TIME_OFFSET             = 0x0d,
    AUDIO_CONTROL           = 0x11,
    AUDIO_INPUT_LEVEL       = 0x12,
    AUDIO_OUTPUT_DRIVE      = 0x14,
    VOLTAGE                 = 0x16,
    AUTO_CALIBRATION_RESULT = 0x18,
    FEEDBACK_CONTROL        = 0x1a,
    CONTROL1                = 0x1b,
    CONTROL2                = 0x1c,
    CONTROL3                = 0x1d,
    CONTROL4                = 0x1e,
    CONTROL5                = 0x1f,
    LRA_OPEN_LOOP_PERIOD    = 0x20,
    VOLTAGE2                = 0x21,
    LRA_RESONANCE           = 0x22,
  };

  void DRV2605L::init() {
    i2c_init(i2c, 400000);

    gpio_set_function(sda, GPIO_FUNC_I2C); gpio_pull_up(sda);
    gpio_set_function(scl, GPIO_FUNC_I2C); gpio_pull_up(scl);

    reset();
  }

  void DRV2605L::i2c_reg_write_uint8(uint8_t reg, uint8_t value) {
    uint8_t buffer[2] = {reg, value};
    i2c_write_blocking(i2c, address, buffer, 2, false);
  }

  uint8_t DRV2605L::i2c_reg_read_uint8(uint8_t reg) {
    uint8_t value;
    i2c_write_blocking(i2c, address, &reg, 1, true);
    i2c_read_blocking(i2c, address, (uint8_t *)&value, 1, false);
    return value;
  }

  int16_t DRV2605L::i2c_reg_read_int16(uint8_t reg) {
    int16_t value;
    i2c_write_blocking(i2c, address, &reg, 1, true);
    i2c_read_blocking(i2c, address, (uint8_t *)&value, 2, false);
    return value;
  }

  void DRV2605L::reset() {
    i2c_reg_write_uint8(reg::MODE, 0x80);
    sleep_ms(0.1);
    while(i2c_reg_read_uint8(reg::MODE) & 0x80) {
      sleep_ms(0.01);
    }
    
    i2c_reg_write_uint8(reg::MODE, 0x00);//standby=False)
  }

  void DRV2605L::set_feedback_mode(uint8_t mode) {
    switch(mode) {
    case FB_MODE_LRA:
      i2c_reg_write_uint8(reg::FEEDBACK_CONTROL, i2c_reg_read_uint8(reg::FEEDBACK_CONTROL) | 0x80);
      break;
    case FB_MODE_ERM:
    default:
      i2c_reg_write_uint8(reg::FEEDBACK_CONTROL, i2c_reg_read_uint8(reg::FEEDBACK_CONTROL) & 0x7f);
      break;
    }    
  }

  void DRV2605L::set_library(uint8_t library) {
    i2c_reg_write_uint8(reg::LIBRARY_SELECTION, library);
  }

  void DRV2605L::set_mode(uint8_t mode) {
    i2c_reg_write_uint8(reg::MODE, 0x00); //mode=mode)
  }

  void DRV2605L::auto_calibrate(uint8_t loop_gain, uint8_t feedback_brake_factor, uint8_t auto_calibration_time,
                      uint8_t zero_crossing_detection_time, uint8_t idiss_time) {
    uint8_t mode = i2c_reg_read_uint8(reg::MODE);
    i2c_reg_write_uint8(reg::MODE, MODE_AUTO_CALIBTRATION);
    i2c_reg_write_uint8(reg::FEEDBACK_CONTROL, loop_gain | feedback_brake_factor);
    i2c_reg_write_uint8(reg::CONTROL4, auto_calibration_time | zero_crossing_detection_time);
    i2c_reg_write_uint8(reg::CONTROL2, idiss_time);
    i2c_reg_write_uint8(reg::GO, 0x01);
    while(i2c_reg_read_uint8(reg::GO)) {
      sleep_ms(0.01);
    }
    i2c_reg_write_uint8(reg::MODE, mode);
  }

  void DRV2605L::set_realtime_input(uint8_t value) {
    i2c_reg_write_uint8(reg::REALTIME_PLAYBACK, value);
  }

  void DRV2605L::set_realtime_data_format(uint8_t value) {
    switch(value) {
    case RT_UNSIGNED:
      i2c_reg_write_uint8(reg::CONTROL3, i2c_reg_read_uint8(reg::CONTROL3) | 0x08);
      break;
    case RT_SIGNED:
    default:
      i2c_reg_write_uint8(reg::CONTROL3, i2c_reg_read_uint8(reg::CONTROL3) & 0xf7);
      break;
    }
  }

  void DRV2605L::set_waveform(uint8_t slot, uint8_t w) {
    if(slot < 8)
      i2c_reg_write_uint8(reg::WAVEFORM_SEQUENCER + slot, w);
  }

  void DRV2605L::go() {
    i2c_reg_write_uint8(reg::GO, 0x01);
  }

  void DRV2605L::stop() {
    i2c_reg_write_uint8(reg::GO, 0x00);
  }

  bool DRV2605L::busy() {
    return i2c_reg_read_uint8(reg::GO) != 0;
  }

}