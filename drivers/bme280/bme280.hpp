#pragma once

#include <vector>
#include <initializer_list>
#include "hardware/i2c.h"
#include "hardware/gpio.h"

#define BME280_CHIP_ID 0xD0
#define BME280_RESET 0xE0
#define BME280_STATUS 0xF3
#define BME280_CTRL_MEAS 0xF4
#define BME280_CTRL_HUM 0xF2
#define BME280_CONIG 0xF5
#define BME280_DATA 0xF7
#define BME280_CALIBRATION 0x88
#define BME280_CALIBRATION2 0xE1

namespace pimoroni {

  class lookup {
    private:
      std::vector<float> lut;
    public:
      lookup(std::initializer_list<float> values);
      uint8_t index(float value);
      float value(uint8_t index);
  };

  struct bme280_raw_data {
    uint16_t humidity;
    uint16_t temperature;
    uint16_t pressure;
  };

  struct bme280_calibration_data {
    uint16_t t1;
    int16_t t2;
    int16_t t3;

    uint16_t p1;
    int16_t p2;
    int16_t p3;
    int16_t p4;
    int16_t p5;
    int16_t p6;
    int16_t p7;
    int16_t p8;
    int16_t p9;

    uint8_t h1;
    int16_t h2;
    uint8_t h3;
    int16_t h4;
    int16_t h5;
    int8_t h6;
  };

  enum bme280_mode: uint8_t {
    SLEEP = 0b01,
    FORCED = 0b10,
    NORMAL = 0b11
  };

  enum bme280_oversampling: uint8_t {
    X1  = 0b001,
    X2  = 0b010,
    X4  = 0b011,
    X8  = 0b100,
    X16 = 0b101
  };

  class BME280 {
    //--------------------------------------------------
    // Constants
    //--------------------------------------------------
  public:
    static const uint8_t DEFAULT_I2C_ADDRESS    = 0x76;
    static const uint8_t I2C_ADDRESS_ALTERNATE  = 0x77;
    static const uint8_t DEFAULT_SDA_PIN        = 20;
    static const uint8_t DEFAULT_SCL_PIN        = 21;
    bool has_humidity = true;

    /***** More public constants here *****/

  private:
    //static const uint8_t CHIP_ID  = 0x60;


    //--------------------------------------------------
    // Variables
    //--------------------------------------------------
  private:
    i2c_inst_t *i2c   = i2c0;

    // interface pins with our standard defaults where appropriate
    int8_t address    = DEFAULT_I2C_ADDRESS;
    int8_t sda        = DEFAULT_SDA_PIN;
    int8_t scl        = DEFAULT_SCL_PIN;

    /***** More variables here *****/
    bme280_calibration_data calibration_data;
    static pimoroni::lookup lookup_temp_standby_duration;
    float temperature;
    float pressure;
    float humidity;


    //--------------------------------------------------
    // Constructors/Destructor
    //--------------------------------------------------
  public:
    BME280() {}

    BME280(uint8_t address) :
      address(address) {}

    BME280(i2c_inst_t *i2c, uint8_t address, uint8_t sda, uint8_t scl) :
      i2c(i2c), address(address), sda(sda), scl(scl) {}


    //--------------------------------------------------
    // Methods
    //--------------------------------------------------
  public:
    bool init(bme280_mode mode=bme280_mode::NORMAL,
    bme280_oversampling temperature_oversampling=bme280_oversampling::X16,
    bme280_oversampling pressure_oversampling=bme280_oversampling::X16,
    bme280_oversampling humidity_oversampling=bme280_oversampling::X16,
    float temperature_standby=500);

    void update_sensor();
    float get_temperature();
    float get_pressure();
    float get_humidity();
    float get_altitude(float qnh = 1013.25f);

  private:
    /***** Private methods here *****/
    // From i2cdevice
    int write_bytes(uint8_t reg, uint8_t *buf, int len);
    int read_bytes(uint8_t reg, uint8_t *buf, int len);
    uint8_t get_bits(uint8_t reg, uint8_t shift, uint8_t mask=0b1);
    void set_bits(uint8_t reg, uint8_t shift, uint8_t mask=0b1);
    void clear_bits(uint8_t reg, uint8_t shift, uint8_t mask=0b1);
  };

}
