#include "apa102.hpp"

namespace plasma {

constexpr uint8_t GAMMA[256] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2,
    2, 2, 2, 3, 3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5,
    6, 6, 6, 7, 7, 7, 8, 8, 8, 9, 9, 9, 10, 10, 11, 11,
    11, 12, 12, 13, 13, 13, 14, 14, 15, 15, 16, 16, 17, 17, 18, 18,
    19, 19, 20, 21, 21, 22, 22, 23, 23, 24, 25, 25, 26, 27, 27, 28,
    29, 29, 30, 31, 31, 32, 33, 34, 34, 35, 36, 37, 37, 38, 39, 40,
    40, 41, 42, 43, 44, 45, 46, 46, 47, 48, 49, 50, 51, 52, 53, 54,
    55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70,
    71, 72, 73, 74, 76, 77, 78, 79, 80, 81, 83, 84, 85, 86, 88, 89,
    90, 91, 93, 94, 95, 96, 98, 99, 100, 102, 103, 104, 106, 107, 109, 110,
    111, 113, 114, 116, 117, 119, 120, 121, 123, 124, 126, 128, 129, 131, 132, 134,
    135, 137, 138, 140, 142, 143, 145, 146, 148, 150, 151, 153, 155, 157, 158, 160,
    162, 163, 165, 167, 169, 170, 172, 174, 176, 178, 179, 181, 183, 185, 187, 189,
    191, 193, 194, 196, 198, 200, 202, 204, 206, 208, 210, 212, 214, 216, 218, 220,
    222, 224, 227, 229, 231, 233, 235, 237, 239, 241, 244, 246, 248, 250, 252, 255};

APA102::APA102(uint num_leds, PIO pio, uint sm, uint pin_dat, uint pin_clk, uint freq, RGB* buffer) : buffer(buffer), num_leds(num_leds), pio(pio), sm(sm) {
    pio_program_offset = pio_add_program(pio, &apa102_program);

    pio_sm_set_pins_with_mask(pio, sm, 0, (1u << pin_clk) | (1u << pin_dat));
    pio_sm_set_pindirs_with_mask(pio, sm, ~0u, (1u << pin_clk) | (1u << pin_dat));
    pio_gpio_init(pio, pin_clk);
    pio_gpio_init(pio, pin_dat);

    pio_sm_config c = apa102_program_get_default_config(pio_program_offset);
    sm_config_set_out_pins(&c, pin_dat, 1);
    sm_config_set_sideset_pins(&c, pin_clk);

    sm_config_set_out_shift(&c, false, true, 32);
    sm_config_set_fifo_join(&c, PIO_FIFO_JOIN_TX);

    // The PIO program transmits 1 bit every 2 execution cycles
    float div = (float)clock_get_hz(clk_sys) / (2 * freq);
    sm_config_set_clkdiv(&c, div);

    pio_sm_init(pio, sm, pio_program_offset, &c);
    pio_sm_set_enabled(pio, sm, true);

    dma_channel = dma_claim_unused_channel(true);
    dma_channel_config config = dma_channel_get_default_config(dma_channel);
    channel_config_set_bswap(&config, true);
    channel_config_set_dreq(&config, pio_get_dreq(pio, sm, true));
    channel_config_set_transfer_data_size(&config, DMA_SIZE_32);
    channel_config_set_read_increment(&config, true);
    dma_channel_configure(dma_channel, &config, &pio->txf[sm], NULL, 0, false);

    if(this->buffer == nullptr) {
        this->buffer = new RGB[num_leds];
        managed_buffer = true;
    }
}

bool APA102::dma_timer_callback(struct repeating_timer *t) {
    ((APA102*)t->user_data)->update();
    return true;
}

void APA102::update(bool blocking) {
    if(dma_channel_is_busy(dma_channel) && !blocking) return;
    while(dma_channel_is_busy(dma_channel)) {}; // Block waiting for DMA finish
    pio->txf[sm] = 0x00000000; // Output the APA102 start-of-frame bytes
    dma_channel_set_trans_count(dma_channel, num_leds, false);
    dma_channel_set_read_addr(dma_channel, buffer, true);
    if (!blocking) return;
    while(dma_channel_is_busy(dma_channel)) {}; // Block waiting for DMA finish
    // This is necessary to prevent a single LED remaining lit when clearing and updating.
    // This code will only run in *blocking* mode since it's assumed non-blocking will be continuously updating anyway.
    // Yes this will slow down LED updates... don't use blocking mode unless you're clearing LEDs before shutdown,
    // or you *really* want to avoid actively driving your APA102s for some reason.
    for(auto x = 0u; x < (num_leds / 16) + 1; x++) {
        pio->txf[sm] = 0x00000000;
        // Some delay is needed, since the PIO is async
        // and this could be happening during a destructor/MicroPython soft reset
        sleep_ms(1); // Chosen by fair dice roll
    }
}

bool APA102::start(uint fps) {
    add_repeating_timer_ms(-(1000 / fps), dma_timer_callback, (void*)this, &timer);
    return true;
}

bool APA102::stop() {
    return cancel_repeating_timer(&timer);
}

void APA102::clear() {
    for (auto i = 0u; i < num_leds; ++i) {
        set_rgb(i, 0, 0, 0);
    }
}

void APA102::set_hsv(uint32_t index, float h, float s, float v) {
    float i = floor(h * 6.0f);
    float f = h * 6.0f - i;
    v *= 255.0f;
    uint8_t p = v * (1.0f - s);
    uint8_t q = v * (1.0f - f * s);
    uint8_t t = v * (1.0f - (1.0f - f) * s);

    switch (int(i) % 6) {
      case 0: set_rgb(index, v, t, p); break;
      case 1: set_rgb(index, q, v, p); break;
      case 2: set_rgb(index, p, v, t); break;
      case 3: set_rgb(index, p, q, v); break;
      case 4: set_rgb(index, t, p, v); break;
      case 5: set_rgb(index, v, p, q); break;
    }
}

void APA102::set_rgb(uint32_t index, uint8_t r, uint8_t g, uint8_t b, bool gamma) {
    if(gamma) {
        r = GAMMA[r];
        g = GAMMA[g];
        b = GAMMA[b];
    }
    buffer[index].rgb(r, g, b);
}

void APA102::set_brightness(uint8_t b) {
    for (auto i = 0u; i < num_leds; ++i) {
        buffer[i].brightness(b);
    }
}

}