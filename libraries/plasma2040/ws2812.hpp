/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*
This code is significantly modified from the PIO apa102 example
found here: https://github.com/raspberrypi/pico-examples/tree/master/pio/ws2812
*/

#pragma once

#include <math.h>
#include <cstdint>

#include "ws2812.pio.h"

#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/dma.h"
#include "hardware/irq.h"
#include "hardware/clocks.h"
#include "hardware/timer.h"

namespace plasma {

    class WS2812 {
        public:
            static const uint SERIAL_FREQ_400KHZ = 400000;
            static const uint SERIAL_FREQ_800KHZ = 800000;
            static const uint DEFAULT_SERIAL_FREQ = SERIAL_FREQ_400KHZ;
            enum class COLOR_ORDER {
                RGB,
                RBG,
                GRB,
                GBR,
                BRG,
                BGR
            };
#pragma pack(push, 1)
            union alignas(4) RGB {
                struct {
                    uint8_t r;
                    uint8_t g;
                    uint8_t b;
                    uint8_t w = 0b00000000;
                } ;
                uint32_t srgb;
                void operator=(uint32_t v) {
                    srgb = v;
                };
                void rgb(uint8_t r, uint8_t g, uint8_t b, uint8_t w=0) {
                    this->r = r;
                    this->g = g;
                    this->b = b;
                    this->w = w;
                };
                RGB() : r(0), g(0), b(0), w(0) {};
            };
#pragma pack(pop)
            RGB *buffer;
            uint32_t num_leds;
            COLOR_ORDER color_order;

            WS2812(uint num_leds, PIO pio, uint sm, uint pin, uint freq=DEFAULT_SERIAL_FREQ, bool rgbw=false, COLOR_ORDER color_order=COLOR_ORDER::RGB, RGB* buffer=nullptr);
            ~WS2812() {
                stop();
                clear();
                update(true);
                dma_channel_unclaim(dma_channel);
                pio_sm_unclaim(pio, sm);
                delete[] buffer;
            }
            bool start(uint fps=60);
            bool stop();
            void update(bool blocking=false);
            void clear();
            void set_hsv(uint32_t index, float h, float s, float v);
            void set_rgb(uint32_t index, uint8_t r, uint8_t g, uint8_t b, uint8_t w=0);
            void set_brightness(uint8_t b);
            RGB get(uint32_t index) {return buffer[index];};

            static bool dma_timer_callback(struct repeating_timer *t);

        private:
            uint32_t fps;
            PIO pio;
            uint sm;
            int dma_channel;
            struct repeating_timer timer;
    };
}