#pragma once

#include "hardware/i2c.h"
#include "hardware/gpio.h"

namespace pimoroni {
  typedef struct  {
    uint16_t r;
    uint16_t g;
    uint16_t b;
    uint16_t c;
  } rgbc_t;

  class BH1745 {
    //--------------------------------------------------
    // Constants
    //--------------------------------------------------
  public:
    static const uint8_t DEFAULT_I2C_ADDRESS    = 0x38;
    static const uint8_t I2C_ADDRESS_ALTERNATE  = 0x39;
    static const uint8_t DEFAULT_SDA_PIN        = 20;
    static const uint8_t DEFAULT_SCL_PIN        = 21;

  private:
    static const uint8_t CHIP_ID      = 0b001011;
    static const uint8_t MANUFACTURER = 0xe0;


    //--------------------------------------------------
    // Variables
    //--------------------------------------------------
  private:
    i2c_inst_t *i2c = i2c0;

    // interface pins with our standard defaults where appropriate
    int8_t address   = DEFAULT_I2C_ADDRESS;
    int8_t sda       = DEFAULT_SDA_PIN;
    int8_t scl       = DEFAULT_SCL_PIN;

    float channel_compensation[4] = {2.2f, 1.0f, 1.8f, 10.0f};


    //--------------------------------------------------
    // Constructors/Destructor
    //--------------------------------------------------
  public:
    BH1745() {}

    BH1745(uint8_t address) :
    address(address) {}

    BH1745(i2c_inst_t *i2c, uint8_t address, uint8_t sda, uint8_t scl) :
      i2c(i2c), address(address), sda(sda), scl(scl) {}


    //--------------------------------------------------
    // Methods
    //--------------------------------------------------
  public:
    bool init();
    void reset();

    uint8_t get_chip_id();
    uint8_t get_manufacturer();
    void set_threshold_high(uint16_t value);
    void set_threshold_low(uint16_t value);
    void set_measurement_time_ms(uint16_t value);
    rgbc_t get_rgbc_raw();
    rgbc_t get_rgb_clamped();
    rgbc_t get_rgb_scaled();    
    void set_leds(bool state = true);

  private:
    // From i2cdevice
    int write_bytes(uint8_t reg, uint8_t *buf, int len);
    int read_bytes(uint8_t reg, uint8_t *buf, int len);
    uint8_t get_bits(uint8_t reg, uint8_t shift, uint8_t mask = 0b1);
    void set_bits(uint8_t reg, uint8_t shift, uint8_t mask = 0b1);
    void clear_bits(uint8_t reg, uint8_t shift, uint8_t mask = 0b1);
  };

}
