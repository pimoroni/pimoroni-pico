#pragma once

#include "drivers/st7789/st7789.hpp"
#include "libraries/pico_graphics/pico_graphics.hpp"
#include "common/pimoroni_bus.hpp"

namespace pimoroni {

  class ST7789Generic : public PicoGraphics {
  private:
    ST7789 st7789;

  public:
    ST7789Generic(uint16_t width, uint16_t height, Rotation rotation, bool round=false, void *frame_buffer=nullptr) :
      PicoGraphics(width, height, frame_buffer),
      st7789(width, height, rotation, round, frame_buffer, get_spi_pins(BG_SPI_FRONT)) {
              common_init();
           };

    ST7789Generic(uint16_t width, uint16_t height, Rotation rotation, bool round, void *frame_buffer, SPIPins bus_pins) :
      PicoGraphics(width, height, frame_buffer),
      st7789(width, height, rotation, round, frame_buffer, bus_pins) {
              common_init();
           };

    ST7789Generic(uint16_t width, uint16_t height, Rotation rotation, void *frame_buffer, ParallelPins bus_pins) :
      PicoGraphics(width, height, frame_buffer),
      st7789(width, height, rotation, frame_buffer, bus_pins) {
              common_init();
           };

    void common_init() {
      this->frame_buffer = (Pen *)st7789.frame_buffer;
      this->st7789.init();
      this->set_dimensions(this->st7789.width, this->st7789.height);
      this->st7789.update(palette);
    }

    void update();
    void set_backlight(uint8_t brightness);
    void configure_display(bool rotate180);
    void set_framebuffer(void* frame_buffer) {
      this->frame_buffer = (Pen *)frame_buffer;
      st7789.frame_buffer = frame_buffer;
    }
  };

}
