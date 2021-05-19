/******************************************************************************
sgp30.cpp
Code based on "Sensirion_Gas_Sensors_Datasheet_SGP30.pdf" on sensirion.com.
Code written by Simon Reap, March 17, 2021
https://github.com/simon3270/pico-pimoroni

This code is released under the [MIT License](http://opensource.org/licenses/MIT).
Please review the LICENSE file included with this example.
Distributed as-is; no warranty is given.
******************************************************************************/

#include "sgp30.hpp"

namespace pimoroni {

  bool SGP30::init() {
    soft_reset();

    if(!retrieve_unique_id()) {
      return false;
    }

    // Retrieve and check Feature Set
    uint16_t featureset;
    if(!read_reg_1_word(GET_FEATURE_SET_VERSION, 10, &featureset))
      return false;
    if((featureset & 0xF0) != SGP30_REQ_FEATURES)
      return false;

    // Start the measurement process
    // - parameter true = wait for readings to initialise
    //             false = return immediately
    // As usual, function returns true if the request succeeded
    // if (!start_measurement(true))
    //   return false;

    return true;
  }

  i2c_inst_t* SGP30::get_i2c() const {
    return i2c->get_i2c();
  }

  int SGP30::get_sda() const {
    return i2c->get_sda();
  }

  int SGP30::get_scl() const {
    return i2c->get_scl();
  }

  // Get the unique ID from the Chip. Will fail if no chip attached
  bool SGP30::retrieve_unique_id() {
    // return the Chip ID, in three separate 16-bit values
    return read_reg_3_words(GET_SERIAL_ID, 10, serial_number, serial_number + 1, serial_number + 2);
  }

  // get the previously-retreved Chip ID as the lower 48 bits of a 64-bit uint
  uint64_t SGP30::get_unique_id() {
    return (((uint64_t)serial_number[0]) << 32) \
      + (((uint64_t)serial_number[1]) << 16) \
      + serial_number[2];
  }

  // Write a soft reset - writes globally, so all devices on this
  // I2C bus receive the request
  bool SGP30::soft_reset() {
    return write_global(SOFT_RESET, 10);
  }

  // Start the measurement process.
  // If the parameter is true, wait for the readings to be valid
  // If false, return immediately
  bool SGP30::start_measurement(bool wait_for_setup) {
    // First kick off the "measurement" phase
    bool rc = write_reg(INIT_AIR_QUALITY, 10);

    // Optionally wait up to 20 seconds for the measurement process to initiate
    if(wait_for_setup) {
      // It takes 15 seconds to start the measurement process but allow 20.
      // Ignore the first 2 readings completely.
      uint16_t eCO2, TVOC;
      uint8_t sec_count = 0;
      while(sec_count < 20) {
        sleep_ms(988); // Will sleep last 12ms of 1sec in get_air_quality()
        get_air_quality(&eCO2, &TVOC);
        if((sec_count >= 2) && (eCO2 != 400 || TVOC != 0)) {
          // startup process finished
          break;
        }
        sec_count++;
      }
    }
    return rc;
  }

  // get the air quality values - will be 400 and 0 respectively for the
  // first 15 seconds after starting measurement
  bool SGP30::get_air_quality(uint16_t * eCO2, uint16_t * TVOC) {
    return read_reg_2_words(MEASURE_AIR_QUALITY, 12, eCO2, TVOC);
  }

  // Get the raw readings - not useful in real-world settings
  bool SGP30::get_air_quality_raw(uint16_t * rawH2, uint16_t * rawEthanol) {
    return read_reg_2_words(MEASURE_RAW_SIGNALS, 25, rawH2, rawEthanol);
  }

  // Get the baseline compensation values for eCO2 and VOC
  bool SGP30::get_baseline(uint16_t *eco2, uint16_t *tvoc) {
    return read_reg_2_words(GET_BASELINE, 10, eco2, tvoc);
  }

  // Write the baseline compensation values for eCO2 and VOC
  void SGP30::set_baseline(uint16_t eco2, uint16_t tvoc) {
    write_reg_2_words(SET_BASELINE, 10, eco2, tvoc);
  }

  // Set the absolute humidity, e.g. from an SHTxx chip
  bool SGP30::set_humidity(uint32_t absolute_humidity) {
    if(absolute_humidity > 256000) {
      return false;
    }

    uint16_t ah_scaled = (uint16_t)(((uint64_t)absolute_humidity * 256 * 16777) >> 24);

    return write_reg_1_word(SET_HUMIDITY, 10, ah_scaled);
  }

  // Write a single byte globally (not to a specifc I2c address)
  bool SGP30::write_global(uint16_t reg, uint16_t delay_ms) {
    uint8_t buffer[1] = { (uint8_t)(reg & 0xFF)};
    i2c->write_blocking(0, buffer, 1, false);
    sleep_ms(delay_ms);
    return true;
  }

  // Write just the register to the i2c address, no parameter
  bool SGP30::write_reg(uint16_t reg, uint16_t delay_ms) {
    uint8_t buffer[2] = { (uint8_t)((reg >> 8) & 0xFF), (uint8_t)(reg & 0xFF)};
    i2c->write_blocking(address, buffer, 2, false);
    sleep_ms(delay_ms);
    return true;
  }

  // Write one 16-bit word (+CRC)
  bool SGP30::write_reg_1_word(uint16_t reg, uint16_t delay_ms, uint16_t value) {
    uint8_t buffer[5] = { (uint8_t)((reg >> 8) & 0xFF), (uint8_t)(reg & 0xFF),
                          (uint8_t)((value >> 8) & 0xFF), (uint8_t)(value & 0xFF), calculate_crc(value)};
    i2c->write_blocking(address, buffer, 5, false);
    sleep_ms(delay_ms);
    return true;
  }

  // Write two 16-bit words (+CRC)
  bool SGP30::write_reg_2_words(uint16_t reg, uint16_t delay_ms, uint16_t value1, uint16_t value2) {
    uint8_t buffer[8] = { (uint8_t)((reg >> 8) & 0xFF), (uint8_t)(reg & 0xFF),
                          (uint8_t)((value1 >> 8) & 0xFF), (uint8_t)(value1 & 0xFF), calculate_crc(value1),
                          (uint8_t)((value2 >> 8) & 0xFF), (uint8_t)(value2 & 0xFF), calculate_crc(value2)};
    i2c->write_blocking(address, buffer, 8, false);
    sleep_ms(delay_ms);
    return true;
  }

  // Write register and read one 16-bit word in response
  bool SGP30::read_reg_1_word(uint16_t reg, uint16_t delay_ms, uint16_t *value) {
    uint8_t regbuf[2] = { (uint8_t)((reg >> 8) & 0xFF), (uint8_t)(reg & 0xFF) };
    uint8_t buffer[3];
    i2c->write_blocking(address, regbuf, 2, true);
    sleep_ms(delay_ms);
    i2c->read_blocking(address, buffer, 3, false);
    if(buffer[2] != calculate_crc(buffer[0], buffer[1]))
      return false;
    *value = (buffer[0] << 8) + buffer[1];
    return true;
  }

  // Write register and read two 16-bit words
  bool SGP30::read_reg_2_words(uint16_t reg, uint16_t delay_ms, uint16_t *value1, uint16_t *value2) {
    uint8_t regbuf[2] = { (uint8_t)((reg >> 8) & 0xFF), (uint8_t)(reg & 0xFF) };
    uint8_t buffer[6];
    i2c->write_blocking(address, regbuf, 2, true);
    sleep_ms(delay_ms);
    i2c->read_blocking(address, buffer, 6, false);
    if((buffer[2] != calculate_crc(buffer[0], buffer[1])) || (buffer[5] != calculate_crc(buffer[3], buffer[4]))) {
      return false;
    }
    *value1 = (buffer[0] << 8) + buffer[1];
    *value2 = (buffer[3] << 8) + buffer[4];
    return true;
  }

  // Write register and read three 16-bit words
  bool SGP30::read_reg_3_words(uint16_t reg, uint16_t delay_ms, uint16_t *value1, uint16_t *value2, uint16_t *value3) {
    uint8_t regbuf[2] = { (uint8_t)((reg >> 8) & 0xFF), (uint8_t)(reg & 0xFF) };
    uint8_t buffer[9];
    i2c->write_blocking(address, regbuf, 2, true);
    sleep_ms(delay_ms);
    i2c->read_blocking(address, buffer, 9, false);
    if(buffer[2] != calculate_crc(buffer[0], buffer[1])) {
      return false;
    }
    if (buffer[5] != calculate_crc(buffer[3], buffer[4])) {
      return false;
    }
    if (buffer[8] != calculate_crc(buffer[6], buffer[7])) {
      return false;
    }
    *value1 = (buffer[0] << 8) + buffer[1];
    *value2 = (buffer[3] << 8) + buffer[4];
    *value3 = (buffer[6] << 8) + buffer[7];
    return true;
  }

  // calculate the CRC for a single 16-bit word
  uint8_t SGP30::calculate_crc(uint16_t value) {
    return calculate_crc((value >> 8) && 0xFF, value & 0xFF);
  }

  // calculate the CRC for two 8-bit bytes
  uint8_t SGP30::calculate_crc(uint8_t value1, uint8_t value2) {
    // calculates 8-Bit checksum with given polynomial
    uint8_t crc = SGP30_CRC_BASE;

    crc ^= value1;
    for(uint8_t b = 0; b < 8; b++) {
      if(crc & 0x80)
        crc = (crc << 1) ^ SGP30_CRC_SEED;
      else
        crc <<= 1;
    }
    crc ^= value2;
    for(uint8_t b = 0; b < 8; b++) {
      if(crc & 0x80)
        crc = (crc << 1) ^ SGP30_CRC_SEED;
      else
        crc <<= 1;
    }

    return crc;
  }

}
