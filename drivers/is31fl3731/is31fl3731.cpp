#include <cstdlib>
#include <math.h>
#include <map>
#include <vector>

#include "is31fl3731.hpp"
#include "common/pimoroni_common.hpp"

namespace pimoroni {

  constexpr uint8_t CONFIG_BANK = 0x0b;
  constexpr uint8_t NUM_PIXELS = 144;
  constexpr uint8_t NUM_FRAMES = 8;

  constexpr uint8_t ENABLE_OFFSET = 0x00;
  constexpr uint8_t COLOR_OFFSET = 0x24;
  
  enum mode {
    PICTURE   = 0x00,
    AUTOPLAY  = 0x08,
    AUDIOPLAY = 0x18
  };

  enum reg : uint8_t {
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

  bool IS31FL3731::init() {

    i2c->reg_write_uint8(address, reg::BANK, CONFIG_BANK);
    i2c->reg_write_uint8(address, reg::SHUTDOWN, 0b00000000);

    clear();
    update(0);

    i2c->reg_write_uint8(address, reg::SHUTDOWN, 0b00000001);    

    i2c->reg_write_uint8(address, reg::MODE, mode::PICTURE);

    i2c->reg_write_uint8(address, reg::AUDIOSYNC, 0);

    return true;
  }

  i2c_inst_t* IS31FL3731::get_i2c() const {
    return i2c->get_i2c();
  }

  int IS31FL3731::get_address() const {
    return address;
  }

  int IS31FL3731::get_sda() const {
    return i2c->get_sda();
  }

  int IS31FL3731::get_scl() const {
    return i2c->get_scl();
  }

  void IS31FL3731::clear() {
    for(auto i = 0u; i < sizeof(buf); i++) {
      buf[i] = 0;
    }
  }

  void IS31FL3731::enable(std::initializer_list<uint8_t> pattern, uint8_t frame) {
    i2c->reg_write_uint8(address, reg::BANK, frame);
    i2c->write_bytes(address, ENABLE_OFFSET, pattern.begin(), pattern.size());
  }

  void IS31FL3731::set(uint8_t index, uint8_t brightness) {
    buf[index + 1] = pimoroni::GAMMA_8BIT[brightness];
  }

  void IS31FL3731::update(uint8_t frame) {
    i2c->reg_write_uint8(address, reg::BANK, frame);
    buf[0] = COLOR_OFFSET;
    i2c->write_blocking(address, buf, sizeof(buf), false);
    i2c->reg_write_uint8(address, reg::BANK, CONFIG_BANK); // Switch back to config bank
    i2c->reg_write_uint8(address, reg::FRAME, frame); // Set the desired frame as active
  }
}