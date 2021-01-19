#include <math.h>

#include "hardware/dma.h"
#include "hardware/irq.h"

#include "pico_unicorn.pio.h"
#include "pico_unicorn.hpp"

// pixel data is stored as a stream of bits delivered in the
// order the PIO needs to manage the shift registers, row
// selects, delays, and latching/blanking
//
// the data consists of 7 rows each of which has 14 frames of
// bcd timing data
//
// each row looks like this:
//
//      0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, # pixel data
//      0b00000000, # dummy byte to 32-bit align the frame (could be used to extend row select in future)
//      0b01111111, # row 0 select (7-bit row address, 1-bit dummy data)
//      0b00001111, 0b11111111, # bcd tick count (0-65536)
//
//      .. next BCD frame for this row (repeat for 14 frames)
//
//  .. next row (repeat for 7 rows)
//
// pixels are encoded as 4 bits: r, g, b, dummy to conveniently
// pack them into nibbles

enum pin {
  LED_DATA  = 8,
  LED_CLOCK = 9,
  LED_LATCH = 10,
  LED_BLANK = 11,
  ROW_0 = 22,
  ROW_1 = 21,
  ROW_2 = 20,
  ROW_3 = 19,
  ROW_4 = 18,
  ROW_5 = 17,
  ROW_6 = 16,
  A     = 12,
  B     = 13,
  X     = 14,
  Y     = 15,
};

constexpr uint32_t ROW_COUNT = 7;
constexpr uint32_t ROW_BYTES = 12;
constexpr uint32_t BCD_FRAMES = 15; // includes fet discharge frame
constexpr uint32_t BITSTREAM_LENGTH = (ROW_COUNT * ROW_BYTES * BCD_FRAMES);

uint8_t bitstream[BITSTREAM_LENGTH] = {0};

uint16_t r_gamma_lut[256] = {0};
uint16_t g_gamma_lut[256] = {0};
uint16_t b_gamma_lut[256] = {0};

uint32_t dma_channel;

static inline void unicorn_jetpack_program_init(PIO pio, uint sm, uint offset) {
  pio_gpio_init(pio, pin::LED_DATA);
  pio_gpio_init(pio, pin::LED_CLOCK);
  pio_gpio_init(pio, pin::LED_LATCH);
  pio_gpio_init(pio, pin::LED_BLANK);
  pio_gpio_init(pio, pin::ROW_0);
  pio_gpio_init(pio, pin::ROW_1);
  pio_gpio_init(pio, pin::ROW_2);
  pio_gpio_init(pio, pin::ROW_3);
  pio_gpio_init(pio, pin::ROW_4);
  pio_gpio_init(pio, pin::ROW_5);
  pio_gpio_init(pio, pin::ROW_6);

  pio_sm_set_consecutive_pindirs(pio, sm, pin::LED_DATA, 4, true);
  pio_sm_set_consecutive_pindirs(pio, sm, pin::ROW_6, 7, true);

  pio_sm_config c = unicorn_program_get_default_config(offset);

  // osr shifts right, autopull on, autopull threshold 8
  sm_config_set_out_shift(&c, true, false, 32);

  // configure out, set, and sideset pins
  sm_config_set_out_pins(&c, pin::ROW_6, 7);
  sm_config_set_sideset_pins(&c, pin::LED_CLOCK);
  sm_config_set_set_pins(&c, pin::LED_DATA, 4);

  // join fifos as only tx needed (gives 8 deep fifo instead of 4)
  sm_config_set_fifo_join(&c, PIO_FIFO_JOIN_TX);

  // set clock divider
  //sm_config_set_clkdiv(&c, 4);

  pio_sm_init(pio, sm, offset, &c);
  pio_sm_set_enabled(pio, sm, true);
}

namespace pimoroni {

  // once the dma transfer of the scanline is complete we move to the
  // next scanline (or quit if we're finished)
  void __isr dma_complete() {
    if (dma_hw->ints0 & (1u << dma_channel)) {
      dma_hw->ints0 = (1u << dma_channel); // clear irq flag
      dma_channel_set_trans_count(dma_channel, BITSTREAM_LENGTH / 4, false);
      dma_channel_set_read_addr(dma_channel, bitstream, true);

      static bool hilo = false;
      gpio_put(2, hilo);
      hilo = !hilo;
    }
  }


  void PicoUnicorn::init() {
    // setup pins
    gpio_init(pin::LED_DATA); gpio_set_dir(pin::LED_DATA, GPIO_OUT);
    gpio_init(pin::LED_CLOCK); gpio_set_dir(pin::LED_CLOCK, GPIO_OUT);
    gpio_init(pin::LED_LATCH); gpio_set_dir(pin::LED_LATCH, GPIO_OUT);
    gpio_init(pin::LED_BLANK); gpio_set_dir(pin::LED_BLANK, GPIO_OUT);

    gpio_init(2); gpio_set_dir(2, GPIO_OUT);

    gpio_init(pin::ROW_0); gpio_set_dir(pin::ROW_0, GPIO_OUT);
    gpio_init(pin::ROW_1); gpio_set_dir(pin::ROW_1, GPIO_OUT);
    gpio_init(pin::ROW_2); gpio_set_dir(pin::ROW_2, GPIO_OUT);
    gpio_init(pin::ROW_3); gpio_set_dir(pin::ROW_3, GPIO_OUT);
    gpio_init(pin::ROW_4); gpio_set_dir(pin::ROW_4, GPIO_OUT);
    gpio_init(pin::ROW_5); gpio_set_dir(pin::ROW_5, GPIO_OUT);
    gpio_init(pin::ROW_6); gpio_set_dir(pin::ROW_6, GPIO_OUT);

    // create 14-bit gamma luts
    for(uint16_t v = 0; v < 256; v++) {
      // gamma correct the provided 0-255 brightness value onto a
      // 0-65535 range for the pwm counter
      float r_gamma = 2.8f;
      r_gamma_lut[v] = (uint16_t)(pow((float)(v) / 255.0f, r_gamma) * 16383.0f + 0.5f);

      float g_gamma = 3.5f;
      g_gamma_lut[v] = (uint16_t)(pow((float)(v) / 255.0f, g_gamma) * 16383.0f + 0.5f);

      float b_gamma = 2.6f;
      b_gamma_lut[v] = (uint16_t)(pow((float)(v) / 255.0f, b_gamma) * 16383.0f + 0.5f);
    }

    // initialise the bcd timing values and row selects in the bitstream
    for(uint8_t row = 0; row < 7; row++) {
      for(uint8_t frame = 0; frame < BCD_FRAMES; frame++) {
        // determine offset in the buffer for this row/frame
        uint16_t offset = (row * ROW_BYTES * BCD_FRAMES) + (ROW_BYTES * frame);

        uint16_t row_select_offset = offset + 9;
        uint16_t bcd_offset = offset + 10;

        // the last bcd frame is used to allow the fets to discharge to avoid ghosting
        if(frame == BCD_FRAMES - 1) {
          bitstream[row_select_offset] = 0b01111111;

          uint16_t bcd_ticks = 65535;
          bitstream[bcd_offset + 1] = (bcd_ticks &  0xff00) >> 8;
          bitstream[bcd_offset]     = (bcd_ticks &  0xff);
        }else{
          uint8_t row_select_mask = ~(1 << (7 - row));
          bitstream[row_select_offset] = row_select_mask;

          uint16_t bcd_ticks = pow(2, frame);
          bitstream[bcd_offset + 1] = (bcd_ticks &  0xff00) >> 8;
          bitstream[bcd_offset]     = (bcd_ticks &  0xff);
        }
      }
    }

    // setup the pio
    bitstream_pio = pio0;
    bitstream_sm = 0;
    uint offset = pio_add_program(bitstream_pio, &unicorn_program);
    unicorn_jetpack_program_init(bitstream_pio, bitstream_sm, offset);

    // setup button inputs
    gpio_set_function(pin::A, GPIO_FUNC_SIO); gpio_set_dir(pin::A, GPIO_IN); gpio_pull_up(pin::A);
    gpio_set_function(pin::B, GPIO_FUNC_SIO); gpio_set_dir(pin::B, GPIO_IN); gpio_pull_up(pin::B);
    gpio_set_function(pin::X, GPIO_FUNC_SIO); gpio_set_dir(pin::X, GPIO_IN); gpio_pull_up(pin::X);
    gpio_set_function(pin::Y, GPIO_FUNC_SIO); gpio_set_dir(pin::Y, GPIO_IN); gpio_pull_up(pin::Y);

    // setup dma transfer for pixel data to the pio
    dma_channel = dma_claim_unused_channel(true);
    dma_channel_config config = dma_channel_get_default_config(dma_channel);
    channel_config_set_transfer_data_size(&config, DMA_SIZE_32);
    channel_config_set_bswap(&config, false); // byte swap to reverse little endian
    channel_config_set_dreq(&config, pio_get_dreq(bitstream_pio, bitstream_sm, true));
    dma_channel_configure(dma_channel, &config, &bitstream_pio->txf[bitstream_sm], NULL, 0, false);
    dma_channel_set_irq0_enabled(dma_channel, true);
    irq_set_enabled(pio_get_dreq(bitstream_pio, bitstream_sm, true), true);
    irq_set_exclusive_handler(DMA_IRQ_0, dma_complete);
    irq_set_enabled(DMA_IRQ_0, true);

    dma_channel_set_trans_count(dma_channel, BITSTREAM_LENGTH / 4, false);
    dma_channel_set_read_addr(dma_channel, bitstream, true);
  }

  void PicoUnicorn::clear() {
    for(uint8_t y = 0; y < 7; y++) {
      for(uint8_t x = 0; x < 16; x++) {
        set_pixel(x, y, 0);
      }
    }
  }

  void PicoUnicorn::set_pixel(uint8_t x, uint8_t y, uint8_t r, uint8_t g, uint8_t b) {
    if(x < 0 || x > 15 || y < 0 || y > 6) return;

    // make those coordinates sane
    x = 15 - x;

    // work out the byte offset of this pixel
    uint8_t byte_offset = x / 2;

    // check if it's the high or low nibble and create mask and shift value
    uint8_t shift = x % 2 == 0 ? 0 : 4;
    uint8_t nibble_mask = 0b00001111 << shift;

    uint16_t gr = r_gamma_lut[r];
    uint16_t gg = g_gamma_lut[g];
    uint16_t gb = b_gamma_lut[b];

    // set the appropriate bits in the separate bcd frames
    for(uint8_t frame = 0; frame < BCD_FRAMES; frame++) {
      // determine offset in the buffer for this row/frame
      uint16_t offset = (y * ROW_BYTES * BCD_FRAMES) + (ROW_BYTES * frame);

      uint8_t rgbd = ((gr & 0b1) << 1) | ((gg & 0b1) << 3) | ((gb & 0b1) << 2);

      // shift to correct nibble
      rgbd <<= shift;

      // clear existing data
      bitstream[offset + byte_offset] &= ~nibble_mask;

      // set new data
      bitstream[offset + byte_offset] |= rgbd;

      gr >>= 1;
      gg >>= 1;
      gb >>= 1;
    }
  }

  void PicoUnicorn::set_pixel(uint8_t x, uint8_t y, uint8_t v) {
    set_pixel(x, y, v, v, v);
  }

  bool PicoUnicorn::is_pressed(uint8_t button) {
    return !gpio_get(button);
  }

}