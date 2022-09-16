#pragma once
#include "pico/stdlib.h"
#include "hardware/pwm.h"

#include "analog.hpp"
#include "pimoroni_i2c.hpp"

using namespace pimoroni;


namespace automation {
  // IO Pin Constants
  const uint GP0 = 0;
  const uint GP1 = 1;
  const uint GP2 = 2;


  // Index Constants
  const uint RELAY_1 = 0;
  const uint RELAY_2 = 1;
  const uint RELAY_3 = 2;

  const uint OUTPUT_1 = 0;
  const uint OUTPUT_2 = 1;
  const uint OUTPUT_3 = 2;

  const uint ADC_1 = 0;
  const uint ADC_2 = 1;
  const uint ADC_3 = 2;

  const uint INPUT_1 = 0;
  const uint INPUT_2 = 1;
  const uint INPUT_3 = 2;
  const uint INPUT_4 = 3;

  const uint SWITCH_A = 0;
  const uint SWITCH_B = 1;


  // Count Constants Regualr
  const uint NUM_GPIOS_REGULAR = 3;
  const uint NUM_RELAYS_REGULAR = 3;
  const uint NUM_OUTPUTS_REGULAR = 3;
  const uint NUM_ADCS_REGULAR = 3;
  const uint NUM_INPUTS_REGULAR = 4;
  const uint NUM_SWITCHES_REGULAR = 2;

  // For Automation Mini
  const uint NUM_GPIOS_MINI = 3;
  const uint NUM_RELAYS_MINI = 1;
  const uint NUM_OUTPUTS_MINI = 2;
  const uint NUM_ADCS_MINI = 3;
  const uint NUM_INPUTS_MINI = 2;
  const uint NUM_SWITCHES_MINI = 2;

  const uint NUM_ADCS_DEFAULT = 3;


  class Automation2040W {
    //--------------------------------------------------
    // Constants
    //--------------------------------------------------
  public:
    static const uint CONN_LED_PIN = 3;
    static const uint I2C_SDA_PIN = 4;
    static const uint I2C_SCL_PIN = 5;
    static const uint ADC_LED_PINS[];
    static const uint RELAY_PINS[];
    static const uint USER_SW_PINS[];
    static const uint USER_LED_PINS[];
    static const uint OUTPUT_PINS[];
    static const uint IN_BUFFERED_PINS[];
    static const uint ADC_PINS[];

    static constexpr float VOLTAGE_GAIN = 0.06f;  // 56 / (56 + 820)
    static constexpr float VOLTAGE_OFFSET = -0.06f;
    static constexpr float MAX_ADC_LED_VOLTAGE = 45.0f;


    //--------------------------------------------------
    // Variables
    //--------------------------------------------------
  public:
    I2C i2c;
    bool isMini = false;
    uint NUM_GPIOS = 0;
    uint NUM_RELAYS = 0;
    uint NUM_OUTPUTS = 0;
    uint NUM_ADCS = NUM_ADCS_DEFAULT;
    uint NUM_INPUTS = 0;
    uint NUM_SWITCHES = 0;

  private:
    Analog analogs[NUM_ADCS_DEFAULT];


    //--------------------------------------------------
    // Constructors/Destructor
    //--------------------------------------------------
  public:
    Automation2040W();


    //--------------------------------------------------
    // Methods
    //--------------------------------------------------
  public:
    bool init(bool automationMini);

    void conn_led(bool on);
    void conn_led(float brightness);
    bool switch_pressed(uint sw);
    void switch_led(uint sw, bool on);
    void switch_led(uint sw, float brightness);

    bool relay(uint relay);
    void relay(uint relay, bool actuate);
    void actuate_relay(uint relay);
    void release_relay(uint relay);

    bool output(uint output);
    void output(uint output, bool value);

    bool read_input(uint input);
    float read_adc(uint adc);

    void reset();
  };

}
