/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#pragma once
#include "pico/audio_i2s.h"

#define SAMPLES_PER_BUFFER 256


typedef int16_t (*buffer_callback)(void);

struct audio_buffer_pool *init_audio(uint32_t sample_rate, uint8_t pin_data, uint8_t pin_bclk, uint8_t pio_sm=0, uint8_t dma_ch=0) {
  static audio_format_t audio_format = {
    .sample_freq = sample_rate,
    .format = AUDIO_BUFFER_FORMAT_PCM_S16,
    .channel_count = 1,
  };

  static struct audio_buffer_format producer_format = {
    .format = &audio_format,
    .sample_stride = 2
  };

  struct audio_buffer_pool *producer_pool = audio_new_producer_pool(
    &producer_format,
    3,
    SAMPLES_PER_BUFFER
  );

  const struct audio_format *output_format;

  struct audio_i2s_config config = {
    .data_pin = pin_data,
    .clock_pin_base = pin_bclk,
    .dma_channel = dma_ch,
    .pio_sm = pio_sm,
  };

  output_format = audio_i2s_setup(&audio_format, &config);
  if (!output_format) {
    panic("PicoAudio: Unable to open audio device.\n");
  }

  bool status = audio_i2s_connect(producer_pool);
  if (!status) {
    panic("PicoAudio: Unable to connect to audio device.\n");
  }

  audio_i2s_set_enabled(true);

  return producer_pool;
}

void update_buffer(struct audio_buffer_pool *ap, buffer_callback cb) {
  struct audio_buffer *buffer = take_audio_buffer(ap, true);
  int16_t *samples = (int16_t *) buffer->buffer->bytes;
  for (uint i = 0; i < buffer->max_sample_count; i++) {
      samples[i] = cb();
  }
  buffer->sample_count = buffer->max_sample_count;
  give_audio_buffer(ap, buffer);
}