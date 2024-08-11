#include "hardware/dma.h"
#include "hardware/irq.h"
#include "common/pimoroni_common.hpp"

#ifndef NO_QSTR
#include "pico_unicorn.pio.h"
#endif
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

static uint32_t dma_channel;
static uint32_t dma_ctrl_channel;

namespace pimoroni {
  PicoUnicorn* PicoUnicorn::unicorn = nullptr;
  PIO PicoUnicorn::bitstream_pio = pio0;
  uint PicoUnicorn::bitstream_sm = 0;
  uint PicoUnicorn::bitstream_sm_offset = 0;

  PicoUnicorn::~PicoUnicorn() {
    if(unicorn == this) {
      partial_teardown();

      dma_channel_unclaim(dma_ctrl_channel); // This works now the teardown behaves correctly
      dma_channel_unclaim(dma_channel); // This works now the teardown behaves correctly
      pio_sm_unclaim(bitstream_pio, bitstream_sm);
      pio_remove_program(bitstream_pio, &unicorn_program, bitstream_sm_offset);

      unicorn = nullptr;
    }
  }

  void PicoUnicorn::partial_teardown() {
    // Stop the bitstream SM
    pio_sm_set_enabled(bitstream_pio, bitstream_sm, false);

    // Make sure the display is off and switch it to an invisible row, to be safe
    const uint pins_to_set = 0b1111111 << ROW_6;
    pio_sm_set_pins_with_mask(bitstream_pio, bitstream_sm, pins_to_set, pins_to_set);

    dma_hw->ch[dma_ctrl_channel].al1_ctrl = (dma_hw->ch[dma_ctrl_channel].al1_ctrl & ~DMA_CH0_CTRL_TRIG_CHAIN_TO_BITS) | (dma_ctrl_channel << DMA_CH0_CTRL_TRIG_CHAIN_TO_LSB);
    dma_hw->ch[dma_channel].al1_ctrl = (dma_hw->ch[dma_channel].al1_ctrl & ~DMA_CH0_CTRL_TRIG_CHAIN_TO_BITS) | (dma_channel << DMA_CH0_CTRL_TRIG_CHAIN_TO_LSB);
    // Abort any in-progress DMA transfer
    dma_safe_abort(dma_ctrl_channel);
    dma_safe_abort(dma_channel);
  }

  [[deprecated("Handled by constructor.")]]
  void PicoUnicorn::init() {
    return;
  }

  PicoUnicorn::PicoUnicorn() {
    if(unicorn != nullptr) {
      partial_teardown();
    }
  
    // setup pins
    gpio_init(pin::LED_DATA); gpio_set_dir(pin::LED_DATA, GPIO_OUT);
    gpio_init(pin::LED_CLOCK); gpio_set_dir(pin::LED_CLOCK, GPIO_OUT);
    gpio_init(pin::LED_LATCH); gpio_set_dir(pin::LED_LATCH, GPIO_OUT);
    gpio_init(pin::LED_BLANK); gpio_set_dir(pin::LED_BLANK, GPIO_OUT);

    gpio_init(pin::ROW_0); gpio_set_dir(pin::ROW_0, GPIO_OUT);
    gpio_init(pin::ROW_1); gpio_set_dir(pin::ROW_1, GPIO_OUT);
    gpio_init(pin::ROW_2); gpio_set_dir(pin::ROW_2, GPIO_OUT);
    gpio_init(pin::ROW_3); gpio_set_dir(pin::ROW_3, GPIO_OUT);
    gpio_init(pin::ROW_4); gpio_set_dir(pin::ROW_4, GPIO_OUT);
    gpio_init(pin::ROW_5); gpio_set_dir(pin::ROW_5, GPIO_OUT);
    gpio_init(pin::ROW_6); gpio_set_dir(pin::ROW_6, GPIO_OUT);

    // initialise the bcd timing values and row selects in the bitstream
    for(uint8_t row = 0; row < HEIGHT; row++) {
      for(uint8_t frame = 0; frame < BCD_FRAMES; frame++) {
        // determine offset in the buffer for this row/frame
        uint16_t offset = (row * ROW_BYTES * BCD_FRAMES) + (ROW_BYTES * frame);

        uint16_t row_select_offset = offset + 9;
        uint16_t bcd_offset = offset + 10;

        // the last bcd frame is used to allow the fets to discharge to avoid ghosting
        if(frame == BCD_FRAMES - 1) {
          bitstream[row_select_offset] = 0b11111111;

          uint16_t bcd_ticks = 65535;
          bitstream[bcd_offset + 1] = (bcd_ticks &  0xff00) >> 8;
          bitstream[bcd_offset]     = (bcd_ticks &  0xff);

          for(uint8_t col = 0; col < 6; col++) {
            bitstream[offset + col] = 0xff;
          }
        }else{
          uint8_t row_select_mask = ~(1 << (7 - row));
          bitstream[row_select_offset] = row_select_mask;

          uint16_t bcd_ticks = 1 << frame;
          bitstream[bcd_offset + 1] = (bcd_ticks &  0xff00) >> 8;
          bitstream[bcd_offset]     = (bcd_ticks &  0xff);
        }
      }
    }

    // setup button inputs
    gpio_set_function(pin::A, GPIO_FUNC_SIO); gpio_set_dir(pin::A, GPIO_IN); gpio_pull_up(pin::A);
    gpio_set_function(pin::B, GPIO_FUNC_SIO); gpio_set_dir(pin::B, GPIO_IN); gpio_pull_up(pin::B);
    gpio_set_function(pin::X, GPIO_FUNC_SIO); gpio_set_dir(pin::X, GPIO_IN); gpio_pull_up(pin::X);
    gpio_set_function(pin::Y, GPIO_FUNC_SIO); gpio_set_dir(pin::Y, GPIO_IN); gpio_pull_up(pin::Y);

    // setup the pio
    bitstream_pio = pio0;
    if(unicorn == nullptr) {
      bitstream_sm = pio_claim_unused_sm(bitstream_pio, true);
      bitstream_sm_offset = pio_add_program(bitstream_pio, &unicorn_program);
    }
  
    pio_gpio_init(bitstream_pio, pin::LED_DATA);
    pio_gpio_init(bitstream_pio, pin::LED_CLOCK);
    pio_gpio_init(bitstream_pio, pin::LED_LATCH);
    pio_gpio_init(bitstream_pio, pin::LED_BLANK);
    pio_gpio_init(bitstream_pio, pin::ROW_0);
    pio_gpio_init(bitstream_pio, pin::ROW_1);
    pio_gpio_init(bitstream_pio, pin::ROW_2);
    pio_gpio_init(bitstream_pio, pin::ROW_3);
    pio_gpio_init(bitstream_pio, pin::ROW_4);
    pio_gpio_init(bitstream_pio, pin::ROW_5);
    pio_gpio_init(bitstream_pio, pin::ROW_6);

    pio_sm_set_consecutive_pindirs(bitstream_pio, bitstream_sm, pin::LED_DATA, 4, true);
    pio_sm_set_consecutive_pindirs(bitstream_pio, bitstream_sm, pin::ROW_6, 7, true);

    pio_sm_config c = unicorn_program_get_default_config(bitstream_sm_offset);

    // osr shifts right, autopull on, autopull threshold 8
    sm_config_set_out_shift(&c, true, false, 32);

    // configure out, set, and sideset pins
    sm_config_set_out_pins(&c, pin::ROW_6, 7);
    sm_config_set_sideset_pins(&c, pin::LED_CLOCK);
    sm_config_set_set_pins(&c, pin::LED_DATA, 4);

    // join fifos as only tx needed (gives 8 deep fifo instead of 4)
    sm_config_set_fifo_join(&c, PIO_FIFO_JOIN_TX);

    // setup chained dma transfer for pixel data to the pio
    dma_channel = dma_claim_unused_channel(true);
    dma_ctrl_channel = dma_claim_unused_channel(true);
  
    dma_channel_config ctrl_config = dma_channel_get_default_config(dma_ctrl_channel);
    channel_config_set_transfer_data_size(&ctrl_config, DMA_SIZE_32);
    channel_config_set_read_increment(&ctrl_config, false);
    channel_config_set_write_increment(&ctrl_config, false);
    channel_config_set_chain_to(&ctrl_config, dma_channel);

    dma_channel_configure(
      dma_ctrl_channel,
      &ctrl_config,
      &dma_hw->ch[dma_channel].read_addr,
      &bitstream_addr,
      1,
      false
    );

    dma_channel_config config = dma_channel_get_default_config(dma_channel);
    channel_config_set_transfer_data_size(&config, DMA_SIZE_32);
    channel_config_set_bswap(&config, false); // byte swap to reverse little endian
    channel_config_set_dreq(&config, pio_get_dreq(bitstream_pio, bitstream_sm, true));
    channel_config_set_chain_to(&config, dma_ctrl_channel); 

    dma_channel_configure(
      dma_channel,
      &config,
      &bitstream_pio->txf[bitstream_sm],
      NULL,
      BITSTREAM_LENGTH / 4,
      false);

    pio_sm_init(bitstream_pio, bitstream_sm, bitstream_sm_offset, &c);
    pio_sm_set_enabled(bitstream_pio, bitstream_sm, true);

    // start the control channel
    dma_start_channel_mask(1u << dma_ctrl_channel);

    unicorn = this;
  }

  void PicoUnicorn::clear() {
    for(uint8_t y = 0; y < HEIGHT; y++) {
      for(uint8_t x = 0; x < WIDTH; x++) {
        set_pixel(x, y, 0);
      }
    }
  }

  void PicoUnicorn::set_pixel(uint8_t x, uint8_t y, uint8_t r, uint8_t g, uint8_t b) {
    if(x < 0 || x >= WIDTH || y < 0 || y >= HEIGHT) return;

    // make those coordinates sane
    x = (WIDTH - 1) - x;

    // work out the byte offset of this pixel
    uint8_t byte_offset = x / 2;

    // check if it's the high or low nibble and create mask and shift value
    uint8_t shift = x % 2 == 0 ? 0 : 4;
    uint8_t nibble_mask = 0b00001111 << shift;

    uint16_t gr = pimoroni::GAMMA_14BIT[r];
    uint16_t gg = pimoroni::GAMMA_14BIT[g];
    uint16_t gb = pimoroni::GAMMA_14BIT[b];

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

  void PicoUnicorn::dma_safe_abort(uint channel) {
    // Tear down the DMA channel.
    // This is copied from: https://github.com/raspberrypi/pico-sdk/pull/744/commits/5e0e8004dd790f0155426e6689a66e08a83cd9fc
    uint32_t irq0_save = dma_hw->inte0 & (1u << channel);
    hw_clear_bits(&dma_hw->inte0, irq0_save);

    dma_hw->abort = 1u << channel;

    // To fence off on in-flight transfers, the BUSY bit should be polled
    // rather than the ABORT bit, because the ABORT bit can clear prematurely.
    while (dma_hw->ch[channel].ctrl_trig & DMA_CH0_CTRL_TRIG_BUSY_BITS) tight_loop_contents();

    // Clear the interrupt (if any) and restore the interrupt masks.
    dma_hw->ints0 = 1u << channel;
    hw_set_bits(&dma_hw->inte0, irq0_save);
  }

  void PicoUnicorn::update(PicoGraphics *graphics) {
    if(unicorn == this) {
      if(graphics->pen_type == PicoGraphics::PEN_RGB888) {
        uint32_t *p = (uint32_t *)graphics->frame_buffer;

        for(int y = 0; y < HEIGHT; y++) {
          for(int x = 0; x < WIDTH; x++) {
            uint32_t col = *p;
            uint8_t r = (col & 0xff0000) >> 16;
            uint8_t g = (col & 0x00ff00) >>  8;
            uint8_t b = (col & 0x0000ff) >>  0;
            p++;

            set_pixel(x, y, r, g, b);
          }
        }
      }
      else if(graphics->pen_type == PicoGraphics::PEN_RGB565) {
        uint16_t *p = (uint16_t *)graphics->frame_buffer;
        for(int y = 0; y < HEIGHT; y++) {
          for(int x = 0; x < WIDTH; x++) {
            uint16_t col = __builtin_bswap16(*p);
            uint8_t r = (col & 0b1111100000000000) >> 8;
            uint8_t g = (col & 0b0000011111100000) >> 3;
            uint8_t b = (col & 0b0000000000011111) << 3;
            p++;

            set_pixel(x, y, r, g, b);
          }
        }
      }
      else if(graphics->pen_type == PicoGraphics::PEN_P8 || graphics->pen_type == PicoGraphics::PEN_P4) {
        int offset = 0;
        graphics->frame_convert(PicoGraphics::PEN_RGB888, [this, offset](void *data, size_t length) mutable {
          uint32_t *p = (uint32_t *)data;
          for(auto i = 0u; i < length / 4; i++) {
            int x = offset % WIDTH;
            int y = offset / WIDTH;

            uint32_t col = *p;
            uint8_t r = (col & 0xff0000) >> 16;
            uint8_t g = (col & 0x00ff00) >>  8;
            uint8_t b = (col & 0x0000ff) >>  0;

            set_pixel(x, y, r, g, b);
            offset++;
            p++;
          }
        });
      }
    }
  }

}
