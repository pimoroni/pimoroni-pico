#include <math.h>

#include "hardware/irq.h"
#include "hardware/adc.h"
#include "hardware/clocks.h"

#include "yukon.hpp"

namespace pimoroni {

  const SLOT Yukon::SLOT1 = {
    1,        // ID
    0,        // FAST1
    1,        // FAST2
    2,        // FAST3
    3,        // FAST4
    {0, 3},   // SLOW1
    {0, 4},   // SLOW2
    {0, 5},   // SLOW3
    0,        // ADC1_ADDR (0b0000)
    3,        // ADC2_TEMP_ADDR (0b0011)
  };

  const SLOT Yukon::SLOT2 = {
    2,        // ID
    4,        // FAST1
    5,        // FAST2
    6,        // FAST3
    7,        // FAST4
    {0, 0},   // SLOW1
    {0, 1},   // SLOW2
    {0, 2},   // SLOW3
    1,        // ADC1_ADDR (0b0001)
    6,        // ADC2_TEMP_ADDR (0b0110)
  };

  const SLOT Yukon::SLOT3 = {
    3,        // ID
    8,        // FAST1
    9,        // FAST2
    10,       // FAST3
    11,       // FAST4
    {0, 8},   // SLOW1
    {0, 9},   // SLOW2
    {0, 10},  // SLOW3
    4,        // ADC1_ADDR (0b0100)
    2,        // ADC2_TEMP_ADDR (0b0010)
  };

  const SLOT Yukon::SLOT4 = {
    4,        // ID
    12,       // FAST1
    13,       // FAST2
    14,       // FAST3
    15,       // FAST4
    {1, 7},   // SLOW1
    {1, 5},   // SLOW2
    {1, 6},   // SLOW3
    5,        // ADC1_ADDR (0b0101)
    7,        // ADC2_TEMP_ADDR (0b0111)
  };

  const SLOT Yukon::SLOT5 = {
    5,        // ID
    16,       // FAST1
    17,       // FAST2
    18,       // FAST3
    19,       // FAST4
    {1, 15},  // SLOW1
    {1, 14},  // SLOW2
    {1, 13},  // SLOW3
    8,        // ADC1_ADDR (0b1000)
    11,       // ADC2_TEMP_ADDR (0b1011)
  };

  const SLOT Yukon::SLOT6 = {
    6,        // ID
    20,       // FAST1
    21,       // FAST2
    22,       // FAST3
    23,       // FAST4
    {1, 10},  // SLOW1
    {1, 12},  // SLOW2
    {1, 11},  // SLOW3
    9,        // ADC1_ADDR (0b1001)
    10,       // ADC2_TEMP_ADDR (0b1010)
  };


  const TCA Yukon::MAIN_EN = {0, 6};
  const TCA Yukon::USER_SW = {0, 7};

  const TCA Yukon::ADC_ADDR_1 = {0, 12};
  const TCA Yukon::ADC_ADDR_2 = {0, 13};
  const TCA Yukon::ADC_ADDR_3 = {0, 14};
  const TCA Yukon::ADC_MUX_EN_1 = {0, 15};
  const TCA Yukon::ADC_MUX_EN_2 = {0, 11};

  const TCA Yukon::SW_A = {1, 1};
  const TCA Yukon::SW_B = {1, 2};
  const TCA Yukon::LED_A = {1, 3};
  const TCA Yukon::LED_B = {1, 4};

  const TCA Yukon::LCD_BL = {1, 0};
  const TCA Yukon::LCD_DC = {1, 8};
  const TCA Yukon::LCD_CS = {1, 9};

  Yukon::~Yukon() {
  }

  void Yukon::init() {
    tca0.init();
    tca1.init();

    reset();
  }

  void Yukon::reset() {
    // Set the first IO expander's initial state
    tca0.set_output_port(0x0000);
    tca0.set_polarity_port(0x0000);
    tca0.set_config_port(0x07BF);

    // Set the second IO expander's initial state
    tca1.set_output_port(0x0000);
    tca1.set_polarity_port(0x0000);
    tca1.set_config_port(0xFCE6);
  }
}
