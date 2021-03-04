#include "bme280.hpp"
#include <algorithm>

namespace pimoroni {

  lookup::lookup(std::initializer_list<float> values) : lut(values) {}

  uint8_t lookup::index(float value) {
    auto it = find(lut.begin(), lut.end(), value);

    if(it == lut.end())
      return 0;

    return it - lut.begin();
  }

  float lookup::value(uint8_t index) {
    return lut[index];
  }

  /***** Device registers and masks here *****/


  pimoroni::lookup BME280::lookup_temp_standby_duration({
    0.5f,
    62.5f,
    125.0f,
    250.0f,
    500.0f,
    1000.0f,
    10.0f,
    20.0f
  });

  bool BME280::init(bme280_mode mode,
    bme280_oversampling temperature_oversampling,
    bme280_oversampling pressure_oversampling,
    bme280_oversampling humidity_oversampling,
    float temperature_standby) {

    i2c_init(i2c, 400000);

    gpio_set_function(sda, GPIO_FUNC_I2C);
    gpio_pull_up(sda);
    gpio_set_function(scl, GPIO_FUNC_I2C);
    gpio_pull_up(scl);

    uint8_t val;

    read_bytes(BME280_CHIP_ID, &val, 1);
    if(val != 0x60 && val != 0x58) {
      // 0x60 == BME280
      // 0x58 == BMP280
      return false;
    }

    if(val == 0x58) {
      has_humidity = false;
    }

    val = 0xb6;
    write_bytes(BME280_RESET, &val, 1);
    sleep_ms(100);

    val = humidity_oversampling;
    write_bytes(BME280_CTRL_HUM, &val, 1);

    val = (temperature_oversampling << 5) | (pressure_oversampling << 2) | mode;
    write_bytes(BME280_CTRL_MEAS, &val, 1);

    val = (lookup_temp_standby_duration.index(temperature_standby) << 5) | (2 << 2);
    write_bytes(BME280_CONIG, &val, 1);

    /***** Replace if(true) with any operations needed to initialise the device *****/

    read_bytes(BME280_CALIBRATION, (uint8_t *)&calibration_data, 25); // t1 to h1

    if(has_humidity) {
      uint8_t test[3];
      read_bytes(BME280_CALIBRATION2, test, 3); // h2, h3
      //read_bytes(BME280_CALIBRATION2, (uint8_t *)(&calibration_data + 25), 3); // h2, h3

      calibration_data.h2 = (test[1] << 8) | test[0];
      calibration_data.h3 = test[2];

      uint8_t buf[4];
      read_bytes(BME280_CALIBRATION2 + 3, buf, 4); // h4, h5, h6 (0xE4 to 0xE7)
      // h4 = buf[0]   = 0xe4
      // h5 = buf[1:2] = 0xe5 0xe6
      // h6 = buf[3]   = 0xe7
      calibration_data.h4 = (buf[0] << 4) | (buf[1] & 0xF);
      calibration_data.h5 = (buf[2] << 4) | (buf[1] >> 4);
      calibration_data.h6 = (int8_t)buf[3];
    }

    return true;
  }

  void BME280::update_sensor() {
    uint8_t buf[8] = {0, 0, 0, 0, 0, 0, 0, 0};
    read_bytes(BME280_DATA, buf, 8);
    int32_t raw_pressure = (buf[0] << 16) | (buf[1] << 8) | buf[2];
    int32_t raw_temperature = (buf[3] << 16) | (buf[4] << 8) | buf[5];
    int32_t raw_humidity = (buf[6] << 8) | buf[7];

    raw_temperature >>= 4;

    int32_t var1 = ((((raw_temperature >> 3) - ((int32_t)calibration_data.t1 << 1))) *
            ((int32_t)calibration_data.t2)) >>
          11;

    int32_t var2 = (((((raw_temperature >> 4) - ((int32_t)calibration_data.t1)) *
              ((raw_temperature >> 4) - ((int32_t)calibration_data.t1))) >>
            12) *
            ((int32_t)calibration_data.t3)) >>
          14;

    int32_t t_fine = var1 + var2;

    temperature = (t_fine * 5 + 128) >> 8;
    temperature /= 100.0f;

    var1 = (t_fine - ((int32_t)76800));

    // Pressure Compensation
    raw_pressure >>= 4;

    int64_t varp1, varp2, p;

    varp1 = ((int64_t)t_fine) - 128000;
    varp2 = varp1 * varp1 * (int64_t)calibration_data.p6;
    varp2 = varp2 + ((varp1 * (int64_t)calibration_data.p5) << 17);
    varp2 = varp2 + (((int64_t)calibration_data.p4) << 35);
    varp1 = ((varp1 * varp1 * (int64_t)calibration_data.p3) >> 8) +
          ((varp1 * (int64_t)calibration_data.p2) << 12);
    varp1 =
        (((((int64_t)1) << 47) + varp1)) * ((int64_t)calibration_data.p1) >> 33;

    if (varp1 == 0) {
      pressure = 0.0f;
    }
    else {
      p = 1048576 - raw_pressure;
      p = (((p << 31) - varp2) * 3125) / varp1;
      varp1 = (((int64_t)calibration_data.p9) * (p >> 13) * (p >> 13)) >> 25;
      varp2 = (((int64_t)calibration_data.p8) * p) >> 19;

      p = ((p + varp1 + varp2) >> 8) + (((int64_t)calibration_data.p7) << 4);
      pressure = (float)p / 256;
    }

    if(!has_humidity) return;
  
    // Humidity Compensation
    var1 = (((((raw_humidity << 14) - (((int32_t)calibration_data.h4) << 20) -
                    (((int32_t)calibration_data.h5) * var1)) +
                  ((int32_t)16384)) >>
                  15) *
                (((((((var1 * ((int32_t)calibration_data.h6)) >> 10) *
                      (((var1 * ((int32_t)calibration_data.h3)) >> 11) +
                      ((int32_t)32768))) >>
                    10) +
                    ((int32_t)2097152)) *
                      ((int32_t)calibration_data.h2) +
                  8192) >>
                  14));

    var1 = (var1 - (((((var1 >> 15) * (var1 >> 15)) >> 7) *
                              ((int32_t)calibration_data.h1)) >>
                              4));

    var1 = (var1 < 0) ? 0 : var1;
    var1 = (var1 > 419430400) ? 419430400 : var1;
    humidity = (var1 >> 12) / 1024.0f;
  }

  float BME280::get_temperature() {
    return temperature;
  }

  float BME280::get_pressure() {
    return pressure;
  }

  float BME280::get_humidity() {
    return humidity;
  }

  float BME280::get_altitude(float qnh) {
    return 0;
  }

  // i2c functions

  int BME280::write_bytes(uint8_t reg, uint8_t *buf, int len) {
    uint8_t buffer[len + 1];
    buffer[0] = reg;
    for(int x = 0; x < len; x++) {
      buffer[x + 1] = buf[x];
    }
    return i2c_write_blocking(i2c, address, buffer, len + 1, false);
  };

  int BME280::read_bytes(uint8_t reg, uint8_t *buf, int len) {
    i2c_write_blocking(i2c, address, &reg, 1, true);
    i2c_read_blocking(i2c, address, buf, len, false);
    return len;
  };

  uint8_t BME280::get_bits(uint8_t reg, uint8_t shift, uint8_t mask) {
    uint8_t value;
    read_bytes(reg, &value, 1);
    return value & (mask << shift);
  }

  void BME280::set_bits(uint8_t reg, uint8_t shift, uint8_t mask) {
    uint8_t value;
    read_bytes(reg, &value, 1);
    value |= mask << shift;
    write_bytes(reg, &value, 1);
  }

  void BME280::clear_bits(uint8_t reg, uint8_t shift, uint8_t mask) {
    uint8_t value;
    read_bytes(reg, &value, 1);
    value &= ~(mask << shift);
    write_bytes(reg, &value, 1);
  }
}