#pragma once

namespace pimoroni {

  class PicoScroll {
  private:
    static const uint8_t DEFAULT_ADDRESS = 0x74;
    static const uint BUFFER_SIZE = 144;
  public:
    static const int WIDTH = 17;
    static const int HEIGHT = 7;
    static const uint8_t A = 12;
    static const uint8_t B = 13;
    static const uint8_t X = 14;
    static const uint8_t Y = 15;

  private:
    uint8_t __fb[BUFFER_SIZE];
  
  public:
    void init();
    void update();
    void set_pixel(uint8_t x, uint8_t y, uint8_t v);
    void clear();
    bool is_pressed(uint8_t button);

  private:
    void i2c_write(uint8_t reg, const char *data, uint8_t len);
  };

}