#pragma once

#include "hardware/i2c.h"
#include "hardware/gpio.h"

namespace pimoroni {

  enum INT_STATUS : uint8_t {
    // INT_STATUS_1
    a_full = 0b10000000,
    data_ready = 0b01000000,
    alc_overflow = 0b00100000,
    prox_int = 0b00010000,
    pwr_ready = 0b00000001,

    // INT_STATUS_2
    die_temp_ready = 0b00000010
  };

  enum SPO2_CONFIG : uint8_t {
    adc_range_2048nA = 0b00 << 5,
    adc_range_4096nA = 0b01 << 5,
    adc_range_8192nA = 0b10 << 5,
    adc_range_16384nA = 0b11 << 5,
    sample_rate_50sps = 0b000 << 2,
    sample_rate_100sps = 0b001 << 2,
    sample_rate_200sps = 0b010 << 2,
    sample_rate_400sps = 0b011 << 2,
    sample_rate_800sps = 0b100 << 2,
    sample_rate_1000sps = 0b101 << 2,
    sample_rate_1600sps = 0b110 << 2,
    sample_rate_3200sps = 0b111 << 2,
    led_pulse_width_69us = 0b00,
    led_pulse_width_118us = 0b01,
    led_pulse_width_215us = 0b10,
    led_pulse_width_411us = 0b11
  };

  enum FIFO_CONFIG : uint8_t {
    sample_average_1x = 0b000 << 5,
    sample_average_2x = 0b001 << 5,
    sample_average_4x = 0b010 << 5,
    sample_average_8x = 0b011 << 5,
    sample_average_16x = 0b100 << 5,
    sample_average_32x = 0b101 << 5,
    fifo_rollover_en = 0b1 << 4,
  };

  enum LED_MODE : uint8_t {
    off = 0b000,
    red = 0b001,
    ir = 0b010,
    green = 0b011,
    //off = 0b100,
    pilot_red = 0b101,
    pilot_ir = 0b110,
    pilot_green = 0b111
  };

  enum MODE : uint8_t {
    none = 0b000,
    red_only = 0b010,
    red_ir = 0b011,
    green_red_ir = 0b111
  };

  class MAX30105 {
    //--------------------------------------------------
    // Constants
    //--------------------------------------------------
  public:
    static const uint8_t DEFAULT_I2C_ADDRESS  = 0x57;
    static const uint8_t DEFAULT_SDA_PIN      = 20;
    static const uint8_t DEFAULT_SCL_PIN      = 21;
    static const uint8_t DEFAULT_INT_PIN      = 22;
    static const uint8_t PIN_UNUSED           = UINT8_MAX;

    /***** More public constants here *****/
    static const uint8_t CHIP_ID    = 0x15;

  private:
    /***** Private constants here *****/


    //--------------------------------------------------
    // Variables
    //--------------------------------------------------
  private:
    i2c_inst_t *i2c   = i2c0;

    // interface pins with our standard defaults where appropriate
    int8_t address    = DEFAULT_I2C_ADDRESS;
    int8_t sda        = DEFAULT_SDA_PIN;
    int8_t scl        = DEFAULT_SCL_PIN;
    int8_t interrupt  = DEFAULT_INT_PIN;

    /***** More variables here *****/
    int leds_enabled  = 0;


    //--------------------------------------------------
    // Constructors/Destructor
    //--------------------------------------------------
  public:
    MAX30105() {}

    MAX30105(i2c_inst_t *i2c, uint8_t sda, uint8_t scl, uint8_t interrupt = PIN_UNUSED) :
      i2c(i2c), sda(sda), scl(scl), interrupt(interrupt) {}


    //--------------------------------------------------
    // Methods
    //--------------------------------------------------
  public:
    bool init(
    float led_power = 6.4,
    enum FIFO_CONFIG sample_average = sample_average_1x,
    int leds_enabled = 3,
    enum SPO2_CONFIG sample_rate = sample_rate_400sps,
    enum SPO2_CONFIG pulse_width = led_pulse_width_215us,
    enum SPO2_CONFIG adc_range = adc_range_16384nA,
    int timeout = 5000);

    /***** More public methods here *****/

    void soft_reset(int timeout = 5000);
    void clear_fifo();
    uint8_t get_samples(uint32_t *samples);
    uint8_t get_chip_id();
    float get_temperature(int timeout = 5000);
    void set_mode(MODE mode);
    void set_slot_mode(uint8_t slot, LED_MODE mode);
    void set_led_pulse_amplitude(uint8_t led, float amplitude);
    void set_fifo_almost_full_count(uint8_t count);
    void set_fifo_almost_full_enable(bool enabled);
    void set_data_ready_enable(bool enabled);
    void set_ambient_light_compensation_overflow_enable(bool enabled);
    void set_proximity_enable(bool enabled);
    void set_proximity_threshold(uint8_t value);
    bool get_fifo_almost_full_status();
    bool get_data_ready_status();
    bool get_ambient_light_compensation_overflow_status();
    bool get_proximity_triggered_threshold_status();
    bool get_power_ready_status();
    bool get_die_temp_ready_status();

  private:
    /***** Private methods here *****/
    enum reg : uint8_t {
      INT_STATUS_1 = 0x00,
      INT_STATUS_2 = 0x01,
      INT_ENABLE_1 = 0x02,
      INT_ENABLE_2 = 0x03,
      FIFO_WRITE = 0x04,
      FIFO_OVERFLOW = 0x05,
      FIFO_READ = 0x06,
      FIFO_DATA = 0x07,
      FIFO_CONFIG = 0x08,
      MODE_CONFIG = 0x09,
      SPO2_CONFIG = 0x0A,
      LED1_PULSE_AMPLITUDE = 0x0C,
      LED2_PULSE_AMPLITUDE = 0x0D,
      LED3_PULSE_AMPLITUDE = 0x0E,
      LED_PROX_PULSE_AMPLITUDE = 0x10,
      LED_MODE_CONTROL_2_1 = 0x11,
      LED_MODE_CONTROL_4_3 = 0x12,
      DIE_TEMP = 0x1f,
      DIE_TEMP_CONFIG = 0x21,
      PROX_INT_THRESHOLD = 0x30,
      REVISION = 0xfe,
      PART_ID = 0xff
    };

    // From i2cdevice
    int write_bytes(uint8_t reg, uint8_t *buf, int len);
    int read_bytes(uint8_t reg, uint8_t *buf, int len);
    uint8_t get_bits(uint8_t reg, uint8_t shift, uint8_t mask=0b1);
    void set_bits(uint8_t reg, uint8_t shift, uint8_t mask=0b1);
    void clear_bits(uint8_t reg, uint8_t shift, uint8_t mask=0b1);
    void change_bits(uint8_t reg, uint8_t mask, uint8_t value);
  };

}
