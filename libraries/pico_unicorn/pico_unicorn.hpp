#pragma once

#include "hardware/pio.h"
#include "pico_graphics.hpp"

namespace pimoroni {

  class PicoUnicorn {
  public:
    static const int WIDTH = 16;
    static const int HEIGHT = 7;
    static const uint8_t A = 12;
    static const uint8_t B = 13;
    static const uint8_t X = 14;
    static const uint8_t Y = 15;

    static const uint32_t ROW_COUNT = 7;
    static const uint32_t ROW_BYTES = 12;
    static const uint32_t BCD_FRAMES = 15; // includes fet discharge frame
    static const uint32_t BITSTREAM_LENGTH = (ROW_COUNT * ROW_BYTES * BCD_FRAMES);

  private:
    static PIO bitstream_pio;
    static uint bitstream_sm;
    static uint bitstream_sm_offset;
  
    // must be aligned for 32bit dma transfer
    alignas(4) uint8_t bitstream[BITSTREAM_LENGTH] = {0};
    const uint32_t bitstream_addr = (uint32_t)bitstream;
    static PicoUnicorn* unicorn;

  public:
    PicoUnicorn();
    ~PicoUnicorn();

    void init();

    void clear();
    void set_pixel(uint8_t x, uint8_t y, uint8_t r, uint8_t g, uint8_t b);
    void set_pixel(uint8_t x, uint8_t y, uint8_t v);

    bool is_pressed(uint8_t button);

    void update(PicoGraphics *graphics);
  private:
    void partial_teardown();
    void dma_safe_abort(uint channel);
  };

}