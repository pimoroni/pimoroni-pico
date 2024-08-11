#pragma once

#include "hardware/gpio.h"
#include "common/pimoroni_common.hpp"
#include "common/pimoroni_i2c.hpp"
#include <vector>
#include <initializer_list>

#define LTR559_ALS_CONTROL 0x80
#define LTR559_ALS_CONTROL_GAIN_MASK 0b111
#define LTR559_ALS_CONTROL_GAIN_SHIFT 2
#define LTR559_ALS_CONTROL_SW_RESET_BIT 1
#define LTR559_ALS_CONTROL_MODE_BIT 0

#define LTR559_PS_CONTROL 0x81
#define LTR559_PS_CONTROL_SATURATION_INDICATOR_ENABLE_BIT 5
#define LTR559_PS_CONTROL_ACTIVE_MASK 0b11

#define LTR559_PS_LED 0x82
#define LTR559_PS_LED_PULSE_FREQ_MASK 0b111
#define LTR559_PS_LED_PULSE_FREQ_SHIFT 5
#define LTR559_PS_LED_DUTY_CYCLE_MASK 0b11
#define LTR559_PS_LED_DUTY_CYCLE_SHIFT 3
#define LTR559_PS_LED_CURRENT_MASK 0b111

#define LTR559_PS_N_PULSES 0x83
#define LTR559_PS_N_PULSES_MASK 0b1111

#define LTR559_PS_MEAS_RATE 0x84
#define LTR559_PS_MEAS_RATE_RATE_MS_MASK 0b1111

#define LTR559_ALS_MEAS_RATE 0x85
#define LTR559_ALS_MEAS_RATE_INTEGRATION_TIME_MASK 0b111
#define LTR559_ALS_MEAS_RATE_INTEGRATION_TIME_SHIFT 3
#define LTR559_ALS_MEAS_RATE_REPEAT_RATE_MASK 0b111

#define LTR559_PART_ID 0x86
#define LTR559_PART_ID_PART_NUMBER_MASK 0b1111
#define LTR559_PART_ID_PART_NUMBER_SHIFT 4
#define LTR559_PART_ID_REVISION_MASK 0b1111

#define LTR559_MANUFACTURER_ID 0x87

#define LTR559_ALS_DATA 0x88
#define LTR559_ALS_DATA_CH1 0x88
#define LTR559_ALS_DATA_CH0 0x8a

#define LTR559_ALS_PS_STATUS 0x8c
#define LTR559_ALS_PS_STATUS_INTERRUPT_MASK 0b00001010
#define LTR559_ALS_PS_STATUS_ALS_DATA_VALID_BIT 7
#define LTR559_ALS_PS_STATUS_ALS_GAIN_MASK 0b111
#define LTR559_ALS_PS_STATUS_ALS_GAIN_SHIFT 4
#define LTR559_ALS_PS_STATUS_ALS_INTERRUPT_BIT 3
#define LTR559_ALS_PS_STATUS_ALS_DATA_BIT 2
#define LTR559_ALS_PS_STATUS_PS_INTERRUPT_BIT 1
#define LTR559_ALS_PS_STATUS_PS_DATA_BIT 0

#define LTR559_PS_DATA 0x8d
#define LTR559_PS_DATA_MASK 0x07FF

#define LTR559_PS_DATA_SATURATION 0x8e
#define LTR559_PS_DATA_SATURATION_SHIFT 4

#define LTR559_INTERRUPT 0x8f
#define LTR559_INTERRUPT_POLARITY_BIT 2
#define LTR559_INTERRUPT_ALS_PS_MASK 0b11
#define LTR559_INTERRUPT_PS_BIT 0
#define LTR559_INTERRUPT_ALS_BIT 1

#define LTR559_PS_THRESHOLD_UPPER 0x90
#define LTR559_PS_THRESHOLD_LOWER 0x92

#define LTR559_PS_OFFSET 0x94
#define LTR559_PS_OFFSET_MASK 0x03FF

#define LTR559_ALS_THRESHOLD_UPPER 0x97
#define LTR559_ALS_THRESHOLD_LOWER 0x99

#define LTR559_INTERRUPT_PERSIST 0x9e
#define LTR559_INTERRUPT_PERSIST_PS_MASK 0b1111
#define LTR559_INTERRUPT_PERSIST_PS_SHIFT 4
#define LTR559_INTERRUPT_PERSIST_ALS_MASK 0b1111

#define LTR559_VALID_PART_ID 0x09
#define LTR559_VALID_REVISION_ID 0x02


namespace pimoroni {

  typedef struct {
    uint16_t proximity;
    uint16_t als0;
    uint16_t als1;
	  uint16_t integration_time;
	  uint16_t gain;
	  float ratio;
    float lux;
  } ltr559_reading;

  class LTR559 {
    //--------------------------------------------------
    // Constants
    //--------------------------------------------------
  public:
    static const uint8_t DEFAULT_I2C_ADDRESS    = 0x23;

  private:
    const int ch0_c[4] = {17743, 42785, 5926, 0};
    const int ch1_c[4] = {-11059, 19548, -1185, 0};


    //--------------------------------------------------
    // Variables
    //--------------------------------------------------
  public:
    ltr559_reading data;

  private:
    I2C *i2c;

    // interface pins with our standard defaults where appropriate
    const uint8_t address    = DEFAULT_I2C_ADDRESS;
    uint interrupt           = PIN_UNUSED;

    static constexpr uint16_t lookup_led_current[5] = {5, 10, 20, 50, 100};
    static constexpr uint16_t lookup_led_duty_cycle[4] = {25, 50, 75, 100};
    static constexpr uint16_t lookup_led_pulse_freq[8] = {30, 40, 50, 60, 70, 80, 90, 100};
    static constexpr uint16_t lookup_proximity_meas_rate[8] = {10, 50, 70, 100, 200, 500, 1000, 2000};
    static constexpr uint16_t lookup_light_integration_time[8] = {100, 50, 200, 400, 150, 250, 300, 350};
    static constexpr uint16_t lookup_light_repeat_rate[6] = {50, 100, 200, 500, 1000, 2000};
    static constexpr uint16_t lookup_light_gain[8] = {1, 2, 4, 8, 0, 0, 48, 96};

    //--------------------------------------------------
    // Constructors/Destructor
    //--------------------------------------------------
  public:
    LTR559() : LTR559(new I2C()) {};

    LTR559(I2C *i2c, uint interrupt = PIN_UNUSED) : i2c(i2c), interrupt(interrupt) {}


    //--------------------------------------------------
    // Methods
    //--------------------------------------------------
  public:
    bool init();
    void reset();

    i2c_inst_t* get_i2c() const;
    int get_address() const;
    int get_sda() const;
    int get_scl() const;
    int get_int() const;

    uint8_t part_id();
    uint8_t revision_id();
    uint8_t manufacturer_id();

    bool get_reading();
    void interrupts(bool light, bool proximity);
    void proximity_led(uint8_t current, uint8_t duty_cycle, uint8_t pulse_freq, uint8_t num_pulses);
    void light_control(bool active, uint8_t gain);
    void proximity_control(bool active, bool saturation_indicator);
    void light_threshold(uint16_t lower, uint16_t upper);
    void proximity_threshold(uint16_t lower, uint16_t upper);
    void light_measurement_rate(uint16_t integration_time, uint16_t rate);
    void proximity_measurement_rate(uint16_t rate);
    void proximity_offset(uint16_t offset);

    template<auto T>
    const uint16_t lookup(uint16_t value) {
      size_t length = sizeof(T) / sizeof(uint16_t);
      for(auto i = 0u; i < length; i++) {
        if(T[i] == value) return i;
      }
      return 0;
    }

  private:
    uint16_t bit12_to_uint16(uint16_t value);
    uint16_t uint16_to_bit12(uint16_t value);
  };

}