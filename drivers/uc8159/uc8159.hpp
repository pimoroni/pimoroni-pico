#pragma once

#include <initializer_list>

#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "hardware/gpio.h"

#include "../../common/pimoroni_common.hpp"

namespace pimoroni {

  class UC8159 {
    //--------------------------------------------------
    // Variables
    //--------------------------------------------------
  private:
    // screen properties
    uint16_t width;
    uint16_t height;

    // highest possible resolution is 160x296 which at 1 bit per pixel
    // requires 5920 bytes of frame buffer
    //uint8_t frame_buffer[5920] = {0};
    uint8_t *frame_buffer;

    spi_inst_t *spi = PIMORONI_SPI_DEFAULT_INSTANCE;

    // interface pins with our standard defaults where appropriate
    uint CS     = SPI_BG_FRONT_CS;
    uint DC     = 27;
    uint SCK    = SPI_DEFAULT_SCK;
    uint MOSI   = SPI_DEFAULT_MOSI;
    uint BUSY   = 26;
    uint RESET  = 25;

    bool inverted = false;

  public:
    enum colour : uint8_t {
      BLACK = 0,
      WHITE = 1,
      GREEN = 2,
      BLUE = 3,
      RED = 4,
      YELLOW = 5,
      ORANGE = 6,
      CLEAN = 7
    };

    UC8159(uint16_t width, uint16_t height) :
      width(width), height(height), frame_buffer(new uint8_t[width * height / 2]) {
    }

    UC8159(uint16_t width, uint16_t height, uint8_t *frame_buffer) :
      width(width), height(height), frame_buffer(frame_buffer) {
    }

    UC8159(uint16_t width, uint16_t height,
           spi_inst_t *spi,
           uint CS, uint DC, uint SCK, uint MOSI,
           uint BUSY = PIN_UNUSED, uint RESET = PIN_UNUSED) :
      width(width), height(height),
      frame_buffer(new uint8_t[width * height / 2]),
      spi(spi),
      CS(CS), DC(DC), SCK(SCK), MOSI(MOSI), BUSY(BUSY), RESET(RESET) {}

    UC8159(uint16_t width, uint16_t height,
           uint8_t *frame_buffer,
           spi_inst_t *spi,
           uint CS, uint DC, uint SCK, uint MOSI,
           uint BUSY = PIN_UNUSED, uint RESET = PIN_UNUSED) :
      width(width), height(height),
      frame_buffer(frame_buffer),
      spi(spi),
      CS(CS), DC(DC), SCK(SCK), MOSI(MOSI), BUSY(BUSY), RESET(RESET) {}


    //--------------------------------------------------
    // Methods
    //--------------------------------------------------
  public:
    void init();
    void busy_wait();
    bool is_busy();
    void reset();
    void setup();
    void power_off();

    void command(uint8_t reg, size_t len, const uint8_t *data);
    void command(uint8_t reg, std::initializer_list<uint8_t> values);
    void command(uint8_t reg, const uint8_t data) {command(reg, 0, &data);};
    void command(uint8_t reg) {command(reg, 0, nullptr);};
    void data(size_t len, const uint8_t *data);

    void update(bool blocking = true);
    void pixel(int x, int y, int v);
  };

}
