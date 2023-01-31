#include <math.h>

#include "hardware/dma.h"
#include "hardware/irq.h"
#include "hardware/adc.h"
#include "hardware/clocks.h"


#include "galactic_unicorn.pio.h"
#include "audio_i2s.pio.h"

#include "galactic_unicorn.hpp"

// pixel data is stored as a stream of bits delivered in the
// order the PIO needs to manage the shift registers, row
// selects, delays, and latching/blanking
//
// the pins used are:
//
//  - 13: column clock (sideset)
//  - 14: column data  (out base)
//  - 15: column latch
//  - 16: column blank
//  - 17: row select bit 0
//  - 18: row select bit 1
//  - 19: row select bit 2
//  - 20: row select bit 3
//
// the framebuffer data is structured like this:
//
// for each row:
//   for each bcd frame:
//            0: 00110110                           // row pixel count (minus one)
//      1  - 53: xxxxxbgr, xxxxxbgr, xxxxxbgr, ...  // pixel data
//      54 - 55: xxxxxxxx, xxxxxxxx                 // dummy bytes to dword align
//           56: xxxxrrrr                           // row select bits
//      57 - 59: tttttttt, tttttttt, tttttttt       // bcd tick count (0-65536)
//
//  .. and back to the start

static uint16_t r_gamma_lut[256] = {0};
static uint16_t g_gamma_lut[256] = {0};
static uint16_t b_gamma_lut[256] = {0};

static uint32_t dma_channel;
static uint32_t dma_ctrl_channel;
static uint32_t audio_dma_channel;

namespace pimoroni {

  GalacticUnicorn* GalacticUnicorn::unicorn = nullptr;
  PIO GalacticUnicorn::bitstream_pio = pio0;
  uint GalacticUnicorn::bitstream_sm = 0;
  uint GalacticUnicorn::bitstream_sm_offset = 0;
  PIO GalacticUnicorn::audio_pio = pio0;
  uint GalacticUnicorn::audio_sm = 0;
  uint GalacticUnicorn::audio_sm_offset = 0;

  // once the dma transfer of the scanline is complete we move to the
  // next scanline (or quit if we're finished)
  void __isr GalacticUnicorn::dma_complete() {
    if(unicorn != nullptr && dma_channel_get_irq0_status(audio_dma_channel)) {
      unicorn->next_audio_sequence();
    }
  }

  GalacticUnicorn::~GalacticUnicorn() {
    if(unicorn == this) {
      partial_teardown();

      dma_channel_unclaim(dma_ctrl_channel); // This works now the teardown behaves correctly
      dma_channel_unclaim(dma_channel); // This works now the teardown behaves correctly
      pio_sm_unclaim(bitstream_pio, bitstream_sm);
      pio_remove_program(bitstream_pio, &galactic_unicorn_program, bitstream_sm_offset);

      dma_channel_unclaim(audio_dma_channel); // This works now the teardown behaves correctly
      pio_sm_unclaim(audio_pio, audio_sm);
      pio_remove_program(audio_pio, &audio_i2s_program, audio_sm_offset);
      irq_remove_handler(DMA_IRQ_0, dma_complete);

      unicorn = nullptr;
    }
  }

  void GalacticUnicorn::partial_teardown() {
    // Stop the bitstream SM
    pio_sm_set_enabled(bitstream_pio, bitstream_sm, false);

    // Make sure the display is off and switch it to an invisible row, to be safe
    const uint pins_to_set = 1 << COLUMN_BLANK | 0b1111 << ROW_BIT_0;
    pio_sm_set_pins_with_mask(bitstream_pio, bitstream_sm, pins_to_set, pins_to_set);


    dma_hw->ch[dma_ctrl_channel].al1_ctrl = (dma_hw->ch[dma_ctrl_channel].al1_ctrl & ~DMA_CH0_CTRL_TRIG_CHAIN_TO_BITS) | (dma_ctrl_channel << DMA_CH0_CTRL_TRIG_CHAIN_TO_LSB);
    dma_hw->ch[dma_channel].al1_ctrl = (dma_hw->ch[dma_channel].al1_ctrl & ~DMA_CH0_CTRL_TRIG_CHAIN_TO_BITS) | (dma_channel << DMA_CH0_CTRL_TRIG_CHAIN_TO_LSB);
    // Abort any in-progress DMA transfer
    dma_safe_abort(dma_ctrl_channel);
    //dma_channel_abort(dma_ctrl_channel);
    //dma_channel_abort(dma_channel);
    dma_safe_abort(dma_channel);


    // Stop the audio SM
    pio_sm_set_enabled(audio_pio, audio_sm, false);

    // Reset the I2S pins to avoid popping when audio is suddenly stopped
    const uint pins_to_clear = 1 << I2S_DATA | 1 << I2S_BCLK | 1 << I2S_LRCLK;
    pio_sm_set_pins_with_mask(audio_pio, audio_sm, 0, pins_to_clear);

    // Abort any in-progress DMA transfer
    dma_safe_abort(audio_dma_channel);
  }

  uint16_t GalacticUnicorn::light() {
    adc_select_input(2);
    return adc_read();
  }

  void GalacticUnicorn::init() {

    if(unicorn != nullptr) {
      // Tear down the old GU instance's hardware resources
      partial_teardown();
    }


    // create 14-bit gamma luts
    for(uint16_t v = 0; v < 256; v++) {
      // gamma correct the provided 0-255 brightness value onto a
      // 0-65535 range for the pwm counter
      float r_gamma = 1.8f;
      r_gamma_lut[v] = (uint16_t)(powf((float)(v) / 255.0f, r_gamma) * (float(1U << (BCD_FRAME_COUNT)) - 1.0f) + 0.5f);
      float g_gamma = 1.8f;
      g_gamma_lut[v] = (uint16_t)(powf((float)(v) / 255.0f, g_gamma) * (float(1U << (BCD_FRAME_COUNT)) - 1.0f) + 0.5f);
      float b_gamma = 1.8f;
      b_gamma_lut[v] = (uint16_t)(powf((float)(v) / 255.0f, b_gamma) * (float(1U << (BCD_FRAME_COUNT)) - 1.0f) + 0.5f);
    }
                
    // for each row:
    //   for each bcd frame:
    //            0: 00110110                           // row pixel count (minus one)
    //      1  - 53: xxxxxbgr, xxxxxbgr, xxxxxbgr, ...  // pixel data
    //      54 - 55: xxxxxxxx, xxxxxxxx                 // dummy bytes to dword align
    //           56: xxxxrrrr                           // row select bits
    //      57 - 59: tttttttt, tttttttt, tttttttt       // bcd tick count (0-65536)
    //
    //  .. and back to the start


    // initialise the bcd timing values and row selects in the bitstream
    for(uint8_t row = 0; row < HEIGHT; row++) {
      for(uint8_t frame = 0; frame < BCD_FRAME_COUNT; frame++) {
        // find the offset of this row and frame in the bitstream
        uint8_t *p = &bitstream[row * ROW_BYTES + (BCD_FRAME_BYTES * frame)];

        p[ 0] = WIDTH - 1;               // row pixel count
        p[56] = row;                     // row select

        // set the number of bcd ticks for this frame
        uint32_t bcd_ticks = (1 << frame);
        p[57] = (bcd_ticks &     0xff) >>  0;
        p[58] = (bcd_ticks &   0xff00) >>  8;
        p[59] = (bcd_ticks & 0xff0000) >> 16;
      }
    }

    // setup light sensor adc
    adc_init();
    adc_gpio_init(LIGHT_SENSOR);

    gpio_init(COLUMN_CLOCK); gpio_set_dir(COLUMN_CLOCK, GPIO_OUT); gpio_put(COLUMN_CLOCK, false);
    gpio_init(COLUMN_DATA); gpio_set_dir(COLUMN_DATA, GPIO_OUT); gpio_put(COLUMN_DATA, false);
    gpio_init(COLUMN_LATCH); gpio_set_dir(COLUMN_LATCH, GPIO_OUT); gpio_put(COLUMN_LATCH, false);
    gpio_init(COLUMN_BLANK); gpio_set_dir(COLUMN_BLANK, GPIO_OUT); gpio_put(COLUMN_BLANK, true);

    // initialise the row select, and set them to a non-visible row to avoid flashes during setup
    gpio_init(ROW_BIT_0); gpio_set_dir(ROW_BIT_0, GPIO_OUT); gpio_put(ROW_BIT_0, true);
    gpio_init(ROW_BIT_1); gpio_set_dir(ROW_BIT_1, GPIO_OUT); gpio_put(ROW_BIT_1, true);
    gpio_init(ROW_BIT_2); gpio_set_dir(ROW_BIT_2, GPIO_OUT); gpio_put(ROW_BIT_2, true);
    gpio_init(ROW_BIT_3); gpio_set_dir(ROW_BIT_3, GPIO_OUT); gpio_put(ROW_BIT_3, true);

    sleep_ms(100);

    // configure full output current in register 2

    uint16_t reg1 = 0b1111111111001110;

    // clock the register value to the first 9 driver chips
    for(int j = 0; j < 9; j++) {
      for(int i = 0; i < 16; i++) {
        if(reg1 & (1U << (15 - i))) {
          gpio_put(COLUMN_DATA, true);
        }else{
          gpio_put(COLUMN_DATA, false);
        }
        sleep_us(10);
        gpio_put(COLUMN_CLOCK, true);
        sleep_us(10);
        gpio_put(COLUMN_CLOCK, false);
      }
    }

    // clock the last chip and latch the value
    for(int i = 0; i < 16; i++) {
      if(reg1 & (1U << (15 - i))) {
        gpio_put(COLUMN_DATA, true);
      }else{
        gpio_put(COLUMN_DATA, false);
      }

      sleep_us(10);
      gpio_put(COLUMN_CLOCK, true);
      sleep_us(10);
      gpio_put(COLUMN_CLOCK, false);

      if(i == 4) {
        gpio_put(COLUMN_LATCH, true);
      }
    }
    gpio_put(COLUMN_LATCH, false);

    // reapply the blank as the above seems to cause a slight glow.
    // Note, this will produce a brief flash if a visible row is selected (which it shouldn't be)
    gpio_put(COLUMN_BLANK, false);
    sleep_us(10);
    gpio_put(COLUMN_BLANK, true);

    gpio_init(MUTE); gpio_set_dir(MUTE, GPIO_OUT); gpio_put(MUTE, true);

    // setup button inputs
    gpio_init(SWITCH_A); gpio_pull_up(SWITCH_A);
    gpio_init(SWITCH_B); gpio_pull_up(SWITCH_B);
    gpio_init(SWITCH_C); gpio_pull_up(SWITCH_C);
    gpio_init(SWITCH_D); gpio_pull_up(SWITCH_D);

    gpio_init(SWITCH_SLEEP); gpio_pull_up(SWITCH_SLEEP);

    gpio_init(SWITCH_BRIGHTNESS_UP); gpio_pull_up(SWITCH_BRIGHTNESS_UP);
    gpio_init(SWITCH_BRIGHTNESS_DOWN); gpio_pull_up(SWITCH_BRIGHTNESS_DOWN);

    gpio_init(SWITCH_VOLUME_UP); gpio_pull_up(SWITCH_VOLUME_UP);
    gpio_init(SWITCH_VOLUME_DOWN); gpio_pull_up(SWITCH_VOLUME_DOWN);

    // setup the pio if it has not previously been set up
    bitstream_pio = pio0;
    if(unicorn == nullptr) {
      bitstream_sm = pio_claim_unused_sm(bitstream_pio, true);
      bitstream_sm_offset = pio_add_program(bitstream_pio, &galactic_unicorn_program);
    }

    pio_gpio_init(bitstream_pio, COLUMN_CLOCK);
    pio_gpio_init(bitstream_pio, COLUMN_DATA);
    pio_gpio_init(bitstream_pio, COLUMN_LATCH);
    pio_gpio_init(bitstream_pio, COLUMN_BLANK);

    pio_gpio_init(bitstream_pio, ROW_BIT_0);
    pio_gpio_init(bitstream_pio, ROW_BIT_1);
    pio_gpio_init(bitstream_pio, ROW_BIT_2);
    pio_gpio_init(bitstream_pio, ROW_BIT_3);

    // set the blank and row pins to be high, then set all led driving pins as outputs.
    // This order is important to avoid a momentary flash
    const uint pins_to_set = 1 << COLUMN_BLANK | 0b1111 << ROW_BIT_0;
    pio_sm_set_pins_with_mask(bitstream_pio, bitstream_sm, pins_to_set, pins_to_set);
    pio_sm_set_consecutive_pindirs(bitstream_pio, bitstream_sm, COLUMN_CLOCK, 8, true);

    pio_sm_config c = galactic_unicorn_program_get_default_config(bitstream_sm_offset);

    // osr shifts right, autopull on, autopull threshold 8
    sm_config_set_out_shift(&c, true, true, 32);

    // configure out, set, and sideset pins
    sm_config_set_out_pins(&c, ROW_BIT_0, 4);
    sm_config_set_set_pins(&c, COLUMN_DATA, 3);
    sm_config_set_sideset_pins(&c, COLUMN_CLOCK);

    // join fifos as only tx needed (gives 8 deep fifo instead of 4)
    sm_config_set_fifo_join(&c, PIO_FIFO_JOIN_TX);

      // setup dma transfer for pixel data to the pio
    //if(unicorn == nullptr) {
      dma_channel = dma_claim_unused_channel(true);
      dma_ctrl_channel = dma_claim_unused_channel(true);
    //}
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


    // setup audio pio program
    audio_pio = pio0;
    if(unicorn == nullptr) {
      audio_sm = pio_claim_unused_sm(audio_pio, true);
      audio_sm_offset = pio_add_program(audio_pio, &audio_i2s_program);
    }

    pio_gpio_init(audio_pio, I2S_DATA);
    pio_gpio_init(audio_pio, I2S_BCLK);
    pio_gpio_init(audio_pio, I2S_LRCLK);

    audio_i2s_program_init(audio_pio, audio_sm, audio_sm_offset, I2S_DATA, I2S_BCLK);
    uint32_t system_clock_frequency = clock_get_hz(clk_sys);
    uint32_t divider = system_clock_frequency * 4 / SYSTEM_FREQ; // avoid arithmetic overflow
    pio_sm_set_clkdiv_int_frac(audio_pio, audio_sm, divider >> 8u, divider & 0xffu);

    audio_dma_channel = dma_claim_unused_channel(true);
    dma_channel_config audio_config = dma_channel_get_default_config(audio_dma_channel);
    channel_config_set_transfer_data_size(&audio_config, DMA_SIZE_16);
    //channel_config_set_bswap(&audio_config, false); // byte swap to reverse little endian
    channel_config_set_dreq(&audio_config, pio_get_dreq(audio_pio, audio_sm, true));
    dma_channel_configure(audio_dma_channel, &audio_config, &audio_pio->txf[audio_sm], NULL, 0, false);

    dma_channel_set_irq0_enabled(audio_dma_channel, true);

    if(unicorn == nullptr) {
      irq_add_shared_handler(DMA_IRQ_0, dma_complete, PICO_SHARED_IRQ_HANDLER_DEFAULT_ORDER_PRIORITY);
      irq_set_enabled(DMA_IRQ_0, true);
    }

    unicorn = this;
  }

  void GalacticUnicorn::clear() {
    if(unicorn == this) {
      for(uint8_t y = 0; y < HEIGHT; y++) {
        for(uint8_t x = 0; x < WIDTH; x++) {
          set_pixel(x, y, 0, 0, 0);
        }
      }
    }
  }

  void GalacticUnicorn::dma_safe_abort(uint channel) {
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

  void GalacticUnicorn::play_sample(uint8_t *data, uint32_t length) {
    stop_playing();

    if(unicorn == this) {
      // Restart the audio SM and start a new DMA transfer
      pio_sm_set_enabled(audio_pio, audio_sm, true);
      dma_channel_transfer_from_buffer_now(audio_dma_channel, data, length / 2);
      play_mode = PLAYING_BUFFER;
    }
  }

  void GalacticUnicorn::play_synth() {
    if(play_mode != PLAYING_SYNTH) {
      stop_playing();
    }

    if(unicorn == this && play_mode == NOT_PLAYING) {
      // Nothing is playing, so we can set up the first buffer straight away
      current_buffer = 0;

      populate_next_synth();

      // Restart the audio SM and start a new DMA transfer
      pio_sm_set_enabled(audio_pio, audio_sm, true);

      play_mode = PLAYING_SYNTH;

      next_audio_sequence();
    }
  }

  void GalacticUnicorn::next_audio_sequence() {
    // Clear any interrupt request caused by our channel
    //dma_channel_acknowledge_irq0(audio_dma_channel);
    // NOTE Temporary replacement of the above until this reaches pico-sdk main:
    // https://github.com/raspberrypi/pico-sdk/issues/974
    dma_hw->ints0 = 1u << audio_dma_channel;

    if(play_mode == PLAYING_SYNTH) {

      dma_channel_transfer_from_buffer_now(audio_dma_channel, tone_buffers[current_buffer], TONE_BUFFER_SIZE);
      current_buffer = (current_buffer + 1) % NUM_TONE_BUFFERS;

      populate_next_synth();
    }
    else {
      play_mode = NOT_PLAYING;
    }
  }

  void GalacticUnicorn::populate_next_synth() {
    int16_t *samples = tone_buffers[current_buffer];
    for(uint i = 0; i < TONE_BUFFER_SIZE; i++) {
      samples[i] = synth.get_audio_frame();
    }
  }

  void GalacticUnicorn::stop_playing() {
    if(unicorn == this) {
      // Stop the audio SM
      pio_sm_set_enabled(audio_pio, audio_sm, false);

      // Reset the I2S pins to avoid popping when audio is suddenly stopped
      const uint pins_to_clear = 1 << I2S_DATA | 1 << I2S_BCLK | 1 << I2S_LRCLK;
      pio_sm_set_pins_with_mask(audio_pio, audio_sm, 0, pins_to_clear);

      // Abort any in-progress DMA transfer
      dma_safe_abort(audio_dma_channel);

      play_mode = NOT_PLAYING;
    }
  }

  AudioChannel& GalacticUnicorn::synth_channel(uint channel) {
    assert(channel < PicoSynth::CHANNEL_COUNT);
    return synth.channels[channel];
  }

  void GalacticUnicorn::set_pixel(int x, int y, uint8_t r, uint8_t g, uint8_t b) {
    if(x < 0 || x >= WIDTH || y < 0 || y >= HEIGHT) return;

    // make those coordinates sane
    x = (WIDTH - 1) - x;
    y = (HEIGHT - 1) - y;

    r = (r * this->brightness) >> 8;
    g = (g * this->brightness) >> 8;
    b = (b * this->brightness) >> 8;

    uint16_t gamma_r = r_gamma_lut[r];
    uint16_t gamma_g = g_gamma_lut[g];
    uint16_t gamma_b = b_gamma_lut[b];

    // for each row:
    //   for each bcd frame:
    //            0: 00110110                           // row pixel count (minus one)
    //      1  - 53: xxxxxbgr, xxxxxbgr, xxxxxbgr, ...  // pixel data
    //      54 - 55: xxxxxxxx, xxxxxxxx                 // dummy bytes to dword align
    //           56: xxxxrrrr                           // row select bits
    //      57 - 59: tttttttt, tttttttt, tttttttt       // bcd tick count (0-65536)
    //
    //  .. and back to the start

    // set the appropriate bits in the separate bcd frames
    for(uint8_t frame = 0; frame < BCD_FRAME_COUNT; frame++) {
      uint8_t *p = &bitstream[y * ROW_BYTES + (BCD_FRAME_BYTES * frame) + 1 + x];

      uint8_t red_bit = gamma_r & 0b1;
      uint8_t green_bit = gamma_g & 0b1;
      uint8_t blue_bit = gamma_b & 0b1;

      *p = (blue_bit << 0) | (green_bit << 1) | (red_bit << 2);

      gamma_r >>= 1;
      gamma_g >>= 1;
      gamma_b >>= 1;
    }
  }

  void GalacticUnicorn::set_brightness(float value) {
    value = value < 0.0f ? 0.0f : value;
    value = value > 1.0f ? 1.0f : value;
    this->brightness = floor(value * 256.0f);
  }

  float GalacticUnicorn::get_brightness() {
    return this->brightness / 255.0f;
  }

  void GalacticUnicorn::adjust_brightness(float delta) {
    this->set_brightness(this->get_brightness() + delta);
  }

  void GalacticUnicorn::set_volume(float value) {
    value = value < 0.0f ? 0.0f : value;
    value = value > 1.0f ? 1.0f : value;
    this->volume = floor(value * 255.0f);
    this->synth.volume = this->volume * 255.0f;
  }

  float GalacticUnicorn::get_volume() {
    return this->volume / 255.0f;
  }

  void GalacticUnicorn::adjust_volume(float delta) {
    this->set_volume(this->get_volume() + delta);
  }

  void GalacticUnicorn::update(PicoGraphics *graphics) {
    if(unicorn == this) {
      if(graphics->pen_type == PicoGraphics::PEN_RGB888) {
        uint32_t *p = (uint32_t *)graphics->frame_buffer;
        for(size_t j = 0; j < 53 * 11; j++) {
          int x = j % 53;
          int y = j / 53;

          uint32_t col = *p;
          uint8_t r = (col & 0xff0000) >> 16;
          uint8_t g = (col & 0x00ff00) >>  8;
          uint8_t b = (col & 0x0000ff) >>  0;
          p++;

          set_pixel(x, y, r, g, b);
        }
      }
      else if(graphics->pen_type == PicoGraphics::PEN_RGB565) {
        uint16_t *p = (uint16_t *)graphics->frame_buffer;
        for(size_t j = 0; j < 53 * 11; j++) {
          int x = j % 53;
          int y = j / 53;

          uint16_t col = __builtin_bswap16(*p);
          uint8_t r = (col & 0b1111100000000000) >> 8;
          uint8_t g = (col & 0b0000011111100000) >> 3;
          uint8_t b = (col & 0b0000000000011111) << 3;
          p++;

          set_pixel(x, y, r, g, b);
        }
      }
      else if(graphics->pen_type == PicoGraphics::PEN_RGB332) {
        uint8_t *p = (uint8_t *)graphics->frame_buffer;
        for(size_t j = 0; j < 53 * 11; j++) {
          int x = j % 53;
          int y = j / 53;

          uint8_t col = *p;
          uint8_t r = (col & 0b11100000);
          uint8_t g = (col & 0b00011100) << 3;
          uint8_t b = (col & 0b00000011) << 6;
          p++;

          set_pixel(x, y, r, g, b);
        }
      }
    }
  }

  bool GalacticUnicorn::is_pressed(uint8_t button) {
    return !gpio_get(button);
  }

}
