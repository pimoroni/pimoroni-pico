#include "pcf85063a.hpp"

#include <chrono>

#include "hardware/i2c.h"
#include "hardware/rtc.h"

namespace pimoroni {

  const uint8_t REG_CONTROL_1         = 0x00;
  const uint8_t REG_CONTROL_2         = 0x01;
  const uint8_t REG_OFFSET            = 0x02;
  const uint8_t REG_RAM_BYTE          = 0x03;

  const uint8_t REG_SECONDS           = 0x04;
  const uint8_t REG_MINUTES           = 0x05;
  const uint8_t REG_HOURS             = 0x06;
  const uint8_t REG_DAYS              = 0x07;
  const uint8_t REG_WEEKDAYS          = 0x08;
  const uint8_t REG_MONTHS            = 0x09;
  const uint8_t REG_YEARS             = 0x0a;

  const uint8_t REG_SECOND_ALARM      = 0x0b;
  const uint8_t REG_MINUTE_ALARM      = 0x0c;
  const uint8_t REG_HOUR_ALARM        = 0x0d;
  const uint8_t REG_DAY_ALARM         = 0x0e;
  const uint8_t REG_WEEKDAY_ALARM     = 0x0f;

  const uint8_t REG_TIMER_VALUE       = 0x10;
  const uint8_t REG_TIMER_MODE        = 0x11;


  void PCF85063A::init() {
    // configure i2c interface and pins
    i2c_init(i2c, i2c_baud);

    gpio_set_function(sda, GPIO_FUNC_I2C);
    gpio_set_function(scl, GPIO_FUNC_I2C);

    if(interrupt != PIN_UNUSED) {
      gpio_set_function(interrupt, GPIO_FUNC_SIO);
      gpio_set_dir(interrupt, GPIO_IN);
      gpio_set_pulls(interrupt, false, true);
    }
  }

  void PCF85063A::configure(uint8_t flags) {
    static uint8_t command[2] = {REG_CONTROL_2, flags};
    i2c_write_blocking(i2c, I2C_ADDRESS, command, 2, false);
  }

  void PCF85063A::reset() {
    static uint8_t command[2] = {REG_CONTROL_1, COMMAND_RESET};
    i2c_write_blocking(i2c, I2C_ADDRESS, command, 2, false);
  }

  uint8_t bcd_encode(uint v) {
    uint v10 = v / 10;
    uint v1 = v - (v10 * 10);
    return v1 | (v10 << 4);
  }

  int8_t bcd_decode(uint v) {    
    uint v10 = (v >> 4) & 0x0f;
    uint v1 = v & 0x0f;
    return v1 + (v10 * 10);
  }

  bool PCF85063A::set_datetime(datetime_t *t) {
    /*if (!valid_datetime(t)) {
      return false;
    }    */

    static uint8_t command[8] = {0};

    command[0] = REG_SECONDS;
    command[1] = bcd_encode((uint)t->sec);
    command[2] = bcd_encode((uint)t->min);
    command[3] = bcd_encode((uint)t->hour);
    command[4] = bcd_encode((uint)t->dotw);
    command[5] = bcd_encode((uint)t->day);
    command[6] = bcd_encode((uint)t->month);
    command[7] = bcd_encode((uint)t->year - 2000);

    i2c_write_blocking(i2c, I2C_ADDRESS, command, 8, false);

    return true;
  }

  datetime_t PCF85063A::get_datetime() {
    static uint8_t result[7] = {0};

    i2c_write_blocking(i2c, I2C_ADDRESS, &REG_SECONDS, 1, false);
    i2c_read_blocking(i2c, I2C_ADDRESS, result, 7, false);

    datetime_t dt = {
      .year   = (int16_t)(bcd_decode(result[6]) + 2000),
      .month  = (int8_t)bcd_decode(result[5]),
      .day    = (int8_t)bcd_decode(result[4]),
      .dotw   = (int8_t)bcd_decode(result[3]),
      .hour   = (int8_t)bcd_decode(result[2]),
      .min    = (int8_t)bcd_decode(result[1]),
      .sec    = (int8_t)bcd_decode(result[0] & 0x7f)
    };

    return dt;
  }

  void PCF85063A::set_second_alarm(uint sec) {
    uint8_t se = bcd_encode((uint)sec);
    se |= 0x80; // enable alarm bit
    static uint8_t command[2] = {REG_SECOND_ALARM, se};
    i2c_write_blocking(i2c, I2C_ADDRESS, command, 2, false);
  }

  void PCF85063A::reset_timer() {    
    static uint8_t command[2] = {REG_TIMER_MODE, 0b00000000};
    i2c_write_blocking(i2c, I2C_ADDRESS, command, 2, false);
  }

  void PCF85063A::set_seconds_timer(uint8_t sec) {    
    reset_timer();
    static uint8_t command1[2] = {REG_TIMER_VALUE, sec};
    i2c_write_blocking(i2c, I2C_ADDRESS, command1, 2, false);

    static uint8_t command2[2] = {REG_TIMER_MODE, 0b00010110};
    i2c_write_blocking(i2c, I2C_ADDRESS, command2, 2, false);
  }

  i2c_inst_t* PCF85063A::get_i2c() const {
    return i2c;
  }

  int PCF85063A::get_sda() const {
    return sda;
  }

  int PCF85063A::get_scl() const {
    return scl;
  }

  int PCF85063A::get_interrupt() const {
    return interrupt;
  }

}