#pragma once

#include "hardware/i2c.h"
#include "hardware/gpio.h"
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
    uint16_t lux;
  } ltr559_reading;

  class lookup {
    private:
      std::vector<uint16_t> lut;
    public:
      lookup(std::initializer_list<uint16_t> values);
      uint8_t index(uint16_t value);
      uint16_t value(uint8_t index);
  };

  class LTR559 {
    //--------------------------------------------------
    // Constants
    //--------------------------------------------------
  public:
    static const uint8_t DEFAULT_I2C_ADDRESS    = 0x23;
    static const uint8_t DEFAULT_SDA_PIN        = 20;
    static const uint8_t DEFAULT_SCL_PIN        = 21;
    static const uint8_t DEFAULT_INT_PIN        = 22;
    static const uint8_t PIN_UNUSED             = UINT8_MAX;

  private:
    const int ch0_c[4] = {17743, 42785, 5926, 0};
    const int ch1_c[4] = {-11059, 19548, -1185, 0};


    //--------------------------------------------------
    // Variables
    //--------------------------------------------------
  public:
    ltr559_reading data;

  private:
    i2c_inst_t *i2c = i2c0;

    // interface pins with our standard defaults where appropriate
    int8_t address    = DEFAULT_I2C_ADDRESS;
    int8_t sda        = DEFAULT_SDA_PIN;
    int8_t scl        = DEFAULT_SCL_PIN;
    int8_t interrupt  = DEFAULT_INT_PIN;

    static pimoroni::lookup lookup_led_current; 
    static pimoroni::lookup lookup_led_duty_cycle;
    static pimoroni::lookup lookup_led_pulse_freq;
    static pimoroni::lookup lookup_proximity_meas_rate;
    static pimoroni::lookup lookup_light_integration_time;
    static pimoroni::lookup lookup_light_repeat_rate;
    static pimoroni::lookup lookup_light_gain;


    //--------------------------------------------------
    // Constructors/Destructor
    //--------------------------------------------------
  public:
    LTR559() {}

    LTR559(uint8_t address) :
      address(address) {}

    LTR559(i2c_inst_t *i2c, uint8_t address, uint8_t sda, uint8_t scl, uint8_t interrupt = PIN_UNUSED) :
      i2c(i2c), address(address), sda(sda), scl(scl), interrupt(interrupt) {}


    //--------------------------------------------------
    // Methods
    //--------------------------------------------------
  public:
    bool init();
    void reset();

    i2c_inst_t* get_i2c() const;
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

  private:
    uint16_t bit12_to_uint16(uint16_t value);
    uint16_t uint16_to_bit12(uint16_t value);

    // From i2cdevice
    int write_bytes(uint8_t reg, uint8_t *buf, int len);
    int read_bytes(uint8_t reg, uint8_t *buf, int len);
    uint8_t get_bits(uint8_t reg, uint8_t shift, uint8_t mask=0b1);
    void set_bits(uint8_t reg, uint8_t shift, uint8_t mask=0b1);
    void clear_bits(uint8_t reg, uint8_t shift, uint8_t mask=0b1);
  };

}