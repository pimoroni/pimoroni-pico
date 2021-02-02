#include <cstdlib>
#include <math.h>
#include <map>
#include <vector>

#include "is31fl3731.hpp"

namespace pimoroni {

  constexpr uint8_t CONFIG_BANK = 0x0b;
  constexpr uint8_t NUM_PIXELS = 144;
  constexpr uint8_t NUM_FRAMES = 8;

  constexpr uint8_t ENABLE_OFFSET = 0x00;
  constexpr uint8_t COLOR_OFFSET = 0x24;

  constexpr uint8_t GAMMA[256] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2,
    2, 2, 2, 3, 3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5,
    6, 6, 6, 7, 7, 7, 8, 8, 8, 9, 9, 9, 10, 10, 11, 11,
    11, 12, 12, 13, 13, 13, 14, 14, 15, 15, 16, 16, 17, 17, 18, 18,
    19, 19, 20, 21, 21, 22, 22, 23, 23, 24, 25, 25, 26, 27, 27, 28,
    29, 29, 30, 31, 31, 32, 33, 34, 34, 35, 36, 37, 37, 38, 39, 40,
    40, 41, 42, 43, 44, 45, 46, 46, 47, 48, 49, 50, 51, 52, 53, 54,
    55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70,
    71, 72, 73, 74, 76, 77, 78, 79, 80, 81, 83, 84, 85, 86, 88, 89,
    90, 91, 93, 94, 95, 96, 98, 99, 100, 102, 103, 104, 106, 107, 109, 110,
    111, 113, 114, 116, 117, 119, 120, 121, 123, 124, 126, 128, 129, 131, 132, 134,
    135, 137, 138, 140, 142, 143, 145, 146, 148, 150, 151, 153, 155, 157, 158, 160,
    162, 163, 165, 167, 169, 170, 172, 174, 176, 178, 179, 181, 183, 185, 187, 189,
    191, 193, 194, 196, 198, 200, 202, 204, 206, 208, 210, 212, 214, 216, 218, 220,
    222, 224, 227, 229, 231, 233, 235, 237, 239, 241, 244, 246, 248, 250, 252, 255};
  
  enum mode {
      PICTURE   = 0x00,
      AUTOPLAY  = 0x08,
      AUDIOPLAY = 0x18
  };

  enum reg {
    MODE      = 0x00,
    FRAME     = 0x01,
    AUTPLAY1  = 0x02,
    AUTOPLAY2 = 0x03,
    BLINK     = 0x05,
    AUDIOSYNC = 0x06,
    BREATH1   = 0x07,
    BREATH2   = 0x08,
    SHUTDOWN  = 0x0a,
    GAIN      = 0x0b,
    ADC       = 0x0c,
    BANK      = 0xfd
  };

  void IS31FL3731::init() {
    i2c_init(i2c, 100000);

    gpio_set_function(sda, GPIO_FUNC_I2C); gpio_pull_up(sda);
    gpio_set_function(scl, GPIO_FUNC_I2C); gpio_pull_up(scl);

    i2c_reg_write_uint8(reg::SHUTDOWN, 0b00000001);

    i2c_reg_write_uint8(reg::BANK, CONFIG_BANK);

    i2c_reg_write_uint8(reg::MODE, mode::PICTURE);

    i2c_reg_write_uint8(reg::AUDIOSYNC, 0);

    clear();
  }

  void IS31FL3731::clear() {
    for(auto i = 0u; i < sizeof(buf); i++) {
        buf[i] = 0;
    }
  }

  void IS31FL3731::i2c_reg_write_uint8(uint8_t reg, uint8_t value) {
    uint8_t buffer[2] = {reg, value};
    i2c_write_blocking(i2c, address, buffer, 2, false);
  }

  int16_t IS31FL3731::i2c_reg_read_int16(uint8_t reg) {
    int16_t value;
    i2c_write_blocking(i2c, address, &reg, 1, true);
    i2c_read_blocking(i2c, address, (uint8_t *)&value, 2, false);
    return value;
  }

  void IS31FL3731::enable(std::initializer_list<uint8_t> pattern, uint8_t frame) {
    i2c_reg_write_uint8(reg::BANK, frame);
    uint8_t enable_buf[19];
    uint8_t *ptr = enable_buf;
    enable_buf[0] = ENABLE_OFFSET;
    ptr++;
    for(auto byte : pattern) {
        *ptr++ = byte;
    }
    i2c_write_blocking(i2c, address, enable_buf, sizeof(enable_buf), false);
  }

  void IS31FL3731::set(uint8_t index, uint8_t brightness) {
    buf[index + 1] = GAMMA[brightness];
  }

  void IS31FL3731::update(uint8_t frame) {
    i2c_reg_write_uint8(reg::BANK, frame);
    buf[0] = COLOR_OFFSET;
    i2c_write_blocking(i2c, address, buf, sizeof(buf), false);
    i2c_reg_write_uint8(reg::BANK, CONFIG_BANK); // Switch back to config bank
    i2c_reg_write_uint8(reg::FRAME, frame); // Set the desired frame as active
  }

}