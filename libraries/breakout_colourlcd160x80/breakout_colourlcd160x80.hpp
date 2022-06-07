#pragma once

#include "drivers/st7735/st7735.hpp"
#include "libraries/pico_graphics/pico_graphics.hpp"
#include "common/pimoroni_common.hpp"
#include "common/pimoroni_bus.hpp"

namespace pimoroni {

  class BreakoutColourLCD160x80 : public PicoGraphics<PenRGB565> {
    //--------------------------------------------------
    // Constants
    //--------------------------------------------------
  public:
    static const int WIDTH            = 160;
    static const int HEIGHT           = 80;

    //--------------------------------------------------
    // Variables
    //--------------------------------------------------
  private:
    ST7735 screen;


    //--------------------------------------------------
    // Constructors/Destructor
    //--------------------------------------------------
  public:

    BreakoutColourLCD160x80(void *frame_buffer)
      : BreakoutColourLCD160x80(frame_buffer, get_spi_pins(BG_SPI_FRONT)) {
    }

    BreakoutColourLCD160x80(void *frame_buffer, SPIPins bus_pins)
      : PicoGraphics<PenRGB565>(WIDTH, HEIGHT, frame_buffer), screen(WIDTH, HEIGHT, bus_pins){
    }

    void update() {
      screen.update(this);
    }

    void set_backlight(uint8_t brightness) {
      screen.set_backlight(brightness);
    }
  };

}
