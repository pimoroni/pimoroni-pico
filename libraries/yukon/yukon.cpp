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

    if (!(adc_hw->cs & ADC_CS_EN_BITS)) adc_init();

    //Make sure GPIO is high-impedance, no pullups etc
    adc_gpio_init(SHARED_ADC);

    //Select ADC input 0 (GPIO26)
    adc_select_input(SHARED_ADC - 26);

    sleep_ms(100);
    set_slow_config(ADC_ADDR_1, true);
    sleep_ms(100);
    set_slow_config(ADC_ADDR_2, true);
    sleep_ms(100);
    set_slow_config(ADC_ADDR_3, true);
    sleep_ms(100);
    set_slow_config(ADC_MUX_EN_1, true);
    sleep_ms(100);
    set_slow_config(ADC_MUX_EN_2, true);
    sleep_ms(100);
  }

  TCA9555& Yukon::get_tca_chip(uint chip) {
    assert(chip < NUM_EXPANDERS);
    return (chip == 0) ? tca0 : tca1;
  }

  bool Yukon::get_slow_input(TCA gpio) {
    return get_tca_chip(gpio.CHIP).get_gpio_input(gpio.GPIO);
  }

  bool Yukon::get_slow_output(TCA gpio) {
    return get_tca_chip(gpio.CHIP).get_gpio_output(gpio.GPIO);
  }

  bool Yukon::get_slow_config(TCA gpio) {
    return get_tca_chip(gpio.CHIP).get_gpio_config(gpio.GPIO);
  }

  bool Yukon::get_slow_polarity(TCA gpio) {
    return get_tca_chip(gpio.CHIP).get_gpio_polarity(gpio.GPIO);
  }

  void Yukon::set_slow_output(TCA gpio, bool value) {
    get_tca_chip(gpio.CHIP).set_gpio_output(gpio.GPIO, value);
  }
  void Yukon::set_slow_config(TCA gpio, bool output) {
    get_tca_chip(gpio.CHIP).set_gpio_config(gpio.GPIO, output);
  }

  void Yukon::set_slow_polarity(TCA gpio, bool polarity) {
    get_tca_chip(gpio.CHIP).set_gpio_polarity(gpio.GPIO, polarity);
  }

  void Yukon::change_output_mask(uint8_t chip, uint16_t mask, uint16_t state) {
    get_tca_chip(chip).change_output_mask(mask, state);
  }

  void Yukon::deselect_address() {
    set_slow_output(ADC_MUX_EN_1, false);
    set_slow_output(ADC_MUX_EN_2, false);
  }

  void Yukon::select_address(uint8_t address) {
    if (address < 0)
      return; //raise ValueError("address is less than zero")
    else if(address > 0b1111)
      return; //raise ValueError("address is greater than number of available addresses")
    else {
      uint16_t state = 0x0000;

      if(address & 0b0001) {
        state |= 1 << ADC_ADDR_1.GPIO;
      }

      if(address & 0b0010) {
        state |= 1 << ADC_ADDR_2.GPIO;
      }

      if(address & 0b0100) {
        state |= 1 << ADC_ADDR_3.GPIO;
      }

      if(address & 0b1000) {
        state |= 1 << ADC_MUX_EN_1.GPIO;
      }
      else {
        state |= 1 << ADC_MUX_EN_2.GPIO;
      }

      uint16_t adc_io_mask = (1 << ADC_MUX_EN_1.GPIO) | (1 << ADC_MUX_EN_2.GPIO) | (1 << ADC_ADDR_1.GPIO) | (1 << ADC_ADDR_2.GPIO) | (1 << ADC_ADDR_3.GPIO);
      change_output_mask(ADC_ADDR_1.CHIP, adc_io_mask, state);
    }
    sleep_us(10); // Add a delay to let the pins settle before taking a reading
  }

  float Yukon::shared_adc_voltage() {
    adc_select_input(SHARED_ADC - 26);
    return ((float)adc_read() * 3.3f) / (1 << 12);
  }

  float Yukon::read_voltage() {
    select_address(VOLTAGE_SENSE_ADDR);
    // return (shared_adc_voltage() * (100 + 16)) / 16  # Old equation, kept for reference
    float voltage = ((shared_adc_voltage() - VOLTAGE_MIN_MEASURE) * VOLTAGE_MAX) / (VOLTAGE_MAX_MEASURE - VOLTAGE_MIN_MEASURE);
    return MAX(voltage, 0.0f);
  }

  float Yukon::read_current() {
    select_address(CURRENT_SENSE_ADDR);
    // return (shared_adc_voltage() - 0.015) / ((1 + (5100 / 27.4)) * 0.0005)  # Old equation, kept for reference
    float current =((shared_adc_voltage() - CURRENT_MIN_MEASURE) * CURRENT_MAX) / (CURRENT_MAX_MEASURE - CURRENT_MIN_MEASURE);
    return MAX(current, 0.0f);
  }

  float Yukon::read_temperature() {
    select_address(TEMP_SENSE_ADDR);
    float sense = shared_adc_voltage();
    float r_thermistor = sense / ((3.3f - sense) / 5100);
    static constexpr float ROOM_TEMP = 273.15f + 25.0f;
    static constexpr float RESISTOR_AT_ROOM_TEMP = 10000.0f;
    static constexpr float BETA = 3435.0f;
    float t_kelvin = (BETA * ROOM_TEMP) / (BETA + (ROOM_TEMP * log(r_thermistor / RESISTOR_AT_ROOM_TEMP)));
    float t_celsius = t_kelvin - 273.15f;

    // https://www.allaboutcircuits.com/projects/measuring-temperature-with-an-ntc-thermistor/
    return t_celsius;
  }

  float Yukon::read_expansion() {
    select_address(EX_ADC_ADDR);
    return shared_adc_voltage();
  }

  float Yukon::read_slot_adc1(SLOT slot) {
    select_address(slot.ADC1_ADDR);
    return shared_adc_voltage();
  }

  float Yukon::read_slot_adc2(SLOT slot) {
    select_address(slot.ADC2_TEMP_ADDR);
    return shared_adc_voltage();
  }
}
