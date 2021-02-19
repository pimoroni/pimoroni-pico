#include "bh1745.hpp"
#include <algorithm>

namespace pimoroni {

  /***** Device registers and masks here *****/

  enum reg {
    SYSTEM_CONTROL  = 0x40,
    MODE_CONTROL1   = 0x41,
    MODE_CONTROL2   = 0x42,
    MODE_CONTROL3   = 0x44,
    COLOUR_DATA     = 0x50,
    DINT_DATA       = 0x58,
    INTERRUPT       = 0x60,
    PERSISTENCE     = 0x61,
    THRESHOLD_LOW   = 0x64,
    THRESHOLD_HIGH  = 0x62,
    MANUFACTURER    = 0x92,
  };

  bool BH1745::init() {
    bool succeeded = false;

    i2c_init(i2c, 400000);

    gpio_set_function(sda, GPIO_FUNC_I2C); gpio_pull_up(sda);
    gpio_set_function(scl, GPIO_FUNC_I2C); gpio_pull_up(scl);

    reset();

    if(get_chip_id() == CHIP_ID && get_manufacturer() == MANUFACTURER) {
      clear_bits(reg::SYSTEM_CONTROL, 6);     // Clear INT reset bit
      set_measurement_time_ms(640);
      set_bits(reg::MODE_CONTROL2, 4);        // Enable RGBC
      set_bits(reg::MODE_CONTROL3, 0, 0xff);  // Turn on sensor
      set_threshold_high(0x0000);            // Set threshold so int will always fire
      set_threshold_low(0xFFFF);             // this lets us turn on the LEDs with the int pin
      clear_bits(reg::INTERRUPT, 4);          // Enable interrupt latch

      sleep_ms(320);

      succeeded = true;
    }

    return succeeded;
  }

  void BH1745::reset() {
    set_bits(reg::SYSTEM_CONTROL, 7);

    while(get_bits(reg::SYSTEM_CONTROL, 7)) {
      sleep_ms(100);
    }
  }

  uint8_t BH1745::get_chip_id() {
    uint8_t chip_id;
    read_bytes(reg::SYSTEM_CONTROL, &chip_id, 1);
    return chip_id & 0b00111111;
  }

  uint8_t BH1745::get_manufacturer() {
    uint8_t manufacturer;
    read_bytes(reg::MANUFACTURER, &manufacturer, 1);
    return manufacturer;
  }

  void BH1745::set_measurement_time_ms(uint16_t value) {
    uint8_t reg = 0;
    switch(value) {
      case 160:
        reg = 0b000;
        break;
      case 320:
        reg = 0b001;
        break;
      case 640:
        reg = 0b010;
        break;
      case 1280:
        reg = 0b011;
        break;
      case 2560:
        reg = 0b100;
        break;
      case 5120:
        reg = 0b101;
        break;
    }
    write_bytes(reg::MODE_CONTROL1, &reg, 1);
  }

  void BH1745::set_threshold_high(uint16_t value) {
    write_bytes(reg::THRESHOLD_HIGH, (uint8_t *)&value, 2);
  }

  void BH1745::set_threshold_low(uint16_t value) {
    write_bytes(reg::THRESHOLD_LOW, (uint8_t *)&value, 2);
  }

  void BH1745::set_leds(bool state) {
    if(state)
      set_bits(reg::INTERRUPT, 0);
    else
      clear_bits(reg::INTERRUPT, 0);
  }

  rgbc_t BH1745::get_rgb_scaled() {
    rgbc_t rgbc = get_rgbc_raw();

    if(rgbc.c > 0) {
      rgbc.r = (uint16_t)((uint32_t)rgbc.r * 255 / rgbc.c) & 0xff;
      rgbc.g = (uint16_t)((uint32_t)rgbc.g * 255 / rgbc.c) & 0xff;
      rgbc.b = (uint16_t)((uint32_t)rgbc.b * 255 / rgbc.c) & 0xff;
    }
    else {
      rgbc.r = 0;
      rgbc.g = 0;
      rgbc.b = 0;
    }

    return rgbc;
  }

  rgbc_t BH1745::get_rgb_clamped() {
    rgbc_t rgbc = get_rgbc_raw();

    uint16_t vmax = std::max(rgbc.r, std::max(rgbc.g, rgbc.b));

    rgbc.r = (uint16_t)((uint32_t)rgbc.r * 255 / vmax);
    rgbc.g = (uint16_t)((uint32_t)rgbc.g * 255 / vmax);
    rgbc.b = (uint16_t)((uint32_t)rgbc.b * 255 / vmax);

    return rgbc;
  }

  rgbc_t BH1745::get_rgbc_raw() {
    while(get_bits(reg::MODE_CONTROL2, 7) == 0) {
      sleep_ms(1);
    }
    rgbc_t colour_data;
    read_bytes(reg::COLOUR_DATA, (uint8_t *)&colour_data, 8);
    colour_data.r *= channel_compensation[0];
    colour_data.g *= channel_compensation[1];
    colour_data.b *= channel_compensation[2];
    colour_data.c *= channel_compensation[3];
    return colour_data;
  }

  // i2c functions

  int BH1745::write_bytes(uint8_t reg, uint8_t *buf, int len) {
    uint8_t buffer[len + 1];
    buffer[0] = reg;
    for(int x = 0; x < len; x++) {
      buffer[x + 1] = buf[x];
    }
    return i2c_write_blocking(i2c, address, buffer, len + 1, false);
  };

  int BH1745::read_bytes(uint8_t reg, uint8_t *buf, int len) {
    i2c_write_blocking(i2c, address, &reg, 1, true);
    i2c_read_blocking(i2c, address, buf, len, false);
    return len;
  };

  uint8_t BH1745::get_bits(uint8_t reg, uint8_t shift, uint8_t mask) {
    uint8_t value;
    read_bytes(reg, &value, 1);
    return value & (mask << shift);
  }

  void BH1745::set_bits(uint8_t reg, uint8_t shift, uint8_t mask) {
    uint8_t value;
    read_bytes(reg, &value, 1);
    value |= mask << shift;
    write_bytes(reg, &value, 1);
  }

  void BH1745::clear_bits(uint8_t reg, uint8_t shift, uint8_t mask) {
    uint8_t value;
    read_bytes(reg, &value, 1);
    value &= ~(mask << shift);
    write_bytes(reg, &value, 1);
  }
}