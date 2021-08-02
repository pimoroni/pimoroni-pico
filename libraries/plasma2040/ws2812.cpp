#include "ws2812.hpp"

namespace plasma {

WS2812::WS2812(uint num_leds, PIO pio, uint sm, uint pin, uint freq, RGB* buffer) : buffer(buffer), num_leds(num_leds), pio(pio), sm(sm) {
    uint offset = pio_add_program(pio, &ws2812_program);

    pio_gpio_init(pio, pin);
    pio_sm_set_consecutive_pindirs(pio, sm, pin, 1, true);

    pio_sm_config c = ws2812_program_get_default_config(offset);
    sm_config_set_sideset_pins(&c, pin);
    
    sm_config_set_out_shift(&c, false, true, 24); // Discard first (APA102 global brightness) byte. TODO support RGBW WS281X LEDs
    sm_config_set_fifo_join(&c, PIO_FIFO_JOIN_TX);

    int cycles_per_bit = ws2812_T1 + ws2812_T2 + ws2812_T3;
    float div = clock_get_hz(clk_sys) / (freq * cycles_per_bit);
    sm_config_set_clkdiv(&c, div);

    pio_sm_init(pio, sm, offset, &c);
    pio_sm_set_enabled(pio, sm, true);

    dma_channel = dma_claim_unused_channel(true);
    dma_channel_config config = dma_channel_get_default_config(dma_channel);
    channel_config_set_bswap(&config, true);
    channel_config_set_dreq(&config, pio_get_dreq(pio, sm, true));
    channel_config_set_transfer_data_size(&config, DMA_SIZE_32);
    channel_config_set_read_increment(&config, true);
    dma_channel_set_trans_count(dma_channel, num_leds, false);
    dma_channel_set_read_addr(dma_channel, (uint32_t *)buffer, false);
    dma_channel_configure(dma_channel, &config, &pio->txf[sm], NULL, 0, false);

    if(!this->buffer) {
        this->buffer = new RGB[num_leds];
    }
}

bool WS2812::dma_timer_callback(struct repeating_timer *t) {
    ((WS2812*)t->user_data)->update();
    return true;
}

void WS2812::update(bool blocking) {
    while(dma_channel_is_busy(dma_channel)) {}; // Block waiting for DMA finish
    dma_channel_set_trans_count(dma_channel, num_leds, false);
    dma_channel_set_read_addr(dma_channel, buffer, true);
    if (!blocking) return;
    while(dma_channel_is_busy(dma_channel)) {}; // Block waiting for DMA finish
}

bool WS2812::start(uint fps) {
    add_repeating_timer_ms(-(1000 / fps), dma_timer_callback, (void*)this, &timer);
    return true;
}

bool WS2812::stop() {
    return cancel_repeating_timer(&timer);
}

void WS2812::clear() {
    for (auto i = 0u; i < num_leds; ++i) {
        buffer[i].rgb(0, 0, 0);
    }
}

void WS2812::set_hsv(uint32_t index, float h, float s, float v) {
    float i = floor(h * 6.0f);
    float f = h * 6.0f - i;
    v *= 255.0f;
    uint8_t p = v * (1.0f - s);
    uint8_t q = v * (1.0f - f * s);
    uint8_t t = v * (1.0f - (1.0f - f) * s);

    switch (int(i) % 6) {
      case 0: buffer[index].rgb(v, t, p); break;
      case 1: buffer[index].rgb(q, v, p); break;
      case 2: buffer[index].rgb(p, v, t); break;
      case 3: buffer[index].rgb(p, q, v); break;
      case 4: buffer[index].rgb(t, p, v); break;
      case 5: buffer[index].rgb(v, p, q); break;
    }
}

void WS2812::set_rgb(uint32_t index, uint8_t r, uint8_t g, uint8_t b) {
    buffer[index].rgb(r, g, b);
}

void WS2812::set_brightness(uint8_t b) {
    // WS2812 LEDs have no global brightness
}

}