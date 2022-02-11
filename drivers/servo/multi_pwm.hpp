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

    class MultiPWM {
        public:
            MultiPWM(PIO pio, uint sm, uint pin);
            ~MultiPWM();
            bool start(uint fps=60);
            bool stop();
            void clear();
        private:
            PIO pio;
            uint sm;
            uint pio_program_offset;
    };
}