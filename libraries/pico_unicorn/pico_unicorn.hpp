#pragma once

#include "hardware/pio.h"

namespace pimoroni {

  class alignas(4) PicoUnicorn {
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
    PIO bitstream_pio = pio0;
    uint bitstream_sm = 0;
    uint sm_offset = 0;
    uint dma_channel;
    // must be aligned for 32bit dma transfer
    alignas(4) uint32_t buffer[BITSTREAM_LENGTH / 4] = {0};

  public:
    PicoUnicorn();
    ~PicoUnicorn();

    void dma_complete();

    void init();

    void clear();
    void set_pixel(uint8_t x, uint8_t y, uint8_t r, uint8_t g, uint8_t b);
    void set_pixel(uint8_t x, uint8_t y, uint8_t v);

    bool is_pressed(uint8_t button);
  };

}