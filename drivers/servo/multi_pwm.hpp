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

#include "multi_pwm.pio.h"

#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/dma.h"
#include "hardware/irq.h"
#include "hardware/clocks.h"
#include "hardware/timer.h"

namespace servo {

    struct TransitionData {
        uint8_t servo;
        uint32_t level;
        bool state;

        TransitionData() : servo(0), level(0), state(false) {};
        TransitionData(uint8_t servo, uint32_t level, bool new_state) : servo(servo), level(level), state(new_state) {};

        bool compare(const TransitionData& other) const {
            return level <= other.level;
        }
    };

    class MultiPWM {
        public:
            MultiPWM(PIO pio, uint sm, uint channel_mask);
            ~MultiPWM();
            void set_wrap(uint32_t wrap, bool load = true);
            void set_chan_level(uint8_t channel, uint32_t level, bool load = true);
            void set_chan_offset(uint8_t channel, uint32_t offset, bool load = true);
            void set_chan_polarity(uint8_t channel, bool polarity, bool load = true);
            void set_clkdiv(float divider);
            void set_clkdiv_int_frac(uint16_t integer, uint8_t fract);
            //void set_phase_correct(bool phase_correct);
            //void set_enabled(bool enabled);
            void load_pwm();
        private:
            static bool bit_in_mask(uint bit, uint mask);
            static void sorted_insert(TransitionData array[], uint &size, const TransitionData &data);
        private:
            PIO pio;
            uint sm;
            uint pio_program_offset;
            uint channel_mask;
            uint channel_levels[NUM_BANK0_GPIOS];
            uint channel_offsets[NUM_BANK0_GPIOS];
            uint channel_polarities;
            uint wrap_level;
    };
}