#pragma once

#include "drivers/st7789/st7789.hpp"
#include "libraries/pico_graphics/pico_graphics.hpp"
#include "common/pimoroni_bus.hpp"

namespace pimoroni {

  class PicoGraphicsST7789 : public PicoGraphics_PenRGB565 {
  private:
    ST7789 st7789;

  public:
    PicoGraphicsST7789(uint16_t width, uint16_t height, Rotation rotation, bool round=false, void *frame_buffer=nullptr) :
      PicoGraphics_PenRGB565(width, height, frame_buffer),
      st7789(width, height, rotation, round, get_spi_pins(BG_SPI_FRONT)) {
              common_init();
           };

    PicoGraphicsST7789(uint16_t width, uint16_t height, Rotation rotation, bool round, void *frame_buffer, SPIPins bus_pins) :
      PicoGraphics_PenRGB565(width, height, frame_buffer),
      st7789(width, height, rotation, round, bus_pins) {
              common_init();
           };

    PicoGraphicsST7789(uint16_t width, uint16_t height, Rotation rotation, void *frame_buffer, ParallelPins bus_pins) :
      PicoGraphics_PenRGB565(width, height, frame_buffer),
      st7789(width, height, rotation, bus_pins) {
              common_init();
           };

    void common_init() {
      this->set_dimensions(st7789.width, st7789.height);
      st7789.update(this);
    }

    void update() {
      st7789.update(this);
    }

    void set_backlight(uint8_t brightness) {
      st7789.set_backlight(brightness);
    }

    void set_framebuffer(void* frame_buffer) {
      this->frame_buffer = frame_buffer;
    }
  };

}
