/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*
This code is significantly modified from the PIO apa102 example
found here: https://github.com/raspberrypi/pico-examples/tree/master/pio/apa102
*/

#pragma once

#include <math.h>
#include <cstdint>

#ifndef NO_QSTR
#include "apa102.pio.h"
#endif

#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/dma.h"
#include "hardware/irq.h"
#include "hardware/clocks.h"
#include "hardware/timer.h"

namespace plasma {

    class APA102 {
        public:
            static const uint DEFAULT_SERIAL_FREQ = 20 * 1000 * 1000; // 20MHz
#pragma pack(push, 1)
            union alignas(4) RGB {
                struct {
                    uint8_t sof = 0b11101111;
                    uint8_t b;
                    uint8_t g;
                    uint8_t r;
                } ;
                uint32_t srgb;
                void operator=(uint32_t v) {
                    srgb = v;
                };
                void brightness(uint8_t b) {
                    sof = 0b11100000 | b;
                };
                void rgb(uint8_t r, uint8_t g, uint8_t b) {
                    this->r = r;
                    this->g = g;
                    this->b = b;;
                }
                RGB() : sof(0b11101111), b(0), g(0), r(0) {};
            };
#pragma pack(pop)
            RGB *buffer;
            uint32_t num_leds;

            APA102(uint num_leds, PIO pio, uint sm, uint pin_dat, uint pin_clk, uint freq=DEFAULT_SERIAL_FREQ, RGB* buffer=nullptr);
            ~APA102() {
                stop();
                clear();
                update(true);
                dma_channel_unclaim(dma_channel);
                pio_sm_set_enabled(pio, sm, false);
                pio_remove_program(pio, &apa102_program, pio_program_offset);
#ifndef MICROPY_BUILD_TYPE
                // pio_sm_unclaim seems to hardfault in MicroPython
                pio_sm_unclaim(pio, sm);
#endif
                if(managed_buffer) {
                    // Only delete buffers we have allocated ourselves.
                    delete[] buffer;
                }
            }
            bool start(uint fps=60);
            bool stop();
            void update(bool blocking=false);
            void clear();
            void set_hsv(uint32_t index, float h, float s, float v);
            void set_rgb(uint32_t index, uint8_t r, uint8_t g, uint8_t b, bool gamma=true);
            void set_brightness(uint8_t b);
            RGB get(uint32_t index) {return buffer[index];};

            static bool dma_timer_callback(struct repeating_timer *t);

        private:
            uint32_t fps;
            PIO pio;
            uint sm;
            uint pio_program_offset;
            int dma_channel;
            struct repeating_timer timer;
            bool managed_buffer = false;
    };
}