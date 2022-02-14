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
        uint32_t time;
        bool state;

        TransitionData() : servo(0), time(0), state(false) {};
        TransitionData(uint8_t servo, uint32_t time, bool new_state) : servo(servo), time(time), state(new_state) {};

        bool compare(const TransitionData& other) const {
            return time <= other.time;
        }
    };

    class MultiPWM {
        public:
            MultiPWM(PIO pio, uint sm, uint pin);
            ~MultiPWM();
            bool start(uint sequence_num=0);
            bool stop();
            void clear();
            void set_servo_duty(uint servo, uint32_t duty);
            void apply_servo_duty();
        private:
            static void sorted_insert(TransitionData array[], uint &size, const TransitionData &data);
            PIO pio;
            uint sm;
            uint pio_program_offset;
            uint pin_mask;
            uint pin_duty[32];
    };
}