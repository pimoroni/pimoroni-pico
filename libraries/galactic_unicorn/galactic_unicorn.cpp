#include <math.h>

#include "hardware/dma.h"
#include "hardware/irq.h"

#include "galactic_unicorn.pio.h"
#include "galactic_unicorn.hpp"

// pixel data is stored as a stream of bits delivered in the
// order the PIO needs to manage the shift registers, row
// selects, delays, and latching/blanking
//
// the data consists of 11 rows each of which has 14 frames of
// bcd timing data
//
// bits are output in order:
//
//  ROW_CLEAR, ROW_DATA1, ROW_DATA0, LED_BLANK, LED_LATCH, LED_CLOCK, LED_DATA0, LED_DATA1
//
// the data is structured like this:
//
// loop through the eleven rows of the display...
//
//   1rr00000                     // set row select bit on rows 0 and 8 (side set the clock)
//   00000000 00000000 00000000   // dummy bytes to align to dwords
//
//   within this row we loop through the 14 bcd frames for this row...
//
//      0  -  161: 100100rr, 100101rr, 100100gg, 100101gg, 100100bb, 100101bb, ... x 27 # left+right half rgb pixel data doubled for clock pulses, keep BLANK high
//            162: 10011000  // LATCH pixel data
//            163: 10000000  // turn off BLANK to output pixel data - now at 164 bytes (41 dwords)
//      164 - 165: 00001111, 11111111, # bcd tick count (0-65536)
//            166: 10010000  // turn BLANK back on 
//            167: 00000000  // dummy byte to ensure dword aligned
//
//  .. and back to the start

/*
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
};*/

enum pin {
  LED_DATA1 = 12,
  LED_DATA0 = 13,
  LED_CLOCK = 14,
  LED_LATCH = 15,
  LED_BLANK = 16,

  ROW_DATA0 = 17,
  ROW_DATA1 = 18,
  ROW_CLEAR = 19,
  ROW_CLOCK = 20,
  
  SWITCH_A = 0,
  SWITCH_B = 1,
  SWITCH_C = 3,
  SWITCH_D = 6,
  SWITCH_E = 2,
  SWITCH_VOLUME_UP = 21,
  SWITCH_VOLUME_DOWN = 26,
  SWITCH_BRIGHTNESS_UP = 7,
  SWITCH_BRIGHTNESS_DOWN = 8
};



constexpr uint32_t ROW_COUNT = 11;
constexpr uint32_t ROW_BYTES = 4;
constexpr uint32_t ROW_FRAME_BYTES = 168;
constexpr uint32_t BCD_FRAMES = 15; // includes fet discharge frame
constexpr uint32_t BITSTREAM_LENGTH = (ROW_COUNT * ROW_BYTES + ROW_COUNT * ROW_FRAME_BYTES * BCD_FRAMES);

// must be aligned for 32bit dma transfer
alignas(4) static uint8_t bitstream[BITSTREAM_LENGTH] = {0};

static uint16_t r_gamma_lut[256] = {0};
static uint16_t g_gamma_lut[256] = {0};
static uint16_t b_gamma_lut[256] = {0};

static uint32_t dma_channel;

static inline void unicorn_jetpack_program_init(PIO pio, uint sm, uint offset) {
  pio_gpio_init(pio, pin::LED_DATA1);
  pio_gpio_init(pio, pin::LED_DATA0);
  pio_gpio_init(pio, pin::LED_CLOCK);
  pio_gpio_init(pio, pin::LED_LATCH);
  pio_gpio_init(pio, pin::LED_BLANK);

  pio_gpio_init(pio, pin::ROW_DATA0);
  pio_gpio_init(pio, pin::ROW_DATA1);
  pio_gpio_init(pio, pin::ROW_CLEAR);
  pio_gpio_init(pio, pin::ROW_CLOCK);

  pio_sm_set_consecutive_pindirs(pio, sm, pin::LED_DATA1, 5, true);
  pio_sm_set_consecutive_pindirs(pio, sm, pin::ROW_DATA0, 4, true);

  pio_sm_config c = galactic_unicorn_program_get_default_config(offset);

  // osr shifts right, autopull on, autopull threshold 8
  sm_config_set_out_shift(&c, true, false, 32);

  // configure out, set, and sideset pins
  sm_config_set_out_pins(&c, pin::LED_DATA1, 8);
  sm_config_set_sideset_pins(&c, pin::ROW_CLOCK);

  // join fifos as only tx needed (gives 8 deep fifo instead of 4)
  sm_config_set_fifo_join(&c, PIO_FIFO_JOIN_TX);

  pio_sm_init(pio, sm, offset, &c);
  pio_sm_set_enabled(pio, sm, true);
  ///pio_sm_set_clkdiv(pio, sm, 4.0f);
}

namespace pimoroni {

  // once the dma transfer of the scanline is complete we move to the
  // next scanline (or quit if we're finished)
  void __isr dma_complete() {
    if (dma_hw->ints0 & (1u << dma_channel)) {
      dma_hw->ints0 = (1u << dma_channel); // clear irq flag
      dma_channel_set_trans_count(dma_channel, BITSTREAM_LENGTH / 4, false);
      dma_channel_set_read_addr(dma_channel, bitstream, true);
    }
  }

  GalacticUnicorn::~GalacticUnicorn() {
    // stop and release the dma channel
    irq_set_enabled(DMA_IRQ_0, false);
    dma_channel_set_irq0_enabled(dma_channel, false);
    irq_set_enabled(pio_get_dreq(bitstream_pio, bitstream_sm, true), false);
    irq_remove_handler(DMA_IRQ_0, dma_complete);

    dma_channel_wait_for_finish_blocking(dma_channel);
    dma_channel_unclaim(dma_channel);

    // release the pio and sm
    pio_sm_unclaim(bitstream_pio, bitstream_sm);
    pio_clear_instruction_memory(bitstream_pio);
    pio_sm_restart(bitstream_pio, bitstream_sm);
  }

  void GalacticUnicorn::init() {
    // todo: shouldn't need to do this if things were cleaned up properly but without
    // this any attempt to run a micropython script twice will fail
    static bool already_init = false;

    // setup pins
    gpio_init(pin::LED_DATA0); gpio_set_dir(pin::LED_DATA0, GPIO_OUT);
    gpio_init(pin::LED_DATA1); gpio_set_dir(pin::LED_DATA1, GPIO_OUT);
    gpio_init(pin::LED_CLOCK); gpio_set_dir(pin::LED_CLOCK, GPIO_OUT);
    gpio_init(pin::LED_LATCH); gpio_set_dir(pin::LED_LATCH, GPIO_OUT);
    gpio_init(pin::LED_BLANK); gpio_set_dir(pin::LED_BLANK, GPIO_OUT);

    gpio_init(pin::ROW_DATA0); gpio_set_dir(pin::ROW_DATA0, GPIO_OUT);
    gpio_init(pin::ROW_DATA1); gpio_set_dir(pin::ROW_DATA1, GPIO_OUT);
    gpio_init(pin::ROW_CLEAR); gpio_set_dir(pin::ROW_CLEAR, GPIO_OUT);
    gpio_init(pin::ROW_CLOCK); gpio_set_dir(pin::ROW_CLOCK, GPIO_OUT);

    // create 14-bit gamma luts
    for(uint16_t v = 0; v < 256; v++) {
      // gamma correct the provided 0-255 brightness value onto a
      // 0-65535 range for the pwm counter
      float r_gamma = 2.8f;
      r_gamma_lut[v] = (uint16_t)(powf((float)(v) / 255.0f, r_gamma) * 16383.0f + 0.5f);

      float g_gamma = 2.8f;
      g_gamma_lut[v] = (uint16_t)(powf((float)(v) / 255.0f, g_gamma) * 16383.0f + 0.5f);

      float b_gamma = 2.8f;
      b_gamma_lut[v] = (uint16_t)(powf((float)(v) / 255.0f, b_gamma) * 16383.0f + 0.5f);
    }

// the data is structured like this:
//
// loop through the eleven rows of the display...
//
//   1rr00000                     // set row select bit on rows 0 and 8 (side set the clock)
//   00000000 00000000 00000000   // dummy bytes to align to dwords
//
//   within this row we loop through the 14 bcd frames for this row...
//
//      0  -  161: 100100rr, 100101rr, 100100gg, 100101gg, 100100bb, 100101bb, ... x 27 # left+right half rgb pixel data doubled for clock pulses, keep BLANK high
//            162: 10011000  // LATCH pixel data
//            163: 10000000  // turn off BLANK to output pixel data - now at 164 bytes (41 dwords)
//      164 - 165: 00001111, 11111111, # bcd tick count (0-65536)
//            166: 10010000  // turn BLANK back on 
//            167: 00000000  // dummy byte to ensure dword aligned
//
//  .. and back to the start

    // initialise the bcd timing values and row selects in the bitstream
    for(uint8_t row = 0; row < HEIGHT; row++) {
      uint16_t row_offset = row * (ROW_BYTES + ROW_FRAME_BYTES * BCD_FRAMES);

      // setup row select on rows 0 and 8
      uint8_t row_select = row == 0 ? 0b10111000 : (row == 8 ? 0b11011000 : 0b10011000);
      bitstream[row_offset + 0] = row_select;

      for(uint8_t frame = 0; frame < BCD_FRAMES; frame++) {
        uint16_t frame_offset = row_offset + ROW_BYTES + (ROW_FRAME_BYTES * frame);

        bitstream[frame_offset + 162] = 0b10011000;  // LATCH pixel data
        bitstream[frame_offset + 163] = 0b10001000;  // BLANK low to enable column outputs

        // set the number of bcd ticks for this frame
        uint16_t bcd_ticks = frame == BCD_FRAMES - 1 ? 1 : 1 << frame;
        bitstream[frame_offset + 164] = (bcd_ticks &  0xff);
        bitstream[frame_offset + 165] = (bcd_ticks &  0xff00) >> 8;

        bitstream[frame_offset + 166] = 0b10010000;  // BLANK high again to disable outputs

        // setup empty pixels with BLANK high and a clock pulse
        for(uint8_t col = 0; col < 162; col += 2) {
          bitstream[frame_offset + col + 0] = 0b10010000;
          bitstream[frame_offset + col + 1] = 0b10010100;
        }

/*
        uint16_t row_select_offset = offset + 164;
        uint16_t bcd_offset = offset + 165;

        // the last bcd frame is used to allow the fets to discharge to avoid ghosting
        if(frame == BCD_FRAMES - 1) {
          uint16_t bcd_ticks = 65535;
          bitstream[bcd_offset + 1] = (bcd_ticks &  0xff00) >> 8;
          bitstream[bcd_offset]     = (bcd_ticks &  0xff);
        }else{
          uint8_t row_select = row == 0 ? 0b01000000 : (row == 8 ? 0b00100000 : 0b00000000);
          bitstream[row_select_offset] = row_select;
          
          uint16_t bcd_ticks = 1 << frame;
          bitstream[bcd_offset + 1] = (bcd_ticks &  0xff00) >> 8;
          bitstream[bcd_offset]     = (bcd_ticks &  0xff);
        }*/
      }
/*
      for(size_t i = 0; i < sizeof(bitstream); i++) {
        bitstream[i] = 0b11100000;
      }*/
    }

    // setup button inputs
    gpio_set_function(pin::SWITCH_A, GPIO_FUNC_SIO); gpio_set_dir(pin::SWITCH_A, GPIO_IN); gpio_pull_up(pin::SWITCH_A);
    gpio_set_function(pin::SWITCH_B, GPIO_FUNC_SIO); gpio_set_dir(pin::SWITCH_B, GPIO_IN); gpio_pull_up(pin::SWITCH_B);
    gpio_set_function(pin::SWITCH_C, GPIO_FUNC_SIO); gpio_set_dir(pin::SWITCH_C, GPIO_IN); gpio_pull_up(pin::SWITCH_C);
    gpio_set_function(pin::SWITCH_D, GPIO_FUNC_SIO); gpio_set_dir(pin::SWITCH_D, GPIO_IN); gpio_pull_up(pin::SWITCH_D);
    gpio_set_function(pin::SWITCH_E, GPIO_FUNC_SIO); gpio_set_dir(pin::SWITCH_E, GPIO_IN); gpio_pull_up(pin::SWITCH_E);

    gpio_set_function(pin::SWITCH_BRIGHTNESS_UP, GPIO_FUNC_SIO); gpio_set_dir(pin::SWITCH_BRIGHTNESS_UP, GPIO_IN); gpio_pull_up(pin::SWITCH_BRIGHTNESS_UP);
    gpio_set_function(pin::SWITCH_BRIGHTNESS_DOWN, GPIO_FUNC_SIO); gpio_set_dir(pin::SWITCH_BRIGHTNESS_DOWN, GPIO_IN); gpio_pull_up(pin::SWITCH_BRIGHTNESS_DOWN);

    gpio_set_function(pin::SWITCH_VOLUME_UP, GPIO_FUNC_SIO); gpio_set_dir(pin::SWITCH_VOLUME_UP, GPIO_IN); gpio_pull_up(pin::SWITCH_VOLUME_UP);
    gpio_set_function(pin::SWITCH_VOLUME_DOWN, GPIO_FUNC_SIO); gpio_set_dir(pin::SWITCH_VOLUME_DOWN, GPIO_IN); gpio_pull_up(pin::SWITCH_VOLUME_DOWN);

    if(already_init) {
      // stop and release the dma channel
      irq_set_enabled(DMA_IRQ_0, false);
      dma_channel_abort(dma_channel);
      dma_channel_wait_for_finish_blocking(dma_channel);

      dma_channel_set_irq0_enabled(dma_channel, false);
      irq_set_enabled(pio_get_dreq(bitstream_pio, bitstream_sm, true), false);
      irq_remove_handler(DMA_IRQ_0, dma_complete);

      dma_channel_unclaim(dma_channel);

      // release the pio and sm
      pio_sm_unclaim(bitstream_pio, bitstream_sm);
      pio_clear_instruction_memory(bitstream_pio);
      pio_sm_restart(bitstream_pio, bitstream_sm);
      //return;
    }

    // setup the pio
    bitstream_pio = pio0;
    bitstream_sm = pio_claim_unused_sm(pio0, true);
    sm_offset = pio_add_program(bitstream_pio, &galactic_unicorn_program);
    unicorn_jetpack_program_init(bitstream_pio, bitstream_sm, sm_offset);

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

    already_init = true;
  }

  void GalacticUnicorn::clear() {
    for(uint8_t y = 0; y < HEIGHT; y++) {
      for(uint8_t x = 0; x < WIDTH; x++) {
        set_pixel(x, y, 0);
      }
    }
  }

  void GalacticUnicorn::set_pixel(int x, int y, uint8_t r, uint8_t g, uint8_t b) {
    

    if(x < 0 || x >= WIDTH || y < 0 || y >= HEIGHT) return;

    // make those coordinates sane
    x = (WIDTH - 1) - x;
    y = (HEIGHT - 1) - y;

    // determine offset in the buffer for this row
    uint16_t row_offset = y * (ROW_BYTES + ROW_FRAME_BYTES * BCD_FRAMES);

    uint16_t bits[3] = {r_gamma_lut[b], g_gamma_lut[g], b_gamma_lut[r]};
    //uint16_t gr = r_gamma_lut[r];
    //uint16_t gg = g_gamma_lut[g];
    //uint16_t gb = b_gamma_lut[b];

    // set the appropriate bits in the separate bcd frames
    for(uint8_t frame = 0; frame < BCD_FRAMES; frame++) {
      uint16_t frame_offset = (ROW_FRAME_BYTES * frame) + 4;
      uint16_t offset = row_offset + frame_offset;// + byte_offset;

// loop through the eleven rows of the display...
//
//   1rr00000                     // set row select bit on rows 0 and 8 (side set the clock)
//   00000000 00000000 00000000   // dummy bytes to align to dwords
//
//   within this row we loop through the 14 bcd frames for this row...
//
//      0  -  161: 100100rr, 100101rr, 100100gg, 100101gg, 100100bb, 100101bb, ... x 27 # left+right half rgb pixel data doubled for clock pulses, keep BLANK high
//            162: 10011000  // LATCH pixel data
//            163: 10000000  // turn off BLANK to output pixel data - now at 164 bytes (41 dwords)
//      164 - 165: 00001111, 11111111, # bcd tick count (0-65536)
//            166: 10010000  // turn BLANK back on 
//            167: 00000000  // dummy byte to ensure dword aligned
//
//  .. and back to the start

      // work out the byte offset of this pixel
      /*if(bit_offset >= 160) {
        bit_offset -= 160;
      }*/

      for(int bit = 0; bit < 3; bit++) {
        int16_t bit_offset = x * 6 + 4 + (bit * 2);
        

        uint8_t bit_position = bit_offset >= 160 ? 1 : 0;
        uint8_t mask = 0b1 << bit_position;
        uint8_t value = (bits[bit] & 0b1) << bit_position;
        
        bitstream[offset + (bit_offset % 160) + 0] &= ~mask;
        bitstream[offset + (bit_offset % 160) + 0] |= value;
        bitstream[offset + (bit_offset % 160) + 1] &= ~mask;
        bitstream[offset + (bit_offset % 160) + 1] |= value;

        //bit_offset += 2;
        bits[bit] >>= 1;
      }
    }
  }

  void GalacticUnicorn::set_pixel(int x, int y, uint8_t v) {
    set_pixel(x, y, v, v, v);
  }

  bool GalacticUnicorn::is_pressed(uint8_t button) {
    return !gpio_get(button);
  }

}
