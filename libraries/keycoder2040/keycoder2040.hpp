#pragma once
#include "pico/stdlib.h"

#include "encoder.hpp"

namespace pimoroni {
  namespace keycoder2040 {


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

    

    const pin_pair ENCODER_ROW1(ENCODER_ROW1_B, ENCODER_ROW1_A);
    const pin_pair ENCODER_ROW2(ENCODER_ROW2_B, ENCODER_ROW2_A);
    const pin_pair ENCODER_ROW3(ENCODER_ROW3_B, ENCODER_ROW3_A);
    const pin_pair ENCODER_ROW4(ENCODER_ROW4_B, ENCODER_ROW4_A);
    
    // The counts per revolution of the motor's output shaft
    constexpr float COUNTS_PER_REV = 24;

    // The direction to spin the motor in. NORMAL_DIR (0), REVERSED_DIR (1)
    const Direction DIRECTION = NORMAL_DIR;

    Encoder enc0 = Encoder(pio0, 0, ENCODER_ROW1, ENCODER_COL1, DIRECTION, COUNTS_PER_REV, true);
    Encoder enc1 = Encoder(pio0, 1, ENCODER_ROW1, ENCODER_COL1, DIRECTION, COUNTS_PER_REV, true);
    
    const uint NUM_ENCODERS = 12;

    const uint USER_SW = 23;


  }
}