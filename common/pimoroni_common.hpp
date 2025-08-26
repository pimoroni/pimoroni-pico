#pragma once
#include <stdint.h>
#include <climits>
#include "pico/stdlib.h"

#define PIMORONI_I2C_DEFAULT_INSTANCE i2c0
#define PIMORONI_SPI_DEFAULT_INSTANCE spi0

// Macro to return a value clamped between a minimum and maximum
#ifndef CLAMP
#define CLAMP(a, mn, mx) ((a)<(mx)?((a)>(mn)?(a):(mn)):(mx))
#endif


namespace pimoroni {
    static const unsigned int PIN_UNUSED = INT_MAX; // Intentionally INT_MAX to avoid overflowing MicroPython's int type

    // I2C
    static const unsigned int I2C_DEFAULT_BAUDRATE = 400000;
    static const unsigned int I2C_DEFAULT_SDA = 20;
    static const unsigned int I2C_DEFAULT_SCL = 21;
    static const unsigned int I2C_DEFAULT_INT = 22;

    static const unsigned int I2C_BG_SDA = 4;
    static const unsigned int I2C_BG_SCL = 5;
    static const unsigned int I2C_BG_INT = 3;

    static const unsigned int I2C_HEADER_SDA = 20;
    static const unsigned int I2C_HEADER_SCL = 21;
    static const unsigned int I2C_HEADER_INT = 19;

    // SPI
    static const unsigned int SPI_DEFAULT_MOSI = 19;
    static const unsigned int SPI_DEFAULT_MISO = 16;
    static const unsigned int SPI_DEFAULT_DC = 16;
    static const unsigned int SPI_DEFAULT_SCK = 18;

    static const unsigned int SPI_BG_FRONT_PWM = 20;
    static const unsigned int SPI_BG_FRONT_CS = 17;

    static const unsigned int SPI_BG_BACK_PWM = 21;
    static const unsigned int SPI_BG_BACK_CS = 22;

    enum BG_SPI_SLOT {
      BG_SPI_FRONT,
      BG_SPI_BACK,
      PICO_EXPLORER_ONBOARD
    };

    enum BOARD {
      BREAKOUT_GARDEN,
      PICO_EXPLORER,
      PLASMA_STICK,
      PLASMA_2040,
      INTERSTATE_75,
      SERVO_2040,
      MOTOR_2040
    };
  
    enum Rotation {
      ROTATE_0 = 0,
      ROTATE_90 = 90,
      ROTATE_180 = 180,
      ROTATE_270 = 270
    };

    enum Polarity {
      ACTIVE_LOW = 0,
      ACTIVE_HIGH = 1
    };

    enum Direction {
      NORMAL_DIR    = 0,
      REVERSED_DIR  = 1,
    };

    inline uint32_t millis() {
      return to_ms_since_boot(get_absolute_time());
    }

    inline constexpr uint8_t GAMMA_8BIT[256] = {
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

    inline constexpr uint16_t GAMMA_12BIT[256] = {
      0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15,
      16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31,
      32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 43, 44, 45, 47, 48, 50,
      52, 53, 55, 57, 59, 61, 63, 65, 68, 70, 72, 75, 78, 80, 83, 86,
      89, 92, 95, 98, 102, 105, 109, 112, 116, 120, 124, 128, 132, 137, 141, 146,
      150, 155, 160, 165, 170, 175, 181, 186, 192, 198, 204, 210, 216, 222, 228, 235,
      242, 249, 256, 263, 270, 277, 285, 293, 301, 309, 317, 325, 334, 342, 351, 360,
      369, 379, 388, 398, 408, 418, 428, 438, 449, 459, 470, 481, 493, 504, 516, 527,
      539, 552, 564, 576, 589, 602, 615, 629, 642, 656, 670, 684, 698, 713, 727, 742,
      758, 773, 789, 804, 820, 837, 853, 870, 887, 904, 921, 939, 957, 975, 993, 1011,
      1030, 1049, 1068, 1088, 1107, 1127, 1147, 1168, 1189, 1209, 1231, 1252, 1274, 1296, 1318, 1340,
      1363, 1386, 1409, 1432, 1456, 1480, 1504, 1529, 1554, 1579, 1604, 1630, 1656, 1682, 1708, 1735,
      1762, 1789, 1817, 1845, 1873, 1901, 1930, 1959, 1988, 2018, 2048, 2078, 2109, 2139, 2171, 2202,
      2234, 2266, 2298, 2331, 2364, 2397, 2430, 2464, 2498, 2533, 2568, 2603, 2638, 2674, 2710, 2747,
      2784, 2821, 2858, 2896, 2934, 2973, 3011, 3050, 3090, 3130, 3170, 3210, 3251, 3292, 3334, 3376,
      3418, 3461, 3504, 3547, 3591, 3635, 3679, 3724, 3769, 3814, 3860, 3906, 3953, 4000, 4047, 4095};

    /* Moved from pico_unicorn.cpp
    v = (uint16_t)(powf((float)(n) / 255.0f, 2.2) * 16383.0f + 0.5f) */
    inline constexpr uint16_t GAMMA_14BIT[256] = {
      0, 0, 0, 1, 2, 3, 4, 6, 8, 10, 13, 16, 20, 23, 28, 32,
      37, 42, 48, 54, 61, 67, 75, 82, 90, 99, 108, 117, 127, 137, 148, 159,
      170, 182, 195, 207, 221, 234, 249, 263, 278, 294, 310, 326, 343, 361, 379, 397,
      416, 435, 455, 475, 496, 517, 539, 561, 583, 607, 630, 654, 679, 704, 730, 756,
      783, 810, 838, 866, 894, 924, 953, 983, 1014, 1045, 1077, 1110, 1142, 1176, 1210, 1244,
      1279, 1314, 1350, 1387, 1424, 1461, 1499, 1538, 1577, 1617, 1657, 1698, 1739, 1781, 1823, 1866,
      1910, 1954, 1998, 2044, 2089, 2136, 2182, 2230, 2278, 2326, 2375, 2425, 2475, 2525, 2577, 2629,
      2681, 2734, 2787, 2841, 2896, 2951, 3007, 3063, 3120, 3178, 3236, 3295, 3354, 3414, 3474, 3535,
      3596, 3658, 3721, 3784, 3848, 3913, 3978, 4043, 4110, 4176, 4244, 4312, 4380, 4449, 4519, 4589,
      4660, 4732, 4804, 4876, 4950, 5024, 5098, 5173, 5249, 5325, 5402, 5479, 5557, 5636, 5715, 5795,
      5876, 5957, 6039, 6121, 6204, 6287, 6372, 6456, 6542, 6628, 6714, 6801, 6889, 6978, 7067, 7156,
      7247, 7337, 7429, 7521, 7614, 7707, 7801, 7896, 7991, 8087, 8183, 8281, 8378, 8477, 8576, 8675,
      8775, 8876, 8978, 9080, 9183, 9286, 9390, 9495, 9600, 9706, 9812, 9920, 10027, 10136, 10245, 10355,
      10465, 10576, 10688, 10800, 10913, 11027, 11141, 11256, 11371, 11487, 11604, 11721, 11840, 11958, 12078, 12198,
      12318, 12440, 12562, 12684, 12807, 12931, 13056, 13181, 13307, 13433, 13561, 13688, 13817, 13946, 14076, 14206,
      14337, 14469, 14602, 14735, 14868, 15003, 15138, 15273, 15410, 15547, 15685, 15823, 15962, 16102, 16242, 16383};

  struct pin_pair {
    union {
      uint8_t first;
      uint8_t a;
      uint8_t positive;
      uint8_t phase;
    };
    union {
      uint8_t second;
      uint8_t b;
      uint8_t negative;
      uint8_t enable;
    };

    constexpr pin_pair() : first(0), second(0) {}
    constexpr pin_pair(uint8_t first, uint8_t second) : first(first), second(second) {}
  };

  struct bool_pair {
    union {
      bool first;
      bool a;
    };
    union {
      bool second;
      bool b;
    };

    bool_pair() : first(false), second(false) {}
    bool_pair(bool first, bool second) : first(first), second(second) {}
  };
}
