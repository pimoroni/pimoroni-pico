#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/dma.h"
#include "hardware/irq.h"
#include "unicorn.pio.h"

#define BLINK_PIN 25

#define LED_DATA  8
#define LED_CLOCK 9
#define LED_LATCH 10
#define LED_BLANK 11

#define ROW_0     22
#define ROW_1     21
#define ROW_2     20
#define ROW_3     19
#define ROW_4     18
#define ROW_5     17
#define ROW_6     16

#define ROW_COUNT 7
#define ROW_BYTES 12
#define BCD_FRAMES 15
#define BITSTREAM_LENGTH (ROW_COUNT * ROW_BYTES * BCD_FRAMES)

uint32_t         dma_channel;

static inline void unicorn_jetpack_program_init(PIO pio, uint sm, uint offset) {
  pio_gpio_select(pio, LED_DATA);
  pio_gpio_select(pio, LED_CLOCK);
  pio_gpio_select(pio, LED_LATCH);
  pio_gpio_select(pio, LED_BLANK);
  pio_gpio_select(pio, ROW_0);
  pio_gpio_select(pio, ROW_1);
  pio_gpio_select(pio, ROW_2);
  pio_gpio_select(pio, ROW_3);
  pio_gpio_select(pio, ROW_4);
  pio_gpio_select(pio, ROW_5);
  pio_gpio_select(pio, ROW_6);

  pio_sm_set_consecutive_pindirs(pio, sm, LED_DATA, 4, true);
  pio_sm_set_consecutive_pindirs(pio, sm, ROW_6, 7, true);

  pio_sm_config c = unicorn_program_get_default_config(offset);

  // osr shifts right, autopull on, autopull threshold 8
  sm_config_set_out_shift(&c, true, true, 32);

  // configure out, set, and sideset pins
  sm_config_set_out_pins(&c, ROW_6, 7);
  sm_config_set_sideset_pins(&c, LED_CLOCK);
  sm_config_set_set_pins(&c, LED_DATA, 4);

  // join fifos as only tx needed (gives 8 deep fifo instead of 4)
  sm_config_set_fifo_join(&c, PIO_FIFO_JOIN_TX);

  // set clock divider
  //sm_config_set_clkdiv(&c, 4);

  pio_sm_init(pio, sm, offset, &c);
  pio_sm_enable(pio, sm, true);
}

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
//      0b01111111, # row 0 select (7-bit row address, 1-bit dummy data)
//      0b00001111, 0b11111111, # bcd tick count (0-65536)
//      .. next BCD frame for this row (repeat for 8 frames)
//
//  .. next row (repeat for 7 rows)
//
// pixels are encoded as 4 bits: r, g, b, dummy to conveniently
// pack them into nibbles

uint8_t bitstream[BITSTREAM_LENGTH] = {0};
uint16_t r_gamma_lut[256] = {0};
uint16_t g_gamma_lut[256] = {0};
uint16_t b_gamma_lut[256] = {0};

void set_pixel(uint8_t x, uint8_t y, uint8_t r, uint8_t g, uint8_t b) {
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

void from_hsv(float h, float s, float v, uint8_t &r, uint8_t &g, uint8_t &b) {
    float i = floor(h * 6.0f);
    float f = h * 6.0f - i;
    v *= 255.0f;
    uint8_t p = v * (1.0f - s);
    uint8_t q = v * (1.0f - f * s);
    uint8_t t = v * (1.0f - (1.0f - f) * s);

    switch (int(i) % 6) {
      case 0: r = v; g = t; b = p; break;
      case 1: r = q; g = v; b = p; break;
      case 2: r = p; g = v; b = t; break;
      case 3: r = p; g = q; b = v; break;
      case 4: r = t; g = p; b = v; break;
      case 5: r = v; g = p; b = q; break;
    }
  }


int main() {
  printf("Unicorn PIO + DMA example\n");

  gpio_init(BLINK_PIN); gpio_set_dir(BLINK_PIN, GPIO_OUT);

  // setup pins
  gpio_init(LED_DATA); gpio_set_dir(LED_DATA, GPIO_OUT);
  gpio_init(LED_CLOCK); gpio_set_dir(LED_CLOCK, GPIO_OUT);
  gpio_init(LED_LATCH); gpio_set_dir(LED_LATCH, GPIO_OUT);
  gpio_init(LED_BLANK); gpio_set_dir(LED_BLANK, GPIO_OUT);

  gpio_init(ROW_0); gpio_set_dir(ROW_0, GPIO_OUT);
  gpio_init(ROW_1); gpio_set_dir(ROW_1, GPIO_OUT);
  gpio_init(ROW_2); gpio_set_dir(ROW_2, GPIO_OUT);
  gpio_init(ROW_3); gpio_set_dir(ROW_3, GPIO_OUT);
  gpio_init(ROW_4); gpio_set_dir(ROW_4, GPIO_OUT);
  gpio_init(ROW_5); gpio_set_dir(ROW_5, GPIO_OUT);
  gpio_init(ROW_6); gpio_set_dir(ROW_6, GPIO_OUT);


  // each row looks like this:
  //
  //      0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, # pixel data
  //      0b01111111, # row 0 select (7-bit row address, 1-bit dummy data)
  //      0b00001111, 0b00001111, # bcd tick count (0-65536)
  //      .. next BCD frame for this row (repeat for 8 frames)
  //
  //  .. next row (repeat for 7 rows)

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

  // initialise the BCD timing values and row selects
  for(uint8_t row = 0; row < 7; row++) {
    for(uint8_t frame = 0; frame < BCD_FRAMES; frame++) {
      // determine offset in the buffer for this row/frame
      uint16_t offset = (row * ROW_BYTES * BCD_FRAMES) + (ROW_BYTES * frame);

      uint16_t row_select_offset = offset + 8;

      // the last BCD frame is used to allow the fets to discharge to avoid ghosting
      if(frame == BCD_FRAMES - 1) {
        bitstream[row_select_offset] = 0b01111111;

        uint16_t bcd_offset = offset + 9;
        uint16_t bcd_ticks = 65535;
        bitstream[bcd_offset + 1] = (bcd_ticks &  0xff00) >> 8;
        bitstream[bcd_offset]     = (bcd_ticks &  0xff);
      }else{
        uint8_t row_select_mask = ~(1 << (7 - row));
        bitstream[row_select_offset] = row_select_mask;

        uint16_t bcd_offset = offset + 9;
        uint16_t bcd_ticks = pow(2, frame);
        bitstream[bcd_offset + 1] = (bcd_ticks &  0xff00) >> 8;
        bitstream[bcd_offset]     = (bcd_ticks &  0xff);
      }
    }
  }

  //set_pixel(4, 1, 20, 20, 20);
  //set_pixel(6, 2, 20, 20, 20);
  //set_pixel(8, 3, 20, 20, 20);
  //set_pixel(10, 4, 20, 20, 20);
  //set_pixel(12, 5, 20, 20, 20);
  //set_pixel(14, 6, 20, 20, 20);

  // setup the pio
  PIO bitstream_pio = pio0;
  uint sm = 0;
  uint offset = pio_add_program(bitstream_pio, &unicorn_program);
  unicorn_jetpack_program_init(bitstream_pio, sm, offset);


  // initialise dma channel for transmitting pixel data to screen
  // via the pixel doubling pio - initially we configure it with no
  // source or transfer length since these need to be assigned for
  // each scaline

  // dma_channel = dma_claim_unused_channel(true);
  // dma_channel_config config = dma_channel_get_default_config(dma_channel);
  // channel_config_set_bswap(&config, true); // byte swap to reverse little endian
  // channel_config_set_dreq(&config, pio_get_dreq(bitstream_pio, sm, true));
  // dma_channel_configure(dma_channel, &config, &bitstream_pio->txf[sm], bitstream, BITSTREAM_LENGTH, false);
  // dma_channel_set_irq0_enabled(dma_channel, true);
  // irq_enable(pio_get_dreq(bitstream_pio, sm, true), true);

  //irq_set_exclusive_handler(DMA_IRQ_0, dma_complete);
  //irq_enable(DMA_IRQ_0, true);


  // printf("configure bitstream DMA channel\n");
  // grab some unused dma channels
  // const uint32_t bitstream_dma = dma_claim_unused_channel(true);
  // dma_channel_config c = dma_channel_get_default_config(bitstream_dma);
  // channel_config_set_transfer_data_size(&c, DMA_SIZE_32);
  // channel_config_set_dreq(&c, pio_get_dreq(bitstream_pio, sm, true));
  // dma_channel_configure(bitstream_dma, &c,
  //                       &bitstream_pio->txf[sm],       // write address
  //                       bitstream,                    // buffer
  //                       BITSTREAM_LENGTH,             // size of buffer
  //                       false);                       // don't auto start
  // dma_channel_set_irq0_enabled(bitstream_dma, true);
  // irq_enable(pio_get_dreq(bitstream_pio, sm, true), true);

  uint32_t i = 0;
  while(true) {
    i = i + 1;
    uint8_t j = 0;
    for(uint8_t y = 0; y < 7; y++) {
      for(uint8_t x = 0; x < 16; x++) {
        uint8_t r, g, b;
        float h = float(x) / 63.0f + float(i) / 500.0f;
        h = h - float(int(h));
        float s = 1.0f;//(sin(float(i) / 200.0f) * 0.5f) + 0.5f;
        float v = (float(y) / 8.0f) + 0.05f;
        from_hsv(h, s, v, r, g, b);

        set_pixel(x, y, r, g, b);
        j = j + 1;
      }
    }
/*
    set_pixel(0, 0, 100, 0, 0);
    set_pixel(1, 1, 0, 100, 0);*/
    //set_pixel(2, 2, 0, 0, 100);
/*
    set_pixel(3, 3, 100, 0, 0);
    set_pixel(4, 4, 0, 100, 0);
    set_pixel(5, 5, 0, 0, 100);

    set_pixel(6, 6, 100, 0, 0);
*/

    // dma_channel_start(dma_channel);
    // dma_channel_wait_for_finish_blocking(dma_channel);

    // copy data to pio tx fifo 4 bytes at a time
    uint32_t *p = (uint32_t *)bitstream;
    for(uint16_t i = 0; i < BITSTREAM_LENGTH; i+=4) {
      pio_sm_put_blocking(bitstream_pio, sm, *p++);
    }
  }

  printf("done\n");
  //dma_channel_unclaim(bitstream_dma);

  return 0;
}
