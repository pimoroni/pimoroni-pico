#pragma once
#include "pico/stdlib.h"
#include "keycoder.pio.h"
#include "common/pimoroni_common.hpp"

namespace pimoroni {

  class keycoder2040 {

    static constexpr float ROTARY_CPR = 24.0f;
    // Constants

    const uint ENCODER_ROW1_A = 8;
    const uint ENCODER_ROW1_B = 9;
    const uint ENCODER_ROW2_A = 10;
    const uint ENCODER_ROW2_B = 11;
    const uint ENCODER_ROW3_A = 12;
    const uint ENCODER_ROW3_B = 13;
    const uint ENCODER_ROW4_A = 14;
    const uint ENCODER_ROW4_B = 15;

    const uint ENCODER_COL1 = 16;
    const uint ENCODER_COL2 = 17;
    const uint ENCODER_COL3 = 18;
    const uint ENCODER_COL4 = 19;

    static constexpr float DEFAULT_COUNTS_PER_REV   = ROTARY_CPR;
    static const bool DEFAULT_COUNT_MICROSTEPS      = false;
    static const uint16_t DEFAULT_FREQ_DIVIDER      = 1;

    

    
    // The counts per revolution of the motor's output shaft
    static const float COUNTS_PER_REV = 96.0;

    // The direction to spin the motor in. NORMAL_DIR (0), REVERSED_DIR (1)
    const Direction DIRECTION = NORMAL_DIR;


    const uint USER_SW = 23;

    
      
      private:
        static const uint32_t COL_1_MASK = 0xFF000000;
        static const uint32_t COL_2_MASK = 0x00FF0000;
        static const uint32_t COL_3_MASK = 0x0000FF00;
        static const uint32_t COL_4_MASK = 0x000000FF;
        static const uint8_t  COL_1_LSHFT = 24;
        static const uint8_t  COL_2_LSHFT = 16;
        static const uint8_t  COL_3_LSHFT = 8;
        static const uint8_t  COL_4_LSHFT = 0;

        enum StepDir {
          NO_DIR    = 0,
          INCREASING = 1,
          DECREASING = -1,
        };

        enum MicroStep : uint8_t {
          MICROSTEP_0 = 0b00,
          MICROSTEP_1 = 0b10,
          MICROSTEP_2 = 0b11,
          MICROSTEP_3 = 0b01,
        };

      private:
        PIO pio;
        uint sm;
        Direction enc_direction;
        float enc_counts_per_rev;

        const bool count_microsteps;
        const uint16_t freq_divider;
        const float clocks_per_time;



      
      public:
        uint32_t last_state = 0x00;
        uint32_t new_state = 0x00;

      public:
        void Interface(PIO pio, uint sm,  Direction direction = NORMAL_DIR,
            float counts_per_rev = DEFAULT_COUNTS_PER_REV, bool count_microsteps = DEFAULT_COUNT_MICROSTEPS,
            uint16_t freq_divider = DEFAULT_FREQ_DIVIDER);
        

        void process_steps();



    


  };
}