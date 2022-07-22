#include "ws2812.hpp"
#include "common/pimoroni_common.hpp"

namespace plasma {

WS2812::WS2812(uint num_leds, PIO pio, uint sm, uint pin, uint freq, bool rgbw, COLOR_ORDER color_order, RGB* buffer) : buffer(buffer), num_leds(num_leds), color_order(color_order), pio(pio), sm(sm) {
    pio_program_offset = pio_add_program(pio, &ws2812_program);

    pio_gpio_init(pio, pin);
    pio_sm_set_consecutive_pindirs(pio, sm, pin, 1, true);

    pio_sm_config c = ws2812_program_get_default_config(pio_program_offset);
    sm_config_set_sideset_pins(&c, pin);
    
    sm_config_set_out_shift(&c, false, true, rgbw ? 32 : 24); // Discard first (APA102 global brightness) byte. TODO support RGBW WS281X LEDs
    sm_config_set_fifo_join(&c, PIO_FIFO_JOIN_TX);

    int cycles_per_bit = ws2812_T1 + ws2812_T2 + ws2812_T3;
    float div = clock_get_hz(clk_sys) / (freq * cycles_per_bit);
    sm_config_set_clkdiv(&c, div);

    pio_sm_init(pio, sm, pio_program_offset, &c);
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
        managed_buffer = true;
    }
}

bool WS2812::dma_timer_callback(struct repeating_timer *t) {
    ((WS2812*)t->user_data)->update();
    return true;
}

void WS2812::update(bool blocking) {
    if(dma_channel_is_busy(dma_channel) && !blocking) return;
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
        set_rgb(i, 0, 0, 0);
    }
}

void WS2812::set_hsv(uint32_t index, float h, float s, float v, uint8_t w) {
    float i = floor(h * 6.0f);
    float f = h * 6.0f - i;
    v *= 255.0f;
    uint8_t p = v * (1.0f - s);
    uint8_t q = v * (1.0f - f * s);
    uint8_t t = v * (1.0f - (1.0f - f) * s);

    switch (int(i) % 6) {
      case 0: set_rgb(index, v, t, p, w); break;
      case 1: set_rgb(index, q, v, p, w); break;
      case 2: set_rgb(index, p, v, t, w); break;
      case 3: set_rgb(index, p, q, v, w); break;
      case 4: set_rgb(index, t, p, v, w); break;
      case 5: set_rgb(index, v, p, q, w); break;
    }
}

void WS2812::set_rgb(uint32_t index, uint8_t r, uint8_t g, uint8_t b, uint8_t w, bool gamma) {
    if(gamma) {
        r = pimoroni::GAMMA_8BIT[r];
        g = pimoroni::GAMMA_8BIT[g];
        b = pimoroni::GAMMA_8BIT[b];
        w = pimoroni::GAMMA_8BIT[w];
    }
    switch(color_order) {
        case COLOR_ORDER::RGB:
            buffer[index].rgb(r, g, b, w);
            break;
        case COLOR_ORDER::RBG:
            buffer[index].rgb(r, b, g, w);
            break;
        case COLOR_ORDER::GRB:
            buffer[index].rgb(g, r, b, w);
            break;
        case COLOR_ORDER::GBR:
            buffer[index].rgb(g, b, r, w);
            break;
        case COLOR_ORDER::BRG:
            buffer[index].rgb(b, r, g, w);
            break;
        case COLOR_ORDER::BGR:
            buffer[index].rgb(b, g, r, w);
            break;
    }
}

void WS2812::set_brightness(uint8_t b) {
    // WS2812 LEDs have no global brightness
}

}